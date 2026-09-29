#pragma once
#include <windows.h>
namespace MosuanUI {
void drawLayerPanel(HDC dc,const RECT& rc);
void drawSettingsPanel(HDC dc,const RECT& rc);
bool handlePanelClick(POINT p,const RECT& rc);
}
