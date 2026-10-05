#include "Utility/AssetAliases.h"
#include <cassert>
int main() {
    PS::AssetAliases::Registry aliases;
    const std::wstring source=L"/Game/Mods/Set/Hood.Hood",runtime=L"/Game/RuneSchema/Armor/Items/hood.hood";
    assert(aliases.Resolve(source)==source);
    aliases.Add(source,runtime);
    assert(aliases.Resolve(source)==runtime);
    assert(aliases.Resolve(runtime)==runtime);
    assert(aliases.Resolve(L"/Game/Vanilla.Item")==L"/Game/Vanilla.Item");
    aliases.Add(source,runtime);
    bool conflict=false;try{aliases.Add(source,L"/Game/Other.Item");}catch(const std::exception&){conflict=true;}
    assert(conflict && aliases.Resolve(source)==runtime);
    aliases.Clear();assert(aliases.Resolve(source)==source);
    // UE4SS text is UTF-16 in char16_t outside Windows. Explicit returns: assert is compiled out
    // of Release builds.
    PS::AssetAliases::BasicRegistry<std::u16string> utf16;
    utf16.Add(u"/Game/Mods/Set/Hood.Hood",u"/Game/RuneSchema/Armor/Items/hood.hood");
    if(utf16.Resolve(u"/Game/Mods/Set/Hood.Hood")!=u"/Game/RuneSchema/Armor/Items/hood.hood")return 1;
    if(utf16.Resolve(u"/Game/Vanilla.Item")!=u"/Game/Vanilla.Item")return 1;
}
