#include "kdl.hxx"

#include <format>
#include <iomanip>
#include <iostream>
#include <type_traits>
#include <utility>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace {
kdl::document_ptr parse(std::u8string_view source)
{
	auto result = kdl::document::parse(source);
	return std::visit([](auto&& v) {
		using V = std::remove_cvref_t<decltype(v)>;
		if constexpr (std::is_same_v<V, kdl::document_ptr>) {
			return std::move(v);
		}
		else if constexpr (std::is_same_v<V, kdl::error_ptr>) {
			throw std::move(v);
			return kdl::document_ptr();
		}
		else {
			static_assert(false, "unreachable");
		}
		}, std::move(result));
}

std::string fix(std::u8string_view view) {
	return std::string(view.begin(), view.end());
}

void dump(kdl::node const&, size_t);
void dump(kdl::entry const&, size_t);
void dump(kdl::document const& document, size_t indent = 0) {
	std::cout
		<< std::nounitbuf
		<< (indent ? "\n" : "")
		<< std::setw(indent) << ""
		<< "(children"
		;
	bool hasChildren = false;
	for (auto const& child : document) {
		hasChildren = true;
		dump(child, indent + 2);
	}
	if (hasChildren) {
		std::cout << "\n" << std::setw(indent) << "";
	}
	std::cout << ")";
}

void dump(kdl::node const& node, size_t indent) {
	std::cout
		<< "\n"
		<< std::setw(indent) << ""
		<< "(node \""
		<< fix(node.name()->string())
		<< "\""
		;
	auto ty = node.ty();
	if (ty) {
		std::cout << " \"" << fix(ty->string()) << "\"";
	}
	bool hasChildren = false;
	for (auto const& entry : node) {
		hasChildren = true;
		dump(entry, indent + 2);
	}
	auto children = node.children();
	if (children) {
		hasChildren = true;
		dump(*children, indent + 2);
	}
	if (hasChildren) {
		std::cout << "\n" << std::setw(indent) << "";
	}
	std::cout << ")";
}

void dump(kdl::entry const& entry, size_t indent) {
	std::cout
		<< "\n"
		<< std::setw(indent) << ""
		<< "(entry "
		;
	auto name = entry.name();
	if (name) {
		std::cout << "\"" << name << "\"" << " ";
	}
	auto const& value = *entry.value();
	std::visit([](auto const& v) {
		using V = std::remove_cvref_t<decltype(v)>;
		if constexpr (std::is_same_v<V, std::monostate>) {
			std::cout << "(null)";
		}
		else if constexpr (std::is_same_v<V, std::u8string_view>) {
			std::cout << fix(v);
		}
		else {
			std::cout << v;
		}
		}, value.which());
	std::cout << ")";
}

}

int main() {
	SetConsoleOutputCP(CP_UTF8);
	setvbuf(stdout, nullptr, _IOFBF, 1000);

	std::u8string_view text = u8R"#(
contents {
	section "First section" {
		paragraph "This is the first paragraph"
	}
}
)#";

	try {
		auto document = parse(text);
		dump(*document);
	}
	catch (kdl::error_ptr& e) {
		std::cout
			<<
			"Parse error("
			<< e->span().data() - e->input().data()
			<< "): "
			<< fix(e->label())
			<< "; "
			<< fix(e->help())
			;
	}
	std::cout << std::endl;
}

