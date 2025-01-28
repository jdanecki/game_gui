# THE GAME PROTOCOL LOL

## Packet types

    enum PacketType {
        PACKET_PLAYER_UPDATE = 0,
        PACKET_PLAYER_ID,
        PACKET_MOVE,
        PACKET_CHUNK_UPDATE,
        PACKET_INIT_INVENTORY_ELEMENT,
    };

### player update

packet size: 25

| type  | bits  | value |
| u8    | 0     | PACKET\_PLAYER\_UPDATE |
| usize | 1-8   | player id |
| i32   | 9-12  | player map\_x |
| i32   | 13-16 | player map\_y |
| i32   | 17-20 | player x |
| i32   | 21-24 | player y |

### player id

packet size: 9

| type  | bits  | value |
| u8    | 0     | PACKET\_PLAYER\_ID |
| usize | 1-8   | id for player |

### move player

packet size: 3

| type  | bits  | value |
| u8    | 0     | PACKET\_MOVE |
| i8    | 0     | x     |
| i8    | 0     | y     |

### chunk update

packet size: 3 + CHUNK\_SIZE * CHUNK\_SIZE * sizeof(enum game\_tiles)

| type  | bits  | value |
| u8    | 0     | PACKET\_CHUNK\_UPDATE |
| u8    | 1     | chunk x |
| u8    | 2     | chunk y |
| enum game\_tiles | 3-size | data |
| objects | ? | |

### object serialization

packet size: 

| type  | bits  | value |
| u8    | 0     | PACKET\_INIT\_INVENTORY\_ELEMENT |



