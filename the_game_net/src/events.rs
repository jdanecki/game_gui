extern "C" {
    pub fn update_player(id: usize, map_x: i32, map_y: i32, x: i32, y: i32);
    pub fn chunk_update(x: i32, y: i32, data: &[u8]);
}
