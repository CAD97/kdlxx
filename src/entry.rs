use kdl::*;
use std::mem;

#[no_mangle]
pub extern "C" fn KDL_Entry_name(entry: &KdlEntry) -> Option<&KdlIdentifier> {
    entry.name()
}

#[no_mangle]
pub extern "C" fn KDL_Entry_value(entry: &KdlEntry) -> &KdlValue {
    entry.value()
}

#[no_mangle]
pub extern "C" fn KDL_Entry_ty(entry: &KdlEntry) -> Option<&KdlIdentifier> {
    entry.ty()
}

#[no_mangle]
pub static KDL_Entry_sizeof: usize = mem::size_of::<KdlEntry>();
