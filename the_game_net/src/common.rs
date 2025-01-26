pub const PACKET_JOIN_REQUEST: PacketType = 0;
pub const PACKET_PLAYER_UPDATE: PacketType = 1;
pub const PACKET_PLAYER_ID: PacketType = 2;
pub const PACKET_PLAYER_MOVE: PacketType = 3;
pub const PACKET_CHUNK_UPDATE: PacketType = 4;
pub const PACKET_INVENTORY_UPDATE: PacketType = 5;
pub const PACKET_OBJECT_UPDATE: PacketType = 6;
pub const PACKET_PLAYER_ACTION_PICKUP: PacketType = 7;
pub const PACKET_PLAYER_ACTION_DROP: PacketType = 8;
pub const PACKET_PLAYER_ACTION_USE_ITEM_ON_OBJECT: PacketType = 9;
pub const PACKET_PLAYER_ACTION_CRAFT: PacketType = 10;
pub const PACKET_LOCATION_UPDATES: PacketType = 11;
pub const PACKET_OBJECT_CREATE: PacketType = 12;
pub const PACKET_DESTROY_OBJECT: PacketType = 13;
pub const PACKET_FAILED_CRAFT: PacketType = 14;
pub const PACKET_PLAYER_ACTION_USE_ITEM_ON_TILE: PacketType = 15;

pub type PacketType = u8;
