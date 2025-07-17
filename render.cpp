#include "render.hpp"
#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <iostream>
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui.h"

#pragma comment(lib, "d3d11.lib")

ID3D11Device* g_pd3dDevice = nullptr;
ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
IDXGISwapChain* g_pSwapChain = nullptr;

using PresentFn = HRESULT(__stdcall*)(IDXGISwapChain*, UINT, UINT);
PresentFn oPresent = nullptr;

HWND window = nullptr;
WNDPROC oWndProc;

LRESULT __stdcall WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;
    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
    static bool initialized = false;
    if (!initialized) {
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&g_pd3dDevice))) {
            g_pd3dDevice->GetImmediateContext(&g_pd3dDeviceContext);
            DXGI_SWAP_CHAIN_DESC sd;
            pSwapChain->GetDesc(&sd);
            window = sd.OutputWindow;
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            ImGui_ImplWin32_Init(window);
            ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
            oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
            initialized = true;
        }
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    extern void RenderOverlay();
    RenderOverlay();

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return oPresent(pSwapChain, SyncInterval, Flags);
}

#include <MinHook.h>

bool SetupD3D11Hook() {
    if (MH_Initialize() != MH_OK)
        return false;

    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = GetForegroundWindow();
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    D3D_FEATURE_LEVEL featureLevel;

    if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
        nullptr, 0, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext))) {
        return false;
    }

    void** vtable = *reinterpret_cast<void***>(g_pSwapChain);
    oPresent = (PresentFn)vtable[8];

    MH_CreateHook(vtable[8], &hkPresent, reinterpret_cast<void**>(&oPresent));
    MH_EnableHook(vtable[8]);

    return true;
}

void Cleanup() {
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)oWndProc);
}