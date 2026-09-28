#include "AppWindow.h"

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int) {
    AppWindow window;
    if (!window.Create(instance)) return 1;
    return window.Run();
}
