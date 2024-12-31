extern "C" {
    pub fn update_player(id: usize, map_x: i32, map_y: i32, x: i32, y: i32);
    pub fn update_chunk(x: i32, y: i32, data: *mut u8);
    pub fn got_id(id: usize, seed: i64);
    pub fn update_inventory(data: *mut u8);
    pub fn update_objects(data: *mut u8);
    pub fn item_picked_up(iid: usize, pid: usize);
}
