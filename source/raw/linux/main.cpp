// Linux entry point: the headless dedicated-server build, loaded by UE4SS as
// Mods/RuneSchema/dlls/main.so. The Windows entry point, with the settings UI and the authoring
// tools, is src/dllmain.cpp.
#include <atomic>
#include <exception>
#include <string>
#include "Mod/CppUserModBase.hpp"
#include "Loader/DragonWildsMainLoader.h"
#include "Runtime/Storefront.h"
#include "SDK/DragonWildsSignatures.h"
#include "Utility/BuildInfo.h"
#include "Utility/Config.h"
#include "Utility/Logging.h"
#include "Utility/StartupTrace.h"

using namespace RC;

class RuneSchema : public RC::CppUserModBase
{
public:
    RuneSchema() : CppUserModBase()
    {
        ModName = STR("RuneSchema");
        ModVersion = PS::ToWideSafe(PS::BuildInfo::Version);
        ModDescription = STR("Allows modifying of DragonWilds's assets dynamically.");
        ModAuthors = STR("See RuneSchema Settings > Attribution");

        PS::StartupTrace::Mark("config load begin");
        PS::PSConfig::Get()->Load();
        RC::Output::send<RC::LogLevel::Normal>(STR("[RuneSchema] Native binding lane: {}.\n"),
            PS::ToWideSafe(PS::Storefront::NativeLaneName(PS::Storefront::CurrentNativeLane())));
        PS::StartupTrace::Mark("signature scan begin");
        DragonWilds::SignatureManager::Initialize();
        PS::StartupTrace::Mark("signature scan complete; early hooks begin");
        MainLoader.PreInitialize();
        PS::StartupTrace::Mark("early hooks complete");

        RC::Output::send<RC::LogLevel::Normal>(
            STR("[RuneSchema] v{} loaded | UE4SS | Linux dedicated server.\n"), ModVersion);
    }

    auto on_unreal_init() -> void override
    {
        if (m_startupFailed.load(std::memory_order_acquire)) return;
        try {
            PS::StartupTrace::Mark("UE4SS on_unreal_init begin");
            MainLoader.SetFatalStartupHandler([this](std::string) {
                m_startupFailed.store(true, std::memory_order_release);
            });
            MainLoader.Initialize();
            PS::StartupTrace::Mark("UE4SS on_unreal_init complete");
        } catch (const std::exception& error) {
            m_startupFailed.store(true, std::memory_order_release);
            MainLoader.AbortStartup("unreal-init", error.what());
        } catch (...) {
            m_startupFailed.store(true, std::memory_order_release);
            MainLoader.AbortStartup("unreal-init", "unknown exception");
        }
    }

private:
    DragonWilds::DragonWildsMainLoader MainLoader;
    std::atomic<bool> m_startupFailed = false;
};

// Clang ignores __declspec(dllexport) on Linux: default visibility is what exports these.
#define RuneSchema_API __attribute__((visibility("default")))
extern "C"
{
    RuneSchema_API RC::CppUserModBase* start_mod()
    {
        try {
            return new RuneSchema();
        } catch (const std::exception& error) {
            PS::StartupTrace::Fatal("construction", error.what());
            try { PS::Log<LogLevel::Error>(STR("[RuneSchema][DID-NOT-START][CONSTRUCTION] {}.\n"), PS::ToWideSafe(error.what())); } catch (...) {}
            return nullptr;
        } catch (...) {
            PS::StartupTrace::Fatal("construction", "unknown exception");
            try { PS::Log<LogLevel::Error>(STR("[RuneSchema][DID-NOT-START][CONSTRUCTION] Unknown exception.\n")); } catch (...) {}
            return nullptr;
        }
    }

    RuneSchema_API void uninstall_mod(RC::CppUserModBase* mod)
    {
        delete mod;
    }
}
