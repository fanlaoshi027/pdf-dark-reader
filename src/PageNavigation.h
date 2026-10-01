#pragma once

class PageNavigation {
public:
    void Reset(int pageCount, int currentPage = 0);
    bool Move(int delta);
    bool SetPage(int page);

    int CurrentPage() const { return currentPage_; }
    int PageCount() const { return pageCount_; }

private:
    int pageCount_ = 0;
    int currentPage_ = 0;
};
