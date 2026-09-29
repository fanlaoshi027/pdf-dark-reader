#pragma once

namespace mosuan::ui {

enum class Tool { Pen, Ruler, Lasso, Eraser, Line };
enum class Color { Black, Red, Blue };
enum class Width { Thin, Medium, Thick };

struct ToolbarState {
    Tool tool = Tool::Pen;
    Color color = Color::Black;
    Width width = Width::Medium;
    bool dashed = false;
    bool oneStroke = true;
    bool pan = false;
};

class ToolbarModel {
public:
    const ToolbarState& state() const noexcept { return state_; }
    void setTool(Tool value) noexcept { state_.tool = value; state_.pan = false; }
    void setColor(Color value) noexcept { state_.color = value; }
    void setWidth(Width value) noexcept { state_.width = value; }
    void toggleDashed() noexcept { state_.dashed = !state_.dashed; }
    void toggleOneStroke() noexcept { state_.oneStroke = !state_.oneStroke; }
    void setPan(bool value) noexcept { state_.pan = value; }
private:
    ToolbarState state_{};
};

} // namespace mosuan::ui
