#pragma once

namespace mosuan::ui {

enum class PageFlow { SinglePageHorizontal, ContinuousVertical };

struct PageSettingsState {
    PageFlow flow = PageFlow::SinglePageHorizontal;
    bool showSideBackground = true;
    bool showNotes = true;
};

class PageSettingsModel {
public:
    const PageSettingsState& state() const noexcept { return state_; }
    void setFlow(PageFlow flow) noexcept { state_.flow = flow; }
    void setSideBackground(bool value) noexcept { state_.showSideBackground = value; }
    void setNotesVisible(bool value) noexcept { state_.showNotes = value; }
private:
    PageSettingsState state_{};
};

} // namespace mosuan::ui
