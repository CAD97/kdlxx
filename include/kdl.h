#ifndef KDL_H
#define KDL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if (defined(__STDC__) && __STDC_VERSION_STDINT_H__ < 202301L) || \
	(defined(__cplusplus) && __cpp_char8_t < 201811L) || \
	(!defined(__STDC__) && !defined(__cplusplus))
typedef unsigned char char8_t;
#endif

#pragma region SAL
#ifdef _MSC_VER
#define SAL_OUT _Out_
#define SAL_OUTPTR _Outptr_
#define SAL_OUTPTR_NULLABLE _Outptr_result_maybenull_
#define SAL_OPT_OUTPTR_NULLABLE _Outptr_opt_result_maybenull_
#define SAL_RETPTR _Ret_
#define SAL_RETPTR_NULLABLE _Ret_maybenull_
#define SAL_INPTR _In_
#define SAL_INARR(len) _In_reads_(len)
#elif __GNUC__ || __clang__
#define SAL_OUT
#define SAL_OUTPTR
#define SAL_OUTPTR_NULLABLE
#define SAL_OPT_OUTPTR_NULLABLE
#define SAL_RETPTR __attribute__((returns_nonnull))
#define SAL_RETPTR_NULLABLE
#define SAL_INPTR
#define SAL_INARR(len)
#else
#define SAL_OUT
#define SAL_OUTPTR
#define SAL_OUTPTR_NULLABLE
#define SAL_OPT_OUTPTR_NULLABLE
#define SAL_RETPTR
#define SAL_RETPTR_NULLABLE
#define SAL_INPTR
#define SAL_INARR(len)
#endif // _MSC_VER
#pragma endregion

/// @brief Represents a KDL Document.
struct KDL_Document;
/// @brief KDL Entries are the “arguments” to KDL nodes: either a (positional) Argument or a (key/value) Property
struct KDL_Entry;
/// @brief An error that occurs when parsing a KDL document.
struct KDL_Error;
/// @brief Represents a KDL Identifier.
struct KDL_Identifier;
/// @brief Represents an individual KDL Node inside a KDL Document.
struct KDL_Node;
/// @brief A specific KDL Value.
struct KDL_Value;

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

#pragma region kdl::document
/// @brief Free a KDL Document.
/// @param document The document to free.
void
KDL_Document_free(
	struct KDL_Document* document);
/// @brief Gets this node’s name.
/// @param string Pointer to UTF-8 document.
/// @param len Length of UTF-8 document.
/// @param document On successs, outptr set to owning pointer to parsed document.
/// @param error On failure, outptr set to owning pointer to parse error.
/// @return Boolean indicating success.
bool
KDL_Document_parse(
	SAL_INARR(len) char8_t const string[],
	size_t len,
	SAL_OUTPTR_NULLABLE struct KDL_Document** document,
	SAL_OUTPTR_NULLABLE struct KDL_Error** error);
/// @brief Gets the first child node with a matching name.
/// @param document The document to work on.
/// @param name Pointer to name.
/// @param len Length of name.
/// @return Pointer to found node, if present.
SAL_RETPTR_NULLABLE struct KDL_Node const*
KDL_Document_get(
	SAL_INPTR struct KDL_Document const* document,
	SAL_INARR(len) char8_t const name[],
	size_t len);
/// @brief Gets the first argument (value) of the first child node with a matching name.
/// @param document The document to work on.
/// @param name Pointer to name.
/// @param len Length of name.
/// @return Pointer to found value, if present.
SAL_RETPTR_NULLABLE struct KDL_Value const*
KDL_Document_get_arg(
	SAL_INPTR struct KDL_Document const* document,
	SAL_INARR(len) char8_t const name[],
	size_t len);
/// @brief Returns a reference to this document’s child nodes.
/// @param document The document to work on.
/// @param node Outptr set to the first child node.
/// @return The number of child nodes (stride by `KDL_Node_sizeof` bytes).
size_t
KDL_Document_nodes(
	SAL_INPTR struct KDL_Document const* document,
	SAL_OUTPTR struct KDL_Node const** node);
#pragma endregion

#pragma region kdl::entry
/// @brief Gets a reference to this entry’s name, if it’s a property entry.
/// @param entry The entry to work on.
/// @return Pointer to name, if present.
SAL_RETPTR_NULLABLE struct KDL_Identifier const*
KDL_Entry_name(
	SAL_INPTR struct KDL_Entry const* entry);
/// @brief Gets the entry’s value.
/// @param entry The entry to work on.
/// @return Pointer to value.
SAL_RETPTR struct KDL_Value const*
KDL_Entry_value(
	SAL_INPTR struct KDL_Entry const* entry);
/// @brief Gets the entry’s type.
/// @param entry The entry to work on.
/// @return The entry’s type, if present.
SAL_RETPTR_NULLABLE struct KDL_Identifier const*
KDL_Entry_ty(
	SAL_INPTR struct KDL_Entry const* entry);
/// @brief Stride in bytes between nodes.
extern size_t KDL_Entry_sizeof;
#pragma endregion

#pragma region kdl::error
/// @brief Free a KDL error.
/// @param error The error to free.
void
KDL_Error_free(
	struct KDL_Error* error);
/// @brief Source string for the KDL document that failed to parse.
/// @param error The error to work on.
/// @return Pointer to source string.
size_t
KDL_Error_input(
	SAL_INPTR struct KDL_Error const* error,
	SAL_OUTPTR char8_t const** string);
/// @brief Span of the error in the source string.
/// @param error The error to work on.
/// @param offset Outptr set to the offset of the error span.
/// @param length Outptr set to the length of the error span.
void
KDL_Error_span(
	SAL_INPTR struct KDL_Error const* error,
	SAL_OUT size_t* offset,
	SAL_OUT size_t* length);
/// @brief Label for the error, if present.
/// @param error The error to work on.
/// @param string Outptr set to the label, if present.
/// @return The length of the label.
size_t
KDL_Error_label(
	SAL_INPTR struct KDL_Error const* error,
	SAL_OUTPTR_NULLABLE char8_t const** string);
/// @brief Help text for the error, if present.
/// @param error The error to work on.
/// @param string Outptr set to the help text, if present.
/// @return The length of the help text.
size_t
KDL_Error_help(
	SAL_INPTR struct KDL_Error const* error,
	SAL_OUTPTR_NULLABLE char8_t const** string);
#pragma endregion

#pragma region kdl::identifier
/// @brief Gets the string value for this identifier.
/// @param identifier The identifier to work on.
/// @param string Outptr set to the string value.
/// @return The length of the string value.
size_t
KDL_Identifier_value(
	SAL_INPTR struct KDL_Identifier const* identifier,
	SAL_OUTPTR char8_t const** string);
#pragma endregion

#pragma region kdl::node
/// @brief Gets this node’s name.
/// @param node The node to work on.
/// @return Pointer to the node’s name.
SAL_RETPTR struct KDL_Identifier const*
KDL_Node_name(
	struct KDL_Node const* node);
/// @brief Gets the node’s type identifier, if any.
/// @param node The node to work on.
/// @return Pointer to the node’s type identifier, if any.
SAL_RETPTR_NULLABLE struct KDL_Identifier const*
KDL_Node_ty(
	struct KDL_Node const* node);
/// @brief Returns a reference to this node’s entries (arguments and properties).
/// @param node The node to work on.
/// @param entry Outptr set to the first entry.
/// @return The number of entries (stride by `KDL_Entry_sizeof` bytes).
size_t
KDL_Node_entries(
	SAL_INPTR struct KDL_Node const* node,
	SAL_OUTPTR struct KDL_Entry const** entry);
/// @brief Fetches the first property entry with a matching name.
/// @param node The node to work on.
/// @param name Pointer to name.
/// @param len Length of name.
/// @return Pointer to found entry, if present.
SAL_RETPTR_NULLABLE struct KDL_Entry const*
KDL_Node_get_prop(
	SAL_INPTR struct KDL_Node const* node,
	SAL_INARR(len) char8_t const name[],
	size_t len);
/// @brief Fetches the argument entry at a given index.
/// @param node The node to work on.
/// @param ix The index of the argument to fetch.
/// @return Pointer to found entry, if present.
SAL_RETPTR_NULLABLE struct KDL_Entry const*
KDL_Node_get_arg(
	SAL_INPTR struct KDL_Node const* node,
	size_t ix);
/// @brief Returns a reference to this node’s children, if any.
/// @param node The node to work on.
SAL_RETPTR_NULLABLE struct KDL_Document const*
KDL_Node_children(
	SAL_INPTR struct KDL_Node const* node);
/// @brief Stride in bytes between nodes.
extern size_t KDL_Node_sizeof;
#pragma endregion

#pragma region kdl::value
/// @brief Extract the string value from a value.
/// @param value The value to work on.
/// @param string Outptr set to the string value.
/// @param len Outptr set to the length of the string value.
/// @return If the value is a string.
bool
KDL_Value_string(
	SAL_INPTR struct KDL_Value const* value,
	SAL_OUTPTR_NULLABLE char8_t const** string,
	SAL_OUT size_t* len);
/// @brief Extract the integer value from a value.
/// @param value The value to work on.
/// @param integer Outptr set to the integer value.
/// @return If the value is an integer.
bool
KDL_Value_int(
	SAL_INPTR struct KDL_Value const* value,
	SAL_OUT int64_t* i);
/// @brief Extract the floating-point value from a value.
/// @param value The value to work on.
/// @param floating Outptr set to the floating-point value.
/// @return If the value is a floating-point.
bool
KDL_Value_float(
	SAL_INPTR struct KDL_Value const* value,
	SAL_OUT double* floating);
/// @brief Extract the boolean value from a value.
/// @param value The value to work on.
/// @param boolean Outptr set to the boolean value.
/// @return If the value is a boolean.
bool
KDL_Value_bool(
	SAL_INPTR struct KDL_Value const* value,
	SAL_OUT bool* boolean);
/// @brief Extract the null value from a value.
/// @param value The value to work on.
/// @return If the value is a null.
bool
KDL_Value_null(
	SAL_INPTR struct KDL_Value const* value);
/// @brief Determine which kind of value this is.
/// @param value The value to work on.
/// @return The value’s kind.
KdlValueWhich
KDL_Value_which(
	SAL_INPTR struct KDL_Value const* value);
#pragma endregion

#endif // KDL_H
