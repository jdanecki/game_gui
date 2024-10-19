fn main() {
    println!(r"cargo:rustc-link-search=../core/build/");
    println!(r"cargo:rustc-link-lib=pime");
    println!(r"cargo:rustc-link-arg=-Wl,-rpath,../core/build/");

    /*cc::Build::new()
        .file("../c_part/clib.c")
        .compile("foo");*/
}
