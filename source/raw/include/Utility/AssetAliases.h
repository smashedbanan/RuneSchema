#pragma once
#include <map>
#include <mutex>
#include <string>
#include <stdexcept>

namespace PS::AssetAliases {
template <class String>
class BasicRegistry {
    std::map<String,String> paths;
public:
    void Add(const String& authored,const String& runtime) {
        const auto slash=typename String::value_type('/');
        if(authored.empty() || runtime.empty() || authored.front()!=slash || runtime.front()!=slash)
            throw std::runtime_error("Asset alias requires absolute object paths");
        const auto found=paths.find(authored);
        if(found!=paths.end() && found->second!=runtime)throw std::runtime_error("Asset alias ownership collision");
        paths.insert_or_assign(authored,runtime);
    }
    String Resolve(const String& path) const {
        const auto found=paths.find(path);
        return found==paths.end()?path:found->second;
    }
    void Clear(){paths.clear();}
};
using Registry=BasicRegistry<std::wstring>;
// Engine text as UE4SS holds it (RC::StringType): UTF-16 in wchar_t on Windows, in char16_t elsewhere.
#ifdef _WIN32
using EngineString=std::wstring;
#else
using EngineString=std::u16string;
#endif
inline BasicRegistry<EngineString> registry;
inline std::mutex mutex;
inline void Add(const EngineString& authored,const EngineString& runtime){std::scoped_lock lock(mutex);registry.Add(authored,runtime);}
inline EngineString Resolve(const EngineString& path){std::scoped_lock lock(mutex);return registry.Resolve(path);}
inline void Clear(){std::scoped_lock lock(mutex);registry.Clear();}
}
