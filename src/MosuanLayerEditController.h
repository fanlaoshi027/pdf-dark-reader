#pragma once
#include <cstddef>
#include <string>

class AppWindow;

class MosuanLayerEditController {
public:
    void BeginRename(std::size_t index);
    void SetRenameText(const std::wstring& text);
    bool CommitRename(AppWindow& app);
    void CancelRename();

    void BeginDrag(std::size_t index, int y);
    bool UpdateDrag(int y);
    bool EndDrag(AppWindow& app);

    bool Renaming() const { return renaming_; }
    bool Dragging() const { return dragging_; }
    std::size_t Index() const { return index_; }

private:
    bool renaming_ = false;
    bool dragging_ = false;
    std::size_t index_ = 0;
    int startY_ = 0;
    int currentY_ = 0;
    std::wstring renameText_;
};
