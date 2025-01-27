use crate::core;
use crate::types;

extern "C" {
    pub fn update_player(id: usize, map_x: i32, map_y: i32, x: i32, y: i32);
    pub fn update_chunk(x: i32, y: i32, data: &core::chunk_table);
    pub fn got_id(id: usize, seed: i64);
    pub fn update_object(data: types::ObjectData);
    pub fn update_item_location(data: types::LocationUpdateData);
    pub fn create_object(data: types::ObjectData);
    pub fn destroy_object(id: usize, location: types::ItemLocationLol);
    pub fn failed_craft();
}
