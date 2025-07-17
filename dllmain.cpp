#include <Windows.h>
#include <iostream>
#include "render.hpp"
#include "hack.hpp"

DWORD WINAPI MainThread(HMODULE hModule) {
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    std::cout << "[+] DLL Injected!" << std::endl;

    if (!SetupD3D11Hook()) {
        std::cout << "[-] Failed to hook D3D11!" << std::endl;
        return 1;
    }

    while (!GetAsyncKeyState(VK_END)) {
        RunHack();
        Sleep(1);
    }

    Cleanup();
    fclose(stdout);
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
    }
    return TRUE;
}