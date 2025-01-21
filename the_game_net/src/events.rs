use crate::core;
use crate::types;

extern "C" {
    pub fn update_player(id: usize, map_x: i32, map_y: i32, x: i32, y: i32);
    pub fn update_chunk(x: i32, y: i32, data: &core::chunk_table);
    pub fn got_id(id: usize, seed: i64);
    pub fn update_inventory(data: *mut u8);
    pub fn update_objects(data: *mut u8);
    pub fn item_picked_up(iid: usize, pid: usize);
    pub fn item_dropped(iid: usize, pid: usize);
    pub fn item_used_on_object(iid: usize, oid: usize, pid: usize);
    pub fn update_item_location(updates_number: i32, data: *mut u8);
    pub fn create_object_in_chunk(x: i32, y: i32, data: types::ObjectData);
    pub fn destroy_object(id: usize, data: *mut u8);
    pub fn failed_craft();
}
