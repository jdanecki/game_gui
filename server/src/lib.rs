mod core;
use enet::*;
use std::net::Ipv4Addr;

pub static mut SEED: i64 = 0;

pub enum ClientEvent {
    Move{x: i32, y: i32},
    Whatever,
}

impl From<&[u8]> for ClientEvent {
    fn from(value: &[u8]) -> Self {
        match value[0] as u32{
            core::PacketType_PACKET_MOVE => ClientEvent::Move{x: (value[1] as i8).into(), y: (value[2] as i8).into()},
            1 => ClientEvent::Whatever,
            _ => panic!("invalid event")
        }
    }
}

pub fn init_enet() -> Host<usize> {
    let enet = Enet::new().expect("failed to init enet");
    let address = Address::new(Ipv4Addr::LOCALHOST, 1234);

    enet.create_host::<usize>(
        Some(&address),
        10,
        ChannelLimit::Maximum,
        BandwidthLimit::Unlimited,
        BandwidthLimit::Unlimited,
    ).expect("failed to create host")
}

pub fn main_loop(mut host: Host<usize>) {
    let mut players: Vec<core::Player> = vec![];
    loop {
        handle_network(&mut host, &mut players);
        unsafe {
            core::update();
        }
        send_game_updates(&mut host, &mut players);
        std::thread::sleep(std::time::Duration::from_millis(50));
    }
}

fn add_player(mut peer: Peer<usize>, players: &mut Vec<core::Player>) {
    peer.set_data(Some(players.len()));

    let mut response = [0 as u8; 17];
    response[0] = core::PacketType_PACKET_PLAYER_ID as u8;
    response[1..9].copy_from_slice(&players.len().to_le_bytes());
    unsafe {
        response[9..17].copy_from_slice(&SEED.to_le_bytes());
    }
    let response = Packet::new(&response, PacketMode::ReliableSequenced).unwrap();
    let _ = peer.send_packet(response, 1);

    unsafe {
        let p = core::Player::new();

        let el = &mut core::Element::new(core::base_elements[0]) as *mut core::Element;
        let el = el as *mut core::InventoryElement;
        println!("item {} {}", (*el).c_id, (*el).uid);
        (*p.inventory).add(el);
        players.push(p);
    }
    println!("{:?} , players {:?}", peer, players);
    update_chunk_for_player(&mut peer, (128, 128));
    update_player_inventory(&mut peer, players);
}


fn update_players(host: &mut Host<usize>, players: &mut Vec<core::Player>) {
    for (i, p) in players.iter().enumerate() {
        let mut data = [0 as u8; 25];
        data[0] = core::PacketType_PACKET_PLAYER_UPDATE as u8;
        data[1..9].clone_from_slice(&i.to_le_bytes());
        data[9..13].clone_from_slice(&p.map_x.to_le_bytes());
        data[13..17].clone_from_slice(&p.map_y.to_le_bytes());
        data[17..21].clone_from_slice(&p.x.to_le_bytes());
        data[21..25].clone_from_slice(&p.y.to_le_bytes());

        //println!("{:?}", data);
        host.broadcast(Packet::new(&data, PacketMode::UnreliableUnsequenced).unwrap(), 1);
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

fn update_player_inventory(peer: &mut Peer<usize>, players: &mut Vec<core::Player>) {
    let id = *peer.data().unwrap();

    let mut data = vec![core::PacketType_PACKET_SEND_INVENTORY as u8];
    unsafe{
        let inv = &mut *players[id].inventory;
        InvList_to_bytes(&mut data, inv);
    }

    let _ = peer.send_packet(Packet::new(&data, PacketMode::ReliableSequenced).unwrap(), 1);
}

fn update_chunk_for_player(peer: &mut Peer<usize>, coords: (u8, u8)) {
    let mut data = vec![ 0 as u8 ; 3 + (core::CHUNK_SIZE * core::CHUNK_SIZE) as usize * size_of::<core::game_tiles>()];
    data[0] = core::PacketType_PACKET_CHUNK_UPDATE as u8;
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
        
        let _ = peer.send_packet(Packet::new(&data, PacketMode::ReliableSequenced).unwrap(), 1);
}

fn handle_network(host: &mut Host<usize>, players: &mut Vec<core::Player>) {
    let mut finish = false;
    while !finish{
        match host.service(0).expect("foo") {
            /*Some(e) => {
                match e {
                    Event::Connect(ref p) => {
                        println!("{:?}", p);
                    }
                    Event::Disconnect(_, _) => {},
                    Event::Receive { .. } => {},
                }
            }*/
            Some(Event::Connect(ref p)) => {
                println!("connected");
                add_player(p.clone(), players);
            }
            Some(Event::Disconnect(..)) => println!("disconnected"),
            Some(Event::Receive { ref mut sender, channel_id: _, ref packet }) => {
                match sender.data() {
                    Some(id) =>  {
                        if *id < players.len() {
                            handle_packet(&mut players[*id], packet);
                        } else {
                            println!("invalid player idx {}", *id);
                        }
                    }
                    None => println!("player without idx"),
                }
                /*println!(
                    "geto packet from {:?} on channel {}, content {}",
                    sender.data(), channel_id, std::str::from_utf8(packet.data()).unwrap()
                );*/
                //let _ = sender.send_packet(Packet::new(b"from server", PacketMode::ReliableSequenced).unwrap(), 1);
            }
            _ => {finish = true;}
        }
    }
//  let _ = sender.send_packet(Packet::new(b"from server", PacketMode::ReliableSequenced).unwrap(), 1);
    
    /*let s = format!("packet nr {}", i);
    host.broadcast(Packet::new(s.as_bytes(), PacketMode::ReliableSequenced).unwrap(), 1);
    i += 1;*/
}

fn handle_packet(player: &mut core::Player, packet: &Packet) {
    let tag = ClientEvent::from(packet.data());
    match tag {
        ClientEvent::Move { x, y } => {
            println!("moved {x} {y}");
            unsafe {player.move_(x, y)}
        },
        ClientEvent::Whatever => println!("whatever"),
    }
}

fn send_game_updates(host: &mut Host<usize>, players: &mut Vec<core::Player>) {

    update_players(host, players);
    unsafe {
        let el = std::ptr::addr_of_mut!(core::objects_to_update);
        let mut data = vec![core::PacketType_PACKET_OBJECTS_UPDATE as u8];
        InvList_to_bytes(&mut data, el);
        host.broadcast(Packet::new(&data, PacketMode::UnreliableUnsequenced).unwrap(), 1);

        while (*el).head != std::ptr::null_mut() {
            (*el).remove((*(*el).head).el);
        }

        let el = std::ptr::addr_of_mut!(core::objects_to_update_reliable);
        let mut data = vec![core::PacketType_PACKET_OBJECTS_UPDATE as u8];
        InvList_to_bytes(&mut data, el);
        host.broadcast(Packet::new(&data, PacketMode::ReliableSequenced).unwrap(), 1);

        while (*el).head != std::ptr::null_mut() {
            (*el).remove((*(*el).head).el);
        }
    }
}
