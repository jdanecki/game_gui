use cmake::Config;
use std::env;
use std::path::PathBuf;

fn main() {
    println!("cargo:rerun-if-changed=../core");
    println!("cargo:rerun-if-changed=cpp-src");
    let server_cpp = Config::new("cpp-src").build_target("server_cpp").build();

    println!(
        "cargo:rustc-link-search=native={}/build/",
        server_cpp.display()
    );
    println!(
        "cargo:rustc-link-search=native={}/build/core",
        server_cpp.display()
    );
    println!("cargo:rustc-link-lib=dylib=pime");
    println!("cargo:rustc-link-lib=dylib=server_cpp");

    let bindings = bindgen::Builder::default()
        .header("cpp-src/headers_wrapper.h")
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
