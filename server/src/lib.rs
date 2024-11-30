use std::error::Error;
use std::net::UdpSocket;
use std::net::SocketAddr;
use std::collections::HashMap;

mod core;
mod common;

pub static mut SEED: i64 = 0;

pub enum ClientEvent {
    Move{x: i32, y: i32},
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

impl From<&[u8]> for ClientEvent {
    fn from(value: &[u8]) -> Self {
        match value[0] {
            common::PACKET_PLAYER_MOVE => ClientEvent::Move{x: (value[1] as i8).into(), y: (value[2] as i8).into()},
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
        let p = core::Player::new();

        let el = &mut core::Element::new(core::base_elements[0]) as *mut core::Element;
        let el = el as *mut core::InventoryElement;
        println!("item {} {}", (*el).c_id, (*el).uid);
        (*p.inventory).add(el);
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

        println!("updating players{:?}", data);
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

        InvList_to_bytes(&mut data, &mut chunk.objects);
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
                                handle_packet(&mut players[*id], &buf);
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

fn handle_packet(player: &mut core::Player, packet: &[u8]) {
    let tag = ClientEvent::from(packet);
    match tag {
        ClientEvent::Move { x, y } => {
            println!("moved {x} {y}");
            unsafe {player.move_(x, y)}
        },
        ClientEvent::Whatever => println!("whatever"),
    }
}

fn send_game_updates(server: &Server, players: &mut Vec<core::Player>) {

    update_players(server, players);
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
