#include "AppWindow.h"
#include <objbase.h>

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int) {
    const HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    const bool comReady = SUCCEEDED(hr) || hr == S_FALSE;
    if (!comReady) return 1;

    AppWindow window;
    const bool created = window.Create(instance);
    const int result = created ? window.Run() : 1;

    CoUninitialize();
    return result;
}
