#ifndef KDL_HXX
#define KDL_HXX

#include <cassert>
#include <cstddef>
#include <cstdint>

extern "C" {
#include "kdl.h"
}

#define KDL_NULLABLE SAL_RETPTR_NULLABLE
#define KDL_NONNULL  SAL_RETPTR
#define KDL_OPAQUE(T) \
	T() = delete; \
	~T() = delete; \
	T(T&) = delete; \
	T(T&&) = delete; \
	T& operator=(T&) = delete; \
	T& operator=(T&&) = delete;

#include <iterator>
#include <memory>
#include <optional>
#include <span>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>

namespace kdl {

/// @brief Represents a KDL Document.
using document = KDL_Document;
/// @brief KDL Entries are the �arguments� to KDL nodes: either a (positional) Argument or a (key/value) Property
using entry = KDL_Entry;
/// @brief An error that occurs when parsing a KDL document.
using error = KDL_Error;
/// @brief Represents a KDL Identifier.
using identifier = KDL_Identifier;
/// @brief Represents an individual KDL Node inside a KDL Document.
using node = KDL_Node;
/// @brief A specific KDL Value.
using value = KDL_Value;

namespace detail {
struct document_deleter;
struct error_deleter;
template<typename T>
class iterator;
} // namespace kdl::detail

using document_ptr = std::unique_ptr<document, detail::document_deleter>;
using error_ptr = std::unique_ptr<error, detail::error_deleter>;

template<typename T>
using slice = std::counted_iterator<detail::iterator<T>>;

namespace detail {

struct document_deleter {
	void operator()(document* doc) const {
		KDL_Document_free(doc);
	}
};

struct error_deleter {
	void operator()(error* err) const {
		KDL_Error_free(err);
	}
};

template<typename T>
class iterator {
public:
	using difference_type = ptrdiff_t;
	using value_type = std::remove_cv_t<T>;
	using pointer = T*;
	using reference = T&;
	using iterator_category = std::contiguous_iterator_tag;

public:
	iterator() = default;
	explicit iterator(pointer begin, ptrdiff_t stride)
		: m_begin(begin), m_stride(stride)
	{}

	iterator& operator++() { return *this += 1; }
	iterator& operator++(int) { auto tmp = *this; *this += 1; return tmp; }
	iterator& operator--() { return *this -= 1; }
	iterator& operator--(int) { auto tmp = *this; *this -= 1; return tmp; }
	iterator& operator-=(difference_type n) { return *this = *this - n; }
	iterator& operator+=(difference_type n) { return *this = *this + n; }

	reference operator[](difference_type n) { return *(*this + n); }
	reference operator*() { return *m_begin; }

	friend auto operator<=>(iterator self, iterator that) {
		assert(self.m_stride == that.m_stride);
		return self.m_begin <=> that.m_begin;
	}
	friend auto operator==(iterator self, iterator that) {
		assert(self.m_stride == that.m_stride);
		return self.m_begin == that.m_begin;
	}

	friend difference_type operator-(iterator self, iterator that) {
		assert(self.m_stride == that.m_stride);
		return ((std::byte*)self.m_begin - (std::byte*)that.m_begin) / self.m_stride;
	}

	friend iterator operator+(iterator self, difference_type n) {
		self.m_begin = (pointer)((std::byte*)self.m_begin + self.m_stride * n);
		return self;
	}

	friend iterator operator+(difference_type n, iterator self) { return self + n; }

private:
	T* m_begin = nullptr;
	ptrdiff_t m_stride = 0;
};

} // namespace kdl::detail

} // namespace kdl

/// @brief Represents a KDL Document.
extern "C" struct KDL_Document {
	KDL_OPAQUE(KDL_Document);

	static std::variant<kdl::document_ptr, kdl::error_ptr> parse(std::u8string_view source) {
		kdl::document* doc;
		kdl::error* err;
		if (KDL_Document_parse(source.data(), source.size(), &doc, &err)) {
			return kdl::document_ptr(doc);
		}
		else {
			return kdl::error_ptr(err);
		}
	}

	KDL_NULLABLE
		kdl::node const* get(std::u8string_view name) const {
		return KDL_Document_get(this, name.data(), name.size());
	}

	KDL_NULLABLE
		kdl::value const* get_arg(std::u8string_view name) const {
		return KDL_Document_get_arg(this, name.data(), name.size());
	}

	kdl::slice<kdl::node const> nodes() const {
		kdl::node const* head;
		size_t count = KDL_Document_nodes(this, &head);
		return { kdl::detail::iterator(head, (ptrdiff_t)KDL_Node_sizeof), (ptrdiff_t)count };
	}

	auto begin() const {
		return nodes().base();
	}

	auto end() const {
		auto children = nodes();
		return children.base() + children.count();
	}
};

/// @brief KDL Entries are the �arguments� to KDL nodes: either a (positional) Argument or a (key/value) Property
struct KDL_Entry {
	KDL_OPAQUE(KDL_Entry);

	KDL_NULLABLE
		kdl::identifier const* name() const {
		return KDL_Entry_name(this);
	}

	KDL_NONNULL
		kdl::value const* value() const {
		return KDL_Entry_value(this);
	}

	KDL_NULLABLE
		kdl::identifier const* ty() const {
		return KDL_Entry_ty(this);
	}
};

/// @brief An error that occurs when parsing a KDL document.
struct KDL_Error {
	KDL_OPAQUE(KDL_Error);

	std::u8string_view input() const {
		char8_t const* string;
		size_t length = KDL_Error_input(this, &string);
		return { string, length };
	}

	std::u8string_view span() const {
		size_t offset = 0, length = 0;
		KDL_Error_span(this, &offset, &length);
		return input().substr(offset, length);
	}

	std::u8string_view label() const {
		char8_t const* string;
		size_t length = KDL_Error_label(this, &string);
		return string ? std::u8string_view(string, length) : std::u8string_view();
	}

	std::u8string_view help() const {
		char8_t const* string;
		size_t length = KDL_Error_help(this, &string);
		return string ? std::u8string_view(string, length) : std::u8string_view();
	}
};

/// @brief Represents a KDL Identifier.
struct KDL_Identifier {
	KDL_OPAQUE(KDL_Identifier);

	std::u8string_view string() const {
		char8_t const* string;
		size_t length = KDL_Identifier_value(this, &string);
		return { string, length };
	}
};

/// @brief Represents an individual KDL Node inside a KDL Document.
struct KDL_Node {
	KDL_OPAQUE(KDL_Node);

	KDL_NONNULL
		kdl::identifier const* name() const {
		return KDL_Node_name(this);
	}

	KDL_NULLABLE
		kdl::identifier const* ty() const {
		return KDL_Node_ty(this);
	}

	kdl::slice<kdl::entry const> entries() const {
		kdl::entry const* head;
		size_t count = KDL_Node_entries(this, &head);
		return { kdl::detail::iterator(head, (ptrdiff_t)KDL_Entry_sizeof), (ptrdiff_t)count };
	}

	auto begin() const {
		return entries().base();
	}

	auto end() const {
		auto children = entries();
		return children.base() + children.count();
	}

	KDL_NULLABLE
		kdl::entry const* get(std::u8string_view name) const {
		return KDL_Node_get_prop(this, name.data(), name.size());
	}

	KDL_NULLABLE
		kdl::entry const* get(size_t ix) const {
		return KDL_Node_get_arg(this, ix);
	}

	KDL_NULLABLE
		kdl::document const* children() const {
		return KDL_Node_children(this);
	}
};

/// @brief A specific KDL Value.
struct KDL_Value {
	std::optional<std::u8string_view> string() const {
		char8_t const* string;
		size_t len;
		if (KDL_Value_string(this, &string, &len) && string && len) {
			return std::u8string_view(string, len);
		}
		else {
			return std::nullopt;
		}
	}

	std::optional<int64_t> integral() const {
		int64_t i;
		if (KDL_Value_int(this, &i)) {
			return i;
		}
		else {
			return std::nullopt;
		}
	}

	std::optional<double> floating() const {
		double f;
		if (KDL_Value_float(this, &f)) {
			return f;
		}
		else {
			return std::nullopt;
		}
	}

	std::optional<bool> boolean() const {
		bool b;
		if (KDL_Value_bool(this, &b)) {
			return b;
		}
		else {
			return std::nullopt;
		}
	}

	std::optional<std::monostate> null() const {
		if (KDL_Value_null(this)) {
			return std::monostate();
		}
		else {
			return std::nullopt;
		}
	}

	std::variant<
		std::monostate,
		std::u8string_view,
		int64_t,
		double,
		bool
	> which() const {
		switch (KDL_Value_which(this)) {
		case KDL_VALUE_WHICH_NULL:
			return *null();
		case KDL_VALUE_WHICH_RAW_STRING:
		case KDL_VALUE_WHICH_STRING:
			return *string();
		case KDL_VALUE_WHICH_BASE2:
		case KDL_VALUE_WHICH_BASE8:
		case KDL_VALUE_WHICH_BASE10:
		case KDL_VALUE_WHICH_BASE16:
			return *integral();
		case KDL_VALUE_WHICH_BASE10_FLOAT:
			return *floating();
		case KDL_VALUE_WHICH_BOOL:
			return *boolean();
		default:
			assert(false && "unreachable");
			// std::unreachable();
			return {};
		}
	}
};

#endif // KDL_HXX
