fn main() {
    let mut build = cc::Build::new();

    build.include("../../backend");
    build.include("../../backend/utils");

    let pattern = "../../backend/**/*.c";

    match glob::glob(pattern) {
        Ok(paths) => {
            for entry in paths {
                match entry {
                    Ok(path) =>{
                        let is_unused = path.components().any(|comp| comp.as_os_str() == "unused");
                        if !is_unused {
                            if let Some(file_name) = path.file_name().and_then(|n| n.to_str()) {
                                if file_name != "main.c" {
                                    build.file(&path);
                                }
                            }
                        }
                    }
                    Err(e) => eprintln!("error parsing path: {:?}", e),
                }
            }
        }
        Err(e) => eprintln!("failed to read glob pattern: {:?}", e),
    }



    build.compile("backend_api");

    println!("cargo::rustc-link-lib=gmp");
    println!("cargo:rustc-link-lib=m");
    println!("cargo:rustc-link-lib=mpfr");
    println!("cargo:rustc-link-lib=mpc");



    println!("cargo:rerun-if_changed=../../backend");

    tauri_build::build()
}
