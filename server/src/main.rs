mod core;
use enet::*;
use std::net::Ipv4Addr;

fn main() {
    let host = init_enet();
    
    unsafe {
        core::init_elements();
        core::generator();
    }

    main_loop(host);

    println!("Hello, world!");
}

fn init_enet() -> Host<usize> {
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

fn main_loop(mut host: Host<usize>) {
    let mut players: Vec<core::Player> = vec![];
    let mut i = 0;
    loop {
        match host.service(1000).expect("foo") {
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
                add_player(p.clone(), &mut players);
            }
            Some(Event::Disconnect(..)) => println!("disconnected"),
            Some(Event::Receive { ref mut sender, channel_id, ref packet }) => {
                println!(
                    "geto packet from {:?} on channel {}, content {}",
                    sender.data(), channel_id, std::str::from_utf8(packet.data()).unwrap()
                );
                let _ = sender.send_packet(Packet::new(b"from server", PacketMode::ReliableSequenced).unwrap(), 1);
            }
            _ => (),
        }
//  let _ = sender.send_packet(Packet::new(b"from server", PacketMode::ReliableSequenced).unwrap(), 1);
        
        update_players(&mut host, &mut players);
        let s = format!("packet nr {}", i);
        host.broadcast(Packet::new(s.as_bytes(), PacketMode::ReliableSequenced).unwrap(), 1);
        i += 1;
    }
}

fn add_player(mut peer: Peer<usize>, players: &mut Vec<core::Player>) {
    peer.set_data(Some(players.len()));
    unsafe {
        let p = core::Player::new();
        players.push(p);
    }
    println!("{:?} , players {:?}", peer, players);
}


fn update_players(host: &mut Host<usize>, players: &mut Vec<core::Player>) {
    for (i, p) in players.iter().enumerate() {
        let data = format!("player {} {} {} {} {}", i, p.map_x, p.map_y, p.x, p.y);
        //println!("{}", data);
        host.broadcast(Packet::new(data.as_bytes(), PacketMode::UnreliableUnsequenced).unwrap(), 1);
    }
}
