use kdl::*;

#[no_mangle]
pub extern "C" fn KDL_Identifier_value(identifier: &KdlIdentifier, len: &mut usize) -> *const u8 {
    let value = identifier.value();
    *len = value.len();
    value.as_ptr()
}
