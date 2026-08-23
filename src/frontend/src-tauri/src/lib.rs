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

    fn evaluate(&mut self, input: &str) -> String {

        if input.trim().is_empty() {
            return String::new();
        }


        let mut stripped_input = input.replace('\n', " ").replace('\r', "");
        stripped_input.push('\n');

        self.stdin.write_all(stripped_input.as_bytes()).unwrap();
        self.stdin.flush().unwrap();

        let mut output = String::new();
        self.stdout.read_line(&mut output).unwrap();

        output.trim().to_string()
    }
}

pub struct CalcState(Mutex<CalcProcess>);
unsafe impl Send for CalcState {}
unsafe impl Sync for CalcState {}


#[tauri::command]
fn evaluate(state: tauri::State<CalcState>, input: String) -> String {
    let mut calc = state.0.lock().unwrap();
    calc.evaluate(&input)
}


#[tauri::command]
fn reset_calculator(state: tauri::State<CalcState>) {
    let mut calc = state.0.lock().unwrap();
    calc.evaluate("RESET_CALC");
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
