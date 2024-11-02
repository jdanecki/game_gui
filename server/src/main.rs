mod core;
use server::*;

fn main() {
    let host = init_enet();
    
    unsafe {
        SEED = core::time(std::ptr::null_mut());
        println!("{SEED}");
        core::srand(SEED as u32);
        core::init_elements();
        core::generator();
    }

    main_loop(host);

    println!("Hello, world!");
}


