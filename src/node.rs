use kdl::*;
use std::{mem, slice, str};

#[no_mangle]
pub extern "C" fn KDL_Node_name(node: &KdlNode) -> &KdlIdentifier {
    node.name()
}

#[no_mangle]
pub extern "C" fn KDL_Node_ty(node: &KdlNode) -> Option<&KdlIdentifier> {
    node.ty()
}

#[no_mangle]
pub extern "C" fn KDL_Node_entries(node: &KdlNode, len: &mut usize) -> *const KdlEntry {
    let entries = node.entries();
    *len = entries.len();
    entries.as_ptr()
}

#[no_mangle]
pub unsafe extern "C" fn KDL_Node_get_prop(
    node: &KdlNode,
    s: *const u8,
    len: usize,
) -> Option<&KdlEntry> {
    let name = str::from_utf8_unchecked(slice::from_raw_parts(s, len));
    node.get(name)
}

#[no_mangle]
pub extern "C" fn KDL_Node_get_arg(node: &KdlNode, ix: usize) -> Option<&KdlEntry> {
    node.get(ix)
}

#[no_mangle]
pub extern "C" fn KDL_Node_children(node: &KdlNode) -> Option<&KdlDocument> {
    node.children()
}

#[no_mangle]
pub static KDL_Node_sizeof: usize = mem::size_of::<KdlNode>();
