#ifndef VIBRANCY_WINDOWS_H_
#define VIBRANCY_WINDOWS_H_

#include <windows.h>
#include <iostream>
namespace WindowEffects
{
std::string osVersion = "";
HWND windowHandle = nullptr;

//Windows 7
typedef struct _DWM_BLURBEHIND
{
    DWORD dwFlags;
    BOOL fEnable;
    HRGN hRgnBlur;
    BOOL fTransitionOnMaximized;
} DWM_BLURBEHIND, *PDWM_BLURBEHIND;
#define DWM_BB_ENABLE 0x1

//Windows 10/11
struct ACCENTPOLICY
{
    int nAccentState;
    int nFlags;
    int nColor;
    int nAnimationId;
};
struct WINCOMPATTRDATA
{
    int nAttribute;
    PVOID pData;
    ULONG ulDataSize;
};
enum AccentTypes
{
    ACCENT_DISABLE = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
    ACCENT_ENABLE_BLURBEHIND = 3
};
#define WCA_ACCENT_POLICY 0x13

BOOL CALLBACK enumWindowsCallback(HWND hWnd, LPARAM lParam)
{
    if (IsWindowVisible(hWnd))
    {
        DWORD winId;
        GetWindowThreadProcessId(hWnd, &winId);
        if(winId==lParam)
        {
            windowHandle=hWnd;
        }
    }
    return TRUE;
}

void detectOSVersion()
{
    typedef NTSTATUS (WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);
    RTL_OSVERSIONINFOW rovi = { 0 };
    HMODULE hMod = ::GetModuleHandleW(L"ntdll.dll");
    if (hMod)
    {
        RtlGetVersionPtr versionPtr = (RtlGetVersionPtr)::GetProcAddress(hMod, "RtlGetVersion");
        if (versionPtr != nullptr)
        {
            rovi.dwOSVersionInfoSize = sizeof(rovi);
            versionPtr(&rovi);
        }
    }

    if (rovi.dwMajorVersion == 6 && rovi.dwMinorVersion == 1)
    {
        osVersion="WIN7";
    }
    else if(rovi.dwBuildNumber>=17763 && rovi.dwBuildNumber < 21996)
    {
        osVersion="WIN10";
    }
    else if(rovi.dwBuildNumber >= 21996)
    {
        osVersion="WIN11";
    }
    else
    {
        osVersion="UNSUPPORTED";
    }
}

inline bool vibrancy(bool state, DWORD pID)
{
    windowHandle = nullptr;
    EnumWindows(enumWindowsCallback, pID);
    if(windowHandle != nullptr)
    {
        if(osVersion=="")
        {
            detectOSVersion();
        }

        if(osVersion=="WIN7")
        {
            const HINSTANCE hModule = LoadLibrary(TEXT("dwmapi.dll"));
            if (hModule)
            {
                DWM_BLURBEHIND bb = {0};
                typedef HRESULT (WINAPI *pDwmEnableBlurBehindWindow)(HWND hWnd, const DWM_BLURBEHIND* bb);
                HRESULT hr = S_OK;

                bb.dwFlags = DWM_BB_ENABLE;
                bb.fEnable = state;
                bb.hRgnBlur = nullptr;

                pDwmEnableBlurBehindWindow DwmEnableBlurBehindWindow = reinterpret_cast<pDwmEnableBlurBehindWindow>(GetProcAddress (hModule, "DwmEnableBlurBehindWindow"));
                hr =  DwmEnableBlurBehindWindow(windowHandle, &bb);
                if (SUCCEEDED(hr))
                {
                    return true;
                }
                FreeLibrary (hModule);
            }

        }
        else if(osVersion=="WIN10" || osVersion=="WIN11")
        {
            bool result = false;
            const HINSTANCE hModule = LoadLibrary(TEXT("user32.dll"));
            if (hModule)
            {
                typedef BOOL(WINAPI*pSetWindowCompositionAttribute)(HWND, WINCOMPATTRDATA*);
                const pSetWindowCompositionAttribute SetWindowCompositionAttribute = (pSetWindowCompositionAttribute)GetProcAddress(hModule, "SetWindowCompositionAttribute");
                if (SetWindowCompositionAttribute)
                {
                    ACCENTPOLICY policy =
                    {
                        state ? ACCENT_ENABLE_BLURBEHIND
                        : ACCENT_DISABLE, 0, 0, 0
                    };
                    WINCOMPATTRDATA data = {WCA_ACCENT_POLICY, &policy, sizeof(ACCENTPOLICY) };

                    result = SetWindowCompositionAttribute(windowHandle, &data);
                }
                FreeLibrary(hModule);
                return result;
            }
        }
    }

    return false;
}
}
#endif
