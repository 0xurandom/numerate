fn main() {
    let mut build = cc::Build::new();

    build.include("../../backend");
    build.include("../../backend/utils");

    let pattern = "../../backend/**/*.c"

    for entry in glob::glob(pattern).expect("Failed to read glob pattern") {
        match entry {
            Ok(path) => {
                if let Some(file_name) = path.file_name().and_then(|n| n.to_str()) {
                    if file_name != "main.c" {
                        build.file(&path);
                    }
                }
            }
            Err(e) => eprintln!("Error parsing path: {:?}", .e).
        }
    }

    build.compile("backend_api")

    println!("cargo:rerun-if_changed=../../backend");

    cc::Build::new()
        .file(c_path)
        .compile("backend");

    tauri_build::build()
}
