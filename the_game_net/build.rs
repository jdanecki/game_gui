use cmake::Config;
use std::env;
use std::path::PathBuf;

fn main() {
    println!("cargo:rerun-if-changed=../core");
    let server_cpp = Config::new("../core").build_target("pime").build();

    println!(
        "cargo:rustc-link-search=native={}/build/",
        server_cpp.display()
    );
    println!("cargo:rustc-link-lib=dylib=pime");

    let bindings = bindgen::Builder::default()
        .header("core_headers.h")
        .clang_arg("-xc++")
        .clang_arg("-std=c++14")
        .parse_callbacks(Box::new(bindgen::CargoCallbacks::new()))
        .generate()
        .expect("Unable to generate bindings");

    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap()).join("core_bindings.rs");
    bindings
        .write_to_file(out_path)
        .expect("Couldn't write bindings!");
}
