mod core;
use server::*;

fn main() {
    let server = init_server();
    if let Ok(mut server) = server {
        unsafe {
            SEED = core::time(std::ptr::null_mut());
            println!("{}", SEED);
            core::srand(SEED as u32);
            core::init_elements();
            core::generator();
        }

        main_loop(&mut server);
    }

    println!("Hello, world!");
}
