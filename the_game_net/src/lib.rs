use std::error::Error;
use std::net::UdpSocket;

mod events;

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
    socket.set_nonblocking(true)?;
    
    Ok(NetClient {
        socket,
    })
}

enum ServerEvent {
    UpdatePlayer{id: usize, map_x: i32, map_y: i32, x: i32, y: i32},
    ChunkUpdate{x: i32, y: i32, data: [u8; 16*16*4]},
}

impl TryFrom<&[u8]> for ServerEvent {
    type Error = ();

    fn try_from(value: &[u8]) -> Result<Self, Self::Error> {

    }
}

#[no_mangle]
pub extern "C" fn foo(a: &NetClient) {
    println!("{:?}", a.socket);
    a.socket.send(&[1, 2, 3]).expect("foo");
}

#[no_mangle]
pub extern "C" fn network_tick(client: &NetClient) {
    let socket = &client.socket;
    
    let mut buf = [0;50];
    if let Ok((amt, src)) = socket.recv_from(&mut buf) {
        let value = &buf;
        match value[0] {
            0 => {
                unsafe{
                    events::update_player(
                        usize::from_le_bytes(value[1..9].try_into().unwrap()), 
                        i32::from_le_bytes(value[9..13].try_into().unwrap()),
                        i32::from_le_bytes(value[13..17].try_into().unwrap()),
                        i32::from_le_bytes(value[17..21].try_into().unwrap()),
                        i32::from_le_bytes(value[21..25].try_into().unwrap()),
                    );
                }
            },
            1 => {
                unsafe {
                    events::chunk_update(
                        i32::from_le_bytes(value[1..5].try_into().unwrap()),
                        i32::from_le_bytes(value[5..9].try_into().unwrap()),
                        &value[9..]
                    )
                }
            }
            _ => ()
        }
    }
    
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
