use std::error::Error;
use std::net::UdpSocket;

mod events;
mod common;

#[repr(C)]
pub struct NetClient {
    socket: UdpSocket,
}

#[no_mangle]
pub extern "C" fn init() -> *const NetClient {
    Box::into_raw(Box::new(init_internal().expect("failed to init NetClient")))
}

fn init_internal() -> Result<NetClient, Box<dyn Error>> {
    let socket = UdpSocket::bind("127.0.0.1:0")?;
    socket.connect("127.0.0.1:1234")?;
    let mut buf = [0];
    buf[0] = common::PACKET_JOIN_REQUEST;
    socket.send(&buf)?;
    let mut buf = [0; 17];
    socket.recv(&mut buf)?;
    if buf[0] == common::PACKET_PLAYER_ID {
        unsafe {
            events::got_id(usize::from_le_bytes(buf[1..9].try_into().unwrap()), i64::from_le_bytes(buf[9..17].try_into().unwrap()));
        };
    } else {
        println!("did not get id");
        panic!();
    }

    socket.set_nonblocking(true)?;
    
    Ok(NetClient {
        socket,
    })
}

#[no_mangle]
pub extern "C" fn foo(a: &NetClient) {
    println!("{:?}", a.socket);
    a.socket.send(&[1, 2, 3]).expect("foo");
}

#[no_mangle]
pub extern "C" fn network_tick(client: &NetClient) {
    let socket = &client.socket;
    
    let mut buf = [0;2048];
    loop {
    if let Ok((amt, _src)) = socket.recv_from(&mut buf) {
        //println!("{:?}", &buf);
        let value = &mut buf;
        //println!("{:?}", &value);
        match value[0] {
            common::PACKET_PLAYER_UPDATE => {
                println!("player update {}", amt);
                if amt == 25 {
                    unsafe{
                        events::update_player(
                            usize::from_le_bytes(value[1..9].try_into().unwrap()), 
                            i32::from_le_bytes(value[9..13].try_into().unwrap()),
                            i32::from_le_bytes(value[13..17].try_into().unwrap()),
                            i32::from_le_bytes(value[17..21].try_into().unwrap()),
                            i32::from_le_bytes(value[21..25].try_into().unwrap()),
                        );
                    }
                } else {
                    println!("invalid player update");
                }
            },
            common::PACKET_CHUNK_UPDATE => {
                println!("chunk update {}", amt);
                unsafe {
                    events::update_chunk(
                        //i32::from_le_bytes(value[1..5].try_into().unwrap()),
                        //i32::from_le_bytes(value[5..9].try_into().unwrap()),
                        i32::from(value[1]),
                        i32::from(value[2]),
                        &mut value[3] as *mut u8
                    )
                }
            },
            common::PACKET_INVENTORY_UPDATE => {
                unsafe {
                    events::update_inventory(value as *mut u8);
                }
            },
            common::PACKET_OBJECTS_UPDATE => {
                unsafe {
                    events::update_objects(value as *mut u8);
                }
            }
            _ => {
                println!("invalid packet type {:?}", value);
            }
        }
    } else {
        break;
    }
    }
}

#[no_mangle]
pub extern "C" fn send_packet_move(client: &NetClient, x: i32, y: i32) {
    let buf = [common::PACKET_PLAYER_MOVE, x as u8, y as u8];
    client.socket.send(&buf).unwrap();
}

pub fn add(left: u64, right: u64) -> u64 {
    left + right
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        let result = add(2, 2);
        assert_eq!(result, 4);
    }
}
