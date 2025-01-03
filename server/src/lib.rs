use std::error::Error;
use std::net::UdpSocket;
use std::net::SocketAddr;
use std::collections::HashMap;

mod core;
mod common;

#[allow(dead_code)]
pub struct LocationUpdate {
    id: usize,
    old: core::ItemLocation,
    new: core::ItemLocation,
}

impl LocationUpdate {
    fn to_le_bytes(&self) -> Vec<u8> {
        let data = self as *const LocationUpdate;
        let data = data as *const u8;

        let mut buf = vec![];
        unsafe {
            let slice = std::slice::from_raw_parts(data, size_of::<LocationUpdate>());
            buf.extend_from_slice(slice);
        }   
        return buf;
    }
}

#[no_mangle]
extern "C" fn update_location(item: *mut core::InventoryElement, location: core::ItemLocation) {
    unsafe {
        println!("begin {} {}", (*item).location.type_, (*item).location.data.player.id);
        LOCATION_UPDATES.push(LocationUpdate{
            id: (*item).uid,
            old: (*item).location,
            new: location,
        });
        (*item).location = location;
        println!("update location, {} {} chunk {} {}", (*item).location.type_, location.type_, (*item).location.data.chunk.map_x, (*item).location.data.chunk.map_y);
    }
}

pub static mut SEED: i64 = 0;
pub static mut LOCATION_UPDATES: Vec<LocationUpdate> = vec![];

pub enum ClientEvent<'a> {
    Move{x: i32, y: i32},
    Pickup{id: usize},
    Drop{id: usize},
    ItemUsedOnObject{iid: usize, oid: usize},
    Craft{product_id: usize, ingredients_num: u32, ingredients_ids: &'a [u8]},
    Whatever,
}

pub struct Server {
    socket: UdpSocket,
    clients: HashMap<SocketAddr, usize>,
}

impl Server {
    fn broadcast(&self, data: &[u8]) {
        for client in self.clients.keys() {
            self.socket.send_to(data, client).unwrap();
        }
    }
}

impl<'a> From<&'a [u8]> for ClientEvent<'a> {
    fn from(value: &'a [u8]) -> Self {
        match value[0] {
            common::PACKET_PLAYER_MOVE => ClientEvent::Move{x: (value[1] as i8).into(), y: (value[2] as i8).into()},
            common::PACKET_PLAYER_ACTION_PICKUP => ClientEvent::Pickup{id: usize::from_le_bytes(value[1..9].try_into().unwrap())},
            common::PACKET_PLAYER_ACTION_DROP => ClientEvent::Drop{id: usize::from_le_bytes(value[1..9].try_into().unwrap())},
            common::PACKET_PLAYER_ACTION_USE_ITEM_ON_OBJECT => ClientEvent::ItemUsedOnObject{iid: usize::from_le_bytes(value[1..9].try_into().unwrap()), oid: usize::from_le_bytes(value[9..17].try_into().unwrap())},
            common::PACKET_PLAYER_ACTION_CRAFT => ClientEvent::Craft{product_id: usize::from_le_bytes(value[1..9].try_into().unwrap()), ingredients_num: ((value.len()-1)/8) as u32, ingredients_ids: &value[9..]},
            1 => ClientEvent::Whatever,
            _ => panic!("invalid event {:?}", value)
        }
    }
}

pub fn init_server() -> Result<Server, Box<dyn Error>>{
    let socket = UdpSocket::bind("127.0.0.1:1234")?;
    socket.set_nonblocking(true).unwrap();

    Ok(Server {
        socket,
        clients: HashMap::new(),
    })
}

pub fn main_loop(server: &mut Server) {
    let mut players: Vec<core::Player> = vec![];
    loop {
        handle_network(server, &mut players);
        unsafe {
            core::update();
        }
        send_game_updates(server, &mut players);
        std::thread::sleep(std::time::Duration::from_millis(100));
    }
}

fn add_player(server: &mut Server, mut peer: std::net::SocketAddr, players: &mut Vec<core::Player>) {
    // TODO
    //peer.set_data(Some(players.len()));
    server.clients.insert(peer, players.len());

    let mut response = [0 as u8; 17];
    response[0] = common::PACKET_PLAYER_ID as u8;
    response[1..9].copy_from_slice(&players.len().to_le_bytes());
    unsafe {
        response[9..17].copy_from_slice(&SEED.to_le_bytes());
    }
    server.socket.send_to(&response, peer).unwrap();

    unsafe {
        let p = core::Player::new(players.len() as i32);

        //let el = &mut core::Element::new(core::base_elements[0]) as *mut core::Element;
        //let el = el as *mut core::InventoryElement;
        //println!("item {} {}", (*el).c_id, (*el).uid);
        //(*p.inventory).add(el);
        players.push(p);
    }
    println!("{:?} , players {:?}", peer, players);
    update_chunk_for_player(server, &mut peer, (128, 128));
    update_player_inventory(server, &mut peer, players);
}


fn update_players(server: &Server, players: &mut Vec<core::Player>) {
    for (i, p) in players.iter().enumerate() {
        let mut data = [0 as u8; 25];
        data[0] = common::PACKET_PLAYER_UPDATE;
        data[1..9].clone_from_slice(&i.to_le_bytes());
        data[9..13].clone_from_slice(&p.map_x.to_le_bytes());
        data[13..17].clone_from_slice(&p.map_y.to_le_bytes());
        data[17..21].clone_from_slice(&p.x.to_le_bytes());
        data[21..25].clone_from_slice(&p.y.to_le_bytes());

        //println!("updating players{:?}", data);
        server.broadcast(&data);
    }
}

#[allow(non_snake_case)]
fn InvList_to_bytes(data: &mut Vec<u8>, list: *mut core::InvList) {
    unsafe{
        let mut list = *list;
        let object_num = list.size();
        data.extend_from_slice(&object_num.to_le_bytes());
        let mut cur = list.head;
        while cur != std::ptr::null_mut() {
            let el = (*cur).el as *mut ::core::ffi::c_void;
            let obj_ptr = core::InventoryElement_to_bytes(el);
            
            let o = std::slice::from_raw_parts_mut(obj_ptr, core::InventoryElement_get_packet_size(el) as usize);
            data.extend_from_slice(o);
            cur = (*cur).next;
            core::free(obj_ptr as *mut ::core::ffi::c_void);
        }
    }
}

fn update_player_inventory(server: &Server, peer: &SocketAddr, players: &mut Vec<core::Player>) {
    let id = server.clients.get(peer).unwrap();

    let mut data = vec![common::PACKET_INVENTORY_UPDATE as u8];
    unsafe{
        let inv = &mut *players[*id].inventory;
        InvList_to_bytes(&mut data, inv);
    }

    server.socket.send_to(&data, peer).unwrap();
}

fn update_chunk_for_player(server: &Server, peer: &SocketAddr, coords: (u8, u8)) {
    let mut data = vec![ 0 as u8 ; 3 + (core::CHUNK_SIZE * core::CHUNK_SIZE) as usize * size_of::<core::game_tiles>()];
    data[0] = common::PACKET_CHUNK_UPDATE;
    data[1] = coords.0;
    data[2] = coords.1;

    unsafe {
        let mut chunk = *core::world_table[coords.1 as usize][coords.0 as usize];
        let tile = core::tile{tile: 1};
        chunk.table[5][5] = tile;
        let table = chunk.table.as_mut_ptr() as *mut u8;
        let table = std::slice::from_raw_parts(table, 1024);

        let dest = data[3..].as_mut_ptr();
        let dest = std::slice::from_raw_parts_mut(dest, 1024);
        dest.clone_from_slice(table);

    }
        
    server.socket.send_to(&data, peer).unwrap();
    create_objects_in_chunk_for_player(server, peer, coords);
}

fn create_objects_in_chunk_for_player(server: &Server, peer: &SocketAddr, coords: (u8, u8)) {
    let mut data = vec![common::PACKET_CREATE_OBJECTS_IN_CHUNK, coords.0, coords.1];

    let mut chunk;
    unsafe {
        chunk = *core::world_table[coords.1 as usize][coords.0 as usize];
        if chunk.objects.size() <= 64 {
            InvList_to_bytes(&mut data, &mut chunk.objects);
        } else {
            println!("TODO to much items in chunk");
        }
    }
    server.socket.send_to(&data, peer).unwrap();
    
}

fn handle_network(server: &mut Server, players: &mut Vec<core::Player>) {
    let mut buf = [0; 100];
    loop {
        if let Ok((amt, src)) = server.socket.recv_from(&mut buf) {
            if amt == 0 {
                break;
            }
            match buf[0] {
                common::PACKET_JOIN_REQUEST => {
                    println!("connected");
                    add_player(server, src, players);
                }
                _ => {
                    match server.clients.get(&src) {
                        Some(id) =>  {
                            if *id < players.len() {
                                handle_packet(server, &mut players[*id], &buf, *id);
                            } else {
                                println!("invalid player idx {}", *id);
                            }
                        }
                        None => println!("player without idx"),
                    }
                }
            }
        } else {
            break;
        }
    }
}

fn handle_packet(server: &Server, player: &mut core::Player, packet: &[u8], player_id: usize) {
    let tag = ClientEvent::from(packet);
    match tag {
        ClientEvent::Move { x, y } => {
            println!("moved {x} {y}");
            unsafe {player.move_(x, y)}
        },
        ClientEvent::Pickup { id } => {
            println!("player picked {id}");
            unsafe {
                let item = (*core::world_table[player.map_y as usize][player.map_x as usize]).find_by_id(id);
                player.pickup(item);
                //let mut buf = vec![common::PACKET_PLAYER_ACTION_PICKUP];
                //buf.extend_from_slice(&id.to_le_bytes());
                //buf.extend_from_slice(&player_id.to_le_bytes());
                //server.broadcast(&buf);
            }
        },
        ClientEvent::Drop { id } => {
            println!("player dropped {id}");
            unsafe {
                let item = player.get_item_by_uid(id);
                if item != std::ptr::null_mut() {
                    (*core::world_table[player.map_y as usize][player.map_x as usize]).move_object(item, player.x, player.y);
                    (*player.inventory).remove(item);
                    //let mut buf = vec![common::PACKET_PLAYER_ACTION_DROP];
                    //buf.extend_from_slice(&id.to_le_bytes());
                    //buf.extend_from_slice(&player_id.to_le_bytes());
                    //server.broadcast(&buf);
                }
            }
        },
        ClientEvent::ItemUsedOnObject { iid, oid } => {
            println!("player used {iid} on {oid}");
            unsafe {
                let item = player.get_item_by_uid(iid);
                let object = (*core::world_table[player.map_y as usize][player.map_x as usize]).find_by_id(oid);
                player.use_item_on_object(item, object);
            }
        },
        ClientEvent::Craft { product_id, ingredients_num, ingredients_ids } => {
            unsafe { core::craft(product_id as i32, ingredients_num as i32, ingredients_ids.as_ptr() as *const usize, player) };
        }
        ClientEvent::Whatever => println!("whatever"),
    }
}

fn send_game_updates(server: &Server, players: &mut Vec<core::Player>) {

    update_players(server, players);
    send_location_updates(server);
    unsafe {
        let el = std::ptr::addr_of_mut!(core::objects_to_update);
        let mut data = vec![common::PACKET_OBJECTS_UPDATE];
        InvList_to_bytes(&mut data, el);
        server.broadcast(&data);

        while (*el).head != std::ptr::null_mut() {
            (*el).remove((*(*el).head).el);
        }

        let el = std::ptr::addr_of_mut!(core::objects_to_update_reliable);
        let mut data = vec![common::PACKET_OBJECTS_UPDATE];
        InvList_to_bytes(&mut data, el);
        server.broadcast(&data);

        while (*el).head != std::ptr::null_mut() {
            (*el).remove((*(*el).head).el);
        }
    }
}

fn send_location_updates(server: &Server) {
    unsafe {
        if LOCATION_UPDATES.len() > 0 {
            let mut data = vec![common::PACKET_LOCATION_UPDATES];
            for update in LOCATION_UPDATES.iter() {
                data.extend_from_slice(&update.to_le_bytes());
            }
            println!("{:?}", data);
            server.broadcast(&data);
            LOCATION_UPDATES.clear();
        }
    }
}

#[no_mangle]
extern "C" fn destroy_object(server: &Server, id: usize, location: core::ItemLocation) {
    let mut buf = vec![common::PACKET_DESTROY_OBJECT];
    buf.extend_from_slice(&id.to_le_bytes());

    let data = &location as *const core::ItemLocation;
    let data = data as *const u8;
    unsafe {
        let slice = std::slice::from_raw_parts(data, size_of::<LocationUpdate>());
        buf.extend_from_slice(slice);
    }
    server.broadcast(&buf);
}

