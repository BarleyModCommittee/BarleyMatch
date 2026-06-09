// dllmain.cpp : 定义 DLL 应用程序的入口点。
import index;
import Setup;
import MatchCycle;
import LuaEnv;

#define DLL_PROCESS_ATTACH   1    
#define DLL_THREAD_ATTACH    2    
#define DLL_THREAD_DETACH    3    
#define DLL_PROCESS_DETACH   0

BOOL __stdcall DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		PVZ::Plant::MemSize = 0x160;
		PVZ::Projectile::MemSize = 0x94;
		LuaEnvInit();
        SetupEvents();
        MatchCycleInit();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return true;
}

