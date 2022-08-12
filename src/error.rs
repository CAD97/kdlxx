use kdl::*;

#[no_mangle]
pub unsafe extern "C" fn KDL_Error_free(_error: Box<KdlError>) {}

#[no_mangle]
pub extern "C" fn KDL_Error_input(error: &KdlError, s: &mut *const u8) -> usize {
    *s = error.input.as_ptr();
    error.input.len()
}

#[no_mangle]
pub extern "C" fn KDL_Error_span(error: &KdlError, offset: &mut usize, length: &mut usize) {
    *offset = error.span.offset();
    *length = error.span.len();
}

#[no_mangle]
pub extern "C" fn KDL_Error_label(error: &KdlError, s: &mut Option<*const u8>) -> usize {
    *s = error.label.filter(|it| !it.is_empty()).map(str::as_ptr);
    error.label.map_or(0, str::len)
}

#[no_mangle]
pub extern "C" fn KDL_Error_help(error: &KdlError, s: &mut Option<*const u8>) -> usize {
    *s = error.help.filter(|it| !it.is_empty()).map(str::as_ptr);
    error.help.map_or(0, str::len)
}
