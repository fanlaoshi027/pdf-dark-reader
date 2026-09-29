#include "AppWindowCommands.h"
#include "AppWindow.h"
#include "AppWindowPdf.h"
#include <windows.h>

namespace {
enum : int {
    ID_OPEN = 1001, ID_SAVE = 1011,
    ID_ZOOM_OUT = 1004, ID_ZOOM_IN = 1005, ID_FIT_WIDTH = 1009,
    ID_PAN = 1401,
    ID_TOOL_PEN = 1301, ID_TOOL_RULER = 1402, ID_TOOL_LASSO = 1304,
    ID_TOOL_ERASER = 1303, ID_TOOL_LINE = 1302,
    ID_PEN_BLACK = 1501, ID_PEN_RED = 1502, ID_PEN_BLUE = 1503,
    ID_WIDTH_THIN = 1511, ID_WIDTH_MEDIUM = 1512, ID_WIDTH_THICK = 1513,
    ID_DASH = 1521, ID_ONE_STROKE = 1522,
    ID_SLOT_BASE = 1601, ID_SLOT_SAVE_BASE = 1701
};
}

LRESULT AppWindowCommands::Execute(AppWindow& app, int id) {
    switch (id) {
    case ID_OPEN: AppWindowPdf::Open(app); return 0;
    case ID_SAVE:
        MessageBoxW(app.hwnd(), L"保存功能下一阶段接入。", L"墨算", MB_OK);
        return 0;
    case ID_ZOOM_OUT: AppWindowPdf::Zoom(app, 0.8); return 0;
    case ID_ZOOM_IN: AppWindowPdf::Zoom(app, 1.25); return 0;
    case ID_FIT_WIDTH: AppWindowPdf::FitWidth(app); return 0;
    case ID_PAN: return 0;
    case ID_TOOL_PEN: app.SetActiveTool(MosuanTool::Pen); app.layers().SetTool(MosuanTool::Pen); app.ApplyInkState(); app.Refresh(); return 0;
    case ID_TOOL_RULER: return 0;
    case ID_TOOL_LASSO: app.SetActiveTool(MosuanTool::Lasso); app.layers().SetTool(MosuanTool::Lasso); app.ApplyInkState(); app.Refresh(); return 0;
    case ID_TOOL_ERASER: app.SetActiveTool(MosuanTool::Eraser); app.layers().SetTool(MosuanTool::Eraser); app.ApplyInkState(); app.Refresh(); return 0;
    case ID_TOOL_LINE: app.SetActiveTool(MosuanTool::Line); app.layers().SetTool(MosuanTool::Line); app.ApplyInkState(); app.Refresh(); return 0;
    case ID_PEN_BLACK: app.SetPenColorIndex(0); app.ApplyInkState(); app.Refresh(); return 0;
    case ID_PEN_RED: app.SetPenColorIndex(1); app.ApplyInkState(); app.Refresh(); return 0;
    case ID_PEN_BLUE: app.SetPenColorIndex(2); app.ApplyInkState(); app.Refresh(); return 0;
    case ID_WIDTH_THIN: app.SetPenWidthIndex(0); app.ApplyInkState(); app.Refresh(); return 0;
    case ID_WIDTH_MEDIUM: app.SetPenWidthIndex(1); app.ApplyInkState(); app.Refresh(); return 0;
    case ID_WIDTH_THICK: app.SetPenWidthIndex(2); app.ApplyInkState(); app.Refresh(); return 0;
    case ID_DASH: app.SetDashMode(!app.dashMode()); app.ApplyInkState(); app.Refresh(); return 0;
    case ID_ONE_STROKE: app.SetOneStrokeMode(!app.oneStrokeMode()); app.ApplyInkState(); app.Refresh(); return 0;
    default:
        if (id >= ID_SLOT_BASE && id < ID_SLOT_BASE + static_cast<int>(InkToolState::kSlotCount)) {
            app.LoadInkSlot(static_cast<std::size_t>(id - ID_SLOT_BASE));
            app.Refresh(); return 0;
        }
        if (id >= ID_SLOT_SAVE_BASE && id < ID_SLOT_SAVE_BASE + static_cast<int>(InkToolState::kSlotCount)) {
            app.SaveInkSlot(static_cast<std::size_t>(id - ID_SLOT_SAVE_BASE));
            return 0;
        }
        return 1;
    }
}
