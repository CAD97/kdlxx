use kdl::*;
use std::ptr;

#[no_mangle]
pub unsafe extern "C" fn KDL_Error_free(_error: Box<KdlError>) {}

#[no_mangle]
pub extern "C" fn KDL_Error_input(error: &KdlError, len: &mut usize) -> *const u8 {
    *len = error.input.len();
    error.input.as_ptr()
}

#[no_mangle]
pub extern "C" fn KDL_Error_span(error: &KdlError, len: &mut usize) -> *const u8 {
    *len = error.span.len();
    unsafe { error.input.as_ptr().add(error.span.offset()) }
}

#[no_mangle]
pub extern "C" fn KDL_Error_label(error: &KdlError, len: &mut usize) -> *const u8 {
    *len = error.label.map_or(0, str::len);
    error
        .label
        .filter(|it| !it.is_empty())
        .map_or(ptr::null(), str::as_ptr)
}

#[no_mangle]
pub extern "C" fn KDL_Error_help(error: &KdlError, len: &mut usize) -> *const u8 {
    *len = error.help.map_or(0, str::len);
    error
        .help
        .filter(|it| !it.is_empty())
        .map_or(ptr::null(), str::as_ptr)
}
