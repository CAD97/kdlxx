use kdl::*;

#[no_mangle]
pub extern "C" fn KDL_Identifier_value(identifier: &KdlIdentifier, s: &mut *const u8) -> usize {
    *s = identifier.value().as_ptr();
    identifier.value().len()
}
