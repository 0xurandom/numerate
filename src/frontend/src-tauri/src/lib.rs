use std::ffi::{CStr, CString};
use std::os::raw::c_char;
use std::sync::Mutex;

#[repr(C)]
struct Calc {_private: [u8; 0]}


extern "C" {
    fn init_calc() -> *mut Calc;
    fn eval_calc(calc: *mut Calc, input: *const c_char) -> *mut c_char;
    fn reset_calc(calc: *mut Calc);
    fn calc_free_result(result: *mut c_char);
}

struct CalcState(Mutex<*mut Calc>);
unsafe impl Send for CalcState {}
unsafe impl Sync for CalcState {}


#[tauri::command]
fn evaluate(state: tauri::State<CalcState>, input: String) -> String {
    let calc = *state.0.lock().unwrap();
    let c_input = CString::new(input).unwrap_or_default();

    unsafe {
        let result_ptr = eval_calc(calc, c_input.as_ptr());
        let result = CStr::from_ptr(result_ptr).to_string_lossy().into_owned();
        calc_free_result(result_ptr);
        result
    }
}

#[tauri::command]
fn reset_calc(state: tauri::State<CalcState>) {
    let calc = *state.0.lock().unwrap();
    unsafe { reset_calc(calc); }
}


#[cfg_attr(mobile, tauri::mobile_entry_point)]
pub fn run() {

    let calc = unsafe {init_calc()};
    let calc_state = CalcState(Mutex::new(calc));

    tauri::Builder::default()
        .manage(calc_state)
        .plugin(tauri_plugin_fs::init())
        .plugin(tauri_plugin_opener::init())
        .invoke_handler(tauri::generate_handler![greet])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
