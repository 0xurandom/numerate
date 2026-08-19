fn main() {
    let c_path = "../../backend/main.c";
    println!("cargo:rerun-if_changed={}", c_path);

    cc::Build::new()
        .file(c_path)
        .compile("backend");

    tauri_build::build()
}
