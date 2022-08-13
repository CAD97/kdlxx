#ifndef KDL_H
#define KDL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if (defined(__STDC__) /*&& __STDC_VERSION_STDINT_H__ < 202301L*/) || \
	(defined(__cplusplus) && __cpp_char8_t < 201811L) || \
	(!defined(__STDC__) && !defined(__cplusplus))
typedef unsigned char char8_t;
#endif

#pragma region SAL
#if _WIN32
#	define KDL_FALLIBLE                 _Success_((bool)return)
#	define KDL_THIS_CONST               _In_
#	define KDL_THIS_MUT                 _Inout_
#	define KDL_ARRAY(len)               _Ret_writes_(len)
#	define KDL_ARRAY_B(len, stride)     _Ret_writes_bytes_((len) * (stride))
#	define KDL_NONNULL                  _Ret_
#	define KDL_NULLABLE                 _Ret_maybenull_ KDL_FALLIBLE
#	define KDL_OUTPTR_NONNULL           _Outptr_
#	define KDL_OUTPTR_NULLABLE          _Outptr_result_maybenull_
#	define KDL_OUTPTR_ARRAY(len)        _Outptr_result_buffer_(len)
#	define KDL_INPTR_ARRAY(len)         _In_reads_(len)
#	define KDL_OUT                      _Out_
#	define KDL_MSVC_SAL(...)            __VA_ARGS__
#elif __GNUC__ || __clang__
#	define KDL_FALLIBLE
#	define KDL_THIS_CONST
#	define KDL_THIS_MUT
#	define KDL_ARRAY(len)
#	define KDL_ARRAY_B(len, stride)
#	define KDL_NONNULL
#	define KDL_NULLABLE                 __attribute__((__returns_nonnull__))
#	define KDL_OUTPTR_NONNULL
#	define KDL_OUTPTR_NULLABLE
#	define KDL_OUTPTR_ARRAY(len)
#	define KDL_INPTR_ARRAY(len)
#	define KDL_OUT
#	define KDL_MSVC_SAL(...)
#else
#	define KDL_FALLIBLE
#	define KDL_THIS_CONST
#	define KDL_THIS_MUT
#	define KDL_ARRAY(len)
#	define KDL_ARRAY_B(len, stride)
#	define KDL_NONNULL
#	define KDL_NULLABLE
#	define KDL_OUTPTR_NONNULL
#	define KDL_OUTPTR_NULLABLE
#	define KDL_OUTPTR_ARRAY(len)
#	define KDL_INPTR_ARRAY(len)
#	define KDL_OUT
#	define KDL_MSVC_SAL(...)
#endif
#pragma endregion

/// @brief Represents a KDL Document.
struct KDL_Document;
/// @brief Represents a KDL Argument or KDL Property.
struct KDL_Entry;
/// @brief Represents a KDL Identifier.
struct KDL_Identifier;
/// @brief Represents a KDL Node.
struct KDL_Node;
/// @brief Represents a KDL Value.
struct KDL_Value;

/// @brief An error that occurs when parsing a KDL document.
struct KDL_Error;

enum KdlValueWhich {
	KDL_VALUE_WHICH_NULL = 0x00,
	KDL_VALUE_WHICH_RAW_STRING = 0x10,
	KDL_VALUE_WHICH_STRING,
	KDL_VALUE_WHICH_BASE2 = 0x20,
	KDL_VALUE_WHICH_BASE8,
	KDL_VALUE_WHICH_BASE10,
	KDL_VALUE_WHICH_BASE16,
	KDL_VALUE_WHICH_BASE10_FLOAT = 0x40,
	KDL_VALUE_WHICH_BOOL = 0x80,
};

#define KDL_VALUE_IS_NULL(which)    ((which) == KDL_VALUE_WHICH_NULL)
#define KDL_VALUE_IS_STRING(which)  ((which) & 0x10)
#define KDL_VALUE_IS_INT(which)     ((which) & 0x20)
#define KDL_VALUE_IS_FLOAT(which)   ((which) & 0x40)
#define KDL_VALUE_IS_BOOL(which)    ((which) & 0x80)

#ifdef __cplusplus
extern "C" {
#endif

#pragma region kdl::document

/// @brief Free a KDL Document.
/// @param document The document to free.
void
KDL_Document_free(
	KDL_THIS_MUT struct KDL_Document* document);

/// @brief Gets this node’s name.
/// @param string Pointer to UTF-8 document.
/// @param length Length of UTF-8 document.
/// @param document (out) On successs, owning pointer to parsed document.
/// @param error (out) On failure, owning pointer to parse error.
/// @return Boolean indicating success.
bool
KDL_FALLIBLE
KDL_MSVC_SAL(
	_At_(*document, _Post_notnull_)
	_At_(*error, _Post_null_)
	_On_failure_(
		_At_(*document, _Post_null_)
		_At_(*error, _Post_notnull_)))
KDL_Document_parse(
	KDL_INPTR_ARRAY(length) char8_t const string[],
	size_t length,
	KDL_OUTPTR_NULLABLE struct KDL_Document** document,
	KDL_OUTPTR_NULLABLE struct KDL_Error** error);

/// @brief Gets the first child node with a matching name.
/// @param document The document to work on.
/// @param name Pointer to name.
/// @param length Length of name.
/// @return Pointer to found node, if present.
KDL_NULLABLE
struct KDL_Node const*
KDL_Document_get(
	KDL_THIS_CONST struct KDL_Document const* document,
	KDL_INPTR_ARRAY(length) char8_t const name[],
	size_t length);

/// @brief Gets the first argument (value) of the first child node with a matching name.
/// @param document The document to work on.
/// @param name Pointer to name.
/// @param len Length of name.
/// @return Pointer to found value, if present.
KDL_NULLABLE
struct KDL_Value const*
KDL_Document_get_arg(
	KDL_THIS_CONST struct KDL_Document const* document,
	KDL_INPTR_ARRAY(length) char8_t const name[],
	size_t length);

/// @brief Returns a reference to this document’s child nodes.
/// @param document The document to work on.
/// @param length (out) The number of child nodes.
/// @return Pointer to the first child node (stride by `KDL_Node_sizeof` bytes).
KDL_NULLABLE
KDL_ARRAY_B(*length, KDL_Node_sizeof)
struct KDL_Node const*
KDL_Document_nodes(
	KDL_THIS_CONST struct KDL_Document const* document,
	KDL_OUT size_t* length);

#pragma endregion

#pragma region kdl::entry

/// @brief Gets a reference to this entry’s name, if it’s a property entry.
/// @param entry The entry to work on.
/// @return Pointer to name, if present.
KDL_NULLABLE
struct KDL_Identifier const*
KDL_Entry_name(
	KDL_THIS_CONST struct KDL_Entry const* entry);

/// @brief Gets the entry’s value.
/// @param entry The entry to work on.
/// @return Pointer to value.
KDL_NONNULL
struct KDL_Value const*
KDL_Entry_value(
	KDL_THIS_CONST struct KDL_Entry const* entry);

/// @brief Gets the entry’s type.
/// @param entry The entry to work on.
/// @return The entry’s type, if present.
KDL_NULLABLE
struct KDL_Identifier const*
KDL_Entry_ty(
	KDL_THIS_CONST struct KDL_Entry const* entry);

/// @brief Stride in bytes between nodes.
extern size_t KDL_Entry_sizeof;

#pragma endregion

#pragma region kdl::identifier

/// @brief Gets the string value for this identifier.
/// @param identifier The identifier to work on.
/// @param length (out) The length of the string value.
/// @return Pointer to the string value.
KDL_ARRAY(*length)
char8_t const*
KDL_Identifier_value(
	KDL_THIS_CONST struct KDL_Identifier const* identifier,
	KDL_OUT size_t* length);

#pragma endregion

#pragma region kdl::node

/// @brief Gets this node’s name.
/// @param node The node to work on.
/// @return Pointer to the node’s name.
KDL_NONNULL
struct KDL_Identifier const*
KDL_Node_name(
	KDL_THIS_CONST struct KDL_Node const* node);

/// @brief Gets the node’s type identifier, if any.
/// @param node The node to work on.
/// @return Pointer to the node’s type identifier, if any.
KDL_NULLABLE
struct KDL_Identifier const*
KDL_Node_ty(
	KDL_THIS_CONST struct KDL_Node const* node);

/// @brief Returns a reference to this node’s entries (arguments and properties).
/// @param node The node to work on.
/// @param length (out) The number of entries.
/// @return Pointer to first entry (stride by `KDL_Entry_sizeof` bytes).
KDL_NULLABLE
KDL_ARRAY_B(*length, KDL_Entry_sizeof)
struct KDL_Entry const*
KDL_Node_entries(
	KDL_THIS_CONST struct KDL_Node const* node,
	KDL_OUT size_t* length);

/// @brief Fetches the first property entry with a matching name.
/// @param node The node to work on.
/// @param name Pointer to name.
/// @param length Length of name.
/// @return Pointer to found entry, if present.
KDL_NULLABLE
struct KDL_Entry const*
KDL_Node_get_prop(
	KDL_THIS_CONST struct KDL_Node const* node,
	KDL_INPTR_ARRAY(length) char8_t const name[],
	size_t length);

/// @brief Fetches the argument entry at a given index.
/// @param node The node to work on.
/// @param index The index of the argument to fetch.
/// @return Pointer to found entry, if present.
KDL_NULLABLE
struct KDL_Entry const*
KDL_Node_get_arg(
	KDL_THIS_CONST struct KDL_Node const* node,
	size_t index);

/// @brief Returns a reference to this node’s children, if any.
/// @param node The node to work on.
/// @return The node’s children, if any.
KDL_NULLABLE
struct KDL_Document const*
KDL_Node_children(
	KDL_THIS_CONST struct KDL_Node const* node);

/// @brief Stride in bytes between nodes.
extern size_t KDL_Node_sizeof;

#pragma endregion

#pragma region kdl::value

/// @brief Extract the string value from a value.
/// @param value The value to work on.
/// @param string (out) The string value.
/// @param length (out) The length of string value.
/// @return If the value is a string.
KDL_FALLIBLE
bool
KDL_Value_string(
	KDL_THIS_CONST struct KDL_Value const* value,
	KDL_OUTPTR_ARRAY(*length) char8_t const** string,
	KDL_OUT size_t* length);

/// @brief Extract the integer value from a value.
/// @param value The value to work on.
/// @param integer (out) The integer value.
/// @return If the value is an integer.
KDL_FALLIBLE
bool
KDL_Value_int(
	KDL_THIS_CONST struct KDL_Value const* value,
	KDL_OUT int64_t* i);

/// @brief Extract the floating-point value from a value.
/// @param value The value to work on.
/// @param floating (out) The floating-point value.
/// @return If the value is a floating-point.
KDL_FALLIBLE
bool
KDL_Value_float(
	KDL_THIS_CONST struct KDL_Value const* value,
	KDL_OUT double* floating);

/// @brief Extract the boolean value from a value.
/// @param value The value to work on.
/// @param boolean (out) The boolean value.
/// @return If the value is a boolean.
KDL_FALLIBLE
bool
KDL_Value_bool(
	KDL_THIS_CONST struct KDL_Value const* value,
	KDL_OUT bool* boolean);

/// @brief Extract the null value from a value.
/// @param value The value to work on.
/// @return If the value is a null.
KDL_FALLIBLE
bool
KDL_Value_null(
	KDL_THIS_CONST struct KDL_Value const* value);

/// @brief Determine which kind of value this is.
/// @param value The value to work on.
/// @return The value’s kind.
KdlValueWhich
KDL_Value_which(
	KDL_THIS_CONST struct KDL_Value const* value);

#pragma endregion

#pragma region kdl::error

/// @brief Free a KDL error.
/// @param error The error to free.
void
KDL_Error_free(
	KDL_THIS_MUT struct KDL_Error* error);

/// @brief Source string for the KDL document that failed to parse.
/// @param error The error to work on.
/// @param length (out) The length of the source string.
/// @return Pointer to source string.
KDL_ARRAY(*length)
char8_t const*
KDL_Error_input(
	KDL_THIS_CONST struct KDL_Error const* error,
	KDL_OUT size_t* length);

/// @brief Span of the error in the source string.
/// @param error The error to work on.
/// @param length (out) The length of the error span.
/// @return Pointer to the error span.
KDL_ARRAY(*length)
char8_t const*
KDL_Error_span(
	KDL_THIS_CONST struct KDL_Error const* error,
	KDL_OUT size_t* length);

/// @brief Label for the error, if present.
/// @param error The error to work on.
/// @param length (out) The length of the label.
/// @return Pointer to the label.
KDL_ARRAY(*length)
char8_t const*
KDL_Error_label(
	KDL_THIS_CONST struct KDL_Error const* error,
	KDL_OUT size_t* length);

/// @brief Help text for the error, if present.
/// @param error The error to work on.
/// @param length (out) The length of the help text.
/// @return Pointer to the help text.
KDL_ARRAY(*length)
char8_t const*
KDL_Error_help(
	KDL_THIS_CONST struct KDL_Error const* error,
	KDL_OUT size_t* length);

#pragma endregion

#ifdef __cplusplus
}
#endif

#endif // KDL_H
