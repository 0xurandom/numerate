use std::io::{BufRead, BufReader, Write};
use std::process::{Command, Stdio};

fn evaluate_process(input: &str) -> Result<String, String> {
    let mut child = Command::new("../../backend/main.out")
        .stdin(Stdio::piped())
        .stdout(Stdio::piped())
        .spawn()
        .map_err(|e| format!("Failed to run calculator: {e}"))?;

    let stdin = child
        .stdin
        .as_mut()
        .ok_or("Failed to open calculator stdin")?;

    stdin
        .write_all(input.as_bytes())
        .map_err(|e| format!("write failed: {e}"))?;

    if !input.ends_with('\n') {
        stdin
            .write_all(b"\n")
            .map_err(|e| format!("newline write failed: {e}"))?;
    }

    stdin
        .flush()
        .map_err(|e| format!("flush failed: {e}"))?;


    drop(child.stdin.take());

    let stdout = child
        .stdout
        .take()
        .ok_or("failed to open calc stdout")?;


    let mut reader = BufReader::new(stdout);
    let mut results = Vec::new();
    let mut output = String::new();

    while reader
        .read_line(&mut output)
        .map_err(|e| format!("read failed: {e}"))?
        > 0
    {
        results.push(output.trim_end().to_string());
        output.clear();
    }

    let status = child
        .wait()
        .map_err(|e| format!("Failed waiting for calculator: {e}"))?;

    if !status.success() {
        return Err(format!("Calculator exited with {status}"));
    }

    Ok(results.join("\n"))
}


#[tauri::command]
fn evaluate(input: String) -> Result<String, String> {
    evaluate_process(&input)
}


#[cfg_attr(mobile, tauri::mobile_entry_point)]
pub fn run() {
    tauri::Builder::default()
        .plugin(tauri_plugin_fs::init())
        .plugin(tauri_plugin_opener::init())
        .invoke_handler(tauri::generate_handler![evaluate])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
