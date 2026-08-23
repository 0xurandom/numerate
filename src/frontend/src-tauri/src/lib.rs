use std::io::{BufRead, BufReader, Write};
use std::process::{ChildStdin, ChildStdout, Command, Stdio};
use std::sync::Mutex;

pub struct CalcProcess {
    stdin: ChildStdin,
    stdout: BufReader<ChildStdout>,
}

impl CalcProcess {
    fn spawn_new() -> Self {
        let mut child = Command::new("../../backend/main.out")
            .stdin(Stdio::piped())
            .stdout(Stdio::piped())
            .spawn()
            .expect("Failed to run c binary");

        let stdin = child.stdin.take().unwrap();
        let stdout = BufReader::new(child.stdout.take().unwrap());

        Self {stdin, stdout}
    }

    fn evaluate(&mut self, input: &str) -> Result<String, String> {

        let mut results = Vec::new();

        for line in input.lines() {
            if line.trim().is_empty() {
                results.push(String::new());
                continue;
            }

        self.stdin.write_all(line.as_bytes()).map_err(|e| format!("write failed: {e}"))?;
        self.stdin.write_all(b"\n").map_err(|e| format!("newline write failed: {e}"))?;
        self.stdin.flush().map_err(|e| format!("flush failed: {e}"))?;

        let mut output = String::new();
        let bytes = self.stdout
            .read_line(&mut output)
            .map_err(|e| format!("read failed: {e}"))?;

        if bytes == 0 {
            return Err("calculator process exited".to_string());
        }

        println!("INPUT: {:?}", line);
        println!("OUTPUT: {:?}", output);

        results.push(output.trim().to_string());
        }
        Ok(results.join("\n"))
    }
}

pub struct CalcState(Mutex<CalcProcess>);
unsafe impl Send for CalcState {}
unsafe impl Sync for CalcState {}


#[tauri::command]
fn evaluate(state: tauri::State<CalcState>, input: String) -> Result<String, String> {
    let mut calc = state.0.lock().map_err(|e| e.to_string())?;
    calc.evaluate(&input)
}


#[tauri::command]
fn reset_calculator(state: tauri::State<CalcState>) -> Result<(), String>{
    let mut calc = state.0.lock().map_err(|e| e.to_string())?;
    let _ = calc.evaluate("RESET_CALC");
    Ok(())
}

#[cfg_attr(mobile, tauri::mobile_entry_point)]
pub fn run() {

    let calc_state = CalcState(Mutex::new(CalcProcess::spawn_new()));

    tauri::Builder::default()
        .manage(calc_state)
        .plugin(tauri_plugin_fs::init())
        .plugin(tauri_plugin_opener::init())
        .invoke_handler(tauri::generate_handler![evaluate, reset_calculator])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
