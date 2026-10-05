#pragma once
#include <string_view>

namespace PS {
template <class Ch>
struct BasicSoftPathParts {
    std::basic_string_view<Ch> package, asset, subobject;
    bool valid{};
};
using SoftPathParts = BasicSoftPathParts<wchar_t>;

template <class Ch>
BasicSoftPathParts<Ch> SplitSoftPathView(std::basic_string_view<Ch> path) {
    using View = std::basic_string_view<Ch>;
    static constexpr Ch DotColon[] = {'.', ':', 0}, PathMarks[] = {'.', '/', ':', 0},
        DotDot[] = {'.', '.', 0}, ColonColon[] = {':', ':', 0};
    if (path.empty() || path.front() != Ch('/') || path.back() == Ch('.') || path.back() == Ch(':')) return {};
    const auto dot = path.find_first_of(DotColon);
    if (dot == View::npos) return {path, {}, {}, true};
    if (path[dot] != Ch('.') || dot <= 1) return {};
    const auto colon = path.find(Ch(':'), dot + 1);
    const auto asset = path.substr(dot + 1, colon == View::npos ? colon : colon - dot - 1);
    if (asset.empty() || asset.find_first_of(PathMarks) != View::npos) return {};
    const auto subobject = colon == View::npos ? View{} : path.substr(colon + 1);
    if (!subobject.empty() && (subobject.front() == Ch('.') || subobject.front() == Ch(':')
        || subobject.find(DotDot) != View::npos || subobject.find(ColonColon) != View::npos)) return {};
    return {path.substr(0, dot), asset, subobject, true};
}
// Engine text is UTF-16 in wchar_t on Windows and in char16_t elsewhere.
inline SoftPathParts SplitSoftPath(std::wstring_view path) { return SplitSoftPathView(path); }
inline BasicSoftPathParts<char16_t> SplitSoftPath(std::u16string_view path) { return SplitSoftPathView(path); }
}
