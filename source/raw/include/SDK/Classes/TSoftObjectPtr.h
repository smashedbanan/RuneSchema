#pragma once

#include "SDK/Classes/TPersistentObjectPtr.h"
#include "SDK/Structs/FSoftObjectPtr.h"

namespace UECustom {
#ifndef _WIN32
    // Clang expands UE4SS's UE_REQUIRES to a constraint that names UE:: without RC::Unreal::.
    namespace UE = RC::Unreal::UE;
#endif
    template<typename UEType>
    class TSoftObjectPtr
    {
    public:
        FORCEINLINE TSoftObjectPtr() {};

        explicit FORCEINLINE TSoftObjectPtr(FSoftObjectPath ObjectPath) : SoftObjectPtr(ObjectPath)
        {
        }

        template <
            class U
            UE_REQUIRES(std::is_convertible_v<U*, UEType*>)
        >
        FORCEINLINE TSoftObjectPtr(const TSoftObjectPtr<U>& Other)
            : SoftObjectPtr(Other.SoftObjectPtr)
        {
        }

        const FSoftObjectPtr Get() const
        {
            return SoftObjectPtr;
        }
    private:
        FSoftObjectPtr SoftObjectPtr;
    };
}
