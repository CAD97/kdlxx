use kdl::*;
use std::{ptr, slice, str};

#[no_mangle]
pub extern "C" fn KDL_Document_free(_doc: Box<KdlDocument>) {}

#[no_mangle]
pub unsafe extern "C" fn KDL_Document_parse(
    s: *const u8,
    len: usize,
    docptr: &mut *mut KdlDocument,
    errptr: &mut *mut KdlError,
) -> bool {
    let s = str::from_utf8_unchecked(slice::from_raw_parts(s, len));
    match s.parse() {
        Ok(doc) => {
            *docptr = Box::into_raw(Box::new(doc));
            *errptr = ptr::null_mut();
            true
        }
        Err(err) => {
            println!("{:#?}", err);
            *docptr = ptr::null_mut();
            *errptr = Box::into_raw(Box::new(err));
            println!("{:p}", *errptr);
            false
        }
    }
}

#[no_mangle]
pub unsafe extern "C" fn KDL_Document_get(
    doc: &KdlDocument,
    s: *const u8,
    len: usize,
) -> Option<&KdlNode> {
    let name = str::from_utf8_unchecked(slice::from_raw_parts(s, len));
    doc.get(name)
}

#[no_mangle]
pub unsafe extern "C" fn KDL_Document_get_arg(
    doc: &KdlDocument,
    s: *const u8,
    len: usize,
) -> Option<&KdlValue> {
    let name = str::from_utf8_unchecked(slice::from_raw_parts(s, len));
    doc.get_arg(name)
}

#[no_mangle]
pub extern "C" fn KDL_Document_nodes(doc: &KdlDocument, len: &mut usize) -> *const KdlNode {
    let nodes = doc.nodes();
    *len = nodes.len();
    nodes.as_ptr()
}
