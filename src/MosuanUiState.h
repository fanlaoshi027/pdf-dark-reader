#pragma once
#include <windows.h>
#include <array>

namespace MosuanUI {
constexpr wchar_t kMainClass[] = L"PDFDarkReaderWindow";
constexpr wchar_t kUiClass[] = L"MosuanUiOverlay";
constexpr int kRightRail = 58;
constexpr int kSlot = 46;
constexpr int kGap = 7;
constexpr int kLayerPanelWidth = 360;

enum CommandId {
    ID_OPEN=1001, ID_SAVE=1011, ID_ZOOM_OUT=1004, ID_ZOOM_IN=1005, ID_FIT_WIDTH=1009, ID_PAN=1401,
    ID_TOOL_PEN=1301, ID_TOOL_RULER=1402, ID_TOOL_LASSO=1304, ID_TOOL_ERASER=1303, ID_TOOL_LINE=1302,
    ID_PEN_BLACK=1501, ID_PEN_RED=1502, ID_PEN_BLUE=1503,
    ID_WIDTH_THIN=1511, ID_WIDTH_MEDIUM=1512, ID_WIDTH_THICK=1513, ID_DASH=1521, ID_ONE_STROKE=1522
};
constexpr UINT WM_NOTE_VISIBILITY = WM_APP + 31;
constexpr UINT WM_BACKGROUND_VISIBILITY = WM_APP + 32;
struct FavoriteSlot { bool valid=false; int command=ID_TOOL_PEN; };
struct State {
    HWND main=nullptr; HWND ui=nullptr;
    bool layersOpen=false, settingsOpen=false, backgroundVisible=true, notesVisible=true, singlePage=true;
    int selectedTool=ID_TOOL_PEN, color=ID_PEN_BLACK, width=ID_WIDTH_MEDIUM;
    bool dashed=false, oneStroke=true;
    std::array<FavoriteSlot,6> favorites{};
};
State& state();
} // namespace MosuanUI
