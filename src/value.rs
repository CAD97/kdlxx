use kdl::*;
use std::ptr;

#[repr(C)]
pub enum KdlValueWhich {
    Null = 0x00,
    RawString = 0x10,
    String,
    Base2 = 0x20,
    Base8,
    Base10,
    Base16,
    Base10Float = 0x40,
    Bool = 0x80,
}

#[no_mangle]
pub extern "C" fn KDL_Value_string(
    value: &KdlValue,
    out_str: &mut *const u8,
    len: &mut usize,
) -> bool {
    match value {
        KdlValue::String(s) | KdlValue::RawString(s) => {
            *out_str = s.as_ptr();
            *len = s.len();
            true
        }
        _ => {
            *out_str = ptr::null();
            *len = 0;
            false
        }
    }
}

#[no_mangle]
pub extern "C" fn KDL_Value_int(value: &KdlValue, out_int: &mut i64) -> bool {
    match value {
        KdlValue::Base2(i) | KdlValue::Base8(i) | KdlValue::Base10(i) | KdlValue::Base16(i) => {
            *out_int = *i;
            true
        }
        _ => {
            *out_int = 0;
            false
        }
    }
}

#[no_mangle]
pub extern "C" fn KDL_Value_float(value: &KdlValue, out_float: &mut f64) -> bool {
    match value {
        KdlValue::Base10Float(f) => {
            *out_float = *f;
            true
        }
        _ => {
            *out_float = 0.0;
            false
        }
    }
}

#[no_mangle]
pub extern "C" fn KDL_Value_bool(value: &KdlValue, out_bool: &mut bool) -> bool {
    match value {
        KdlValue::Bool(b) => {
            *out_bool = *b;
            true
        }
        _ => {
            *out_bool = false;
            false
        }
    }
}

#[no_mangle]
pub extern "C" fn KDL_Value_null(value: &KdlValue) -> bool {
    match value {
        KdlValue::Null => true,
        _ => false,
    }
}

#[no_mangle]
pub extern "C" fn KDL_Value_which(value: &KdlValue) -> KdlValueWhich {
    match value {
        KdlValue::RawString(_) => KdlValueWhich::RawString,
        KdlValue::String(_) => KdlValueWhich::String,
        KdlValue::Base2(_) => KdlValueWhich::Base2,
        KdlValue::Base8(_) => KdlValueWhich::Base8,
        KdlValue::Base10(_) => KdlValueWhich::Base10,
        KdlValue::Base10Float(_) => KdlValueWhich::Base10Float,
        KdlValue::Base16(_) => KdlValueWhich::Base16,
        KdlValue::Bool(_) => KdlValueWhich::Bool,
        KdlValue::Null => KdlValueWhich::Null,
    }
}
