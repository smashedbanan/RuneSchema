#include "Utility/SoftPathParts.h"
#include <stdexcept>
#include <string>
#include <string_view>

static void Require(bool value) { if (!value) throw std::runtime_error("Soft path split regression"); }

template <class Ch>
static std::basic_string<Ch> Text(const char* ascii)
{
    return std::basic_string<Ch>(ascii, ascii + std::char_traits<char>::length(ascii));
}

template <class Ch>
static void Check()
{
    using View = std::basic_string_view<Ch>;

    const auto full = Text<Ch>("/Game/Spell.Spell:DamageModule.Shape");
    auto parts = PS::SplitSoftPath(View(full));
    Require(parts.valid && parts.package == Text<Ch>("/Game/Spell") && parts.asset == Text<Ch>("Spell")
        && parts.subobject == Text<Ch>("DamageModule.Shape"));

    const auto item = Text<Ch>("/Game/Items/Rune.Rune");
    parts = PS::SplitSoftPath(View(item));
    Require(parts.valid && parts.asset == Text<Ch>("Rune") && parts.subobject.empty());

    const auto package = Text<Ch>("/Game/Items/Rune");
    parts = PS::SplitSoftPath(View(package));
    Require(parts.valid && parts.asset.empty());

    for (const char* bad : {"", "None", "Spell", "/Game/Spell.", "/Game/Spell.Spell:", "/Game/Spell..Spell",
            "/Game/Spell:Module", "/Game/Spell.Spell:.Module"}) {
        const auto text = Text<Ch>(bad);
        Require(!PS::SplitSoftPath(View(text)).valid);
    }

    // A view need not be NUL-terminated.
    const auto bounded = Text<Ch>("/Game/Spell.Spell:ModuleTRAILING");
    parts = PS::SplitSoftPath(View(bounded).substr(0, bounded.size() - 8));
    Require(parts.valid && parts.subobject == Text<Ch>("Module"));
}

int main()
{
    Check<wchar_t>();
    Check<char16_t>();
}
