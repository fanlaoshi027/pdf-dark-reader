#include "PageNavigation.h"

void PageNavigation::Reset(int pageCount, int currentPage) {
    pageCount_ = pageCount > 0 ? pageCount : 0;
    if (pageCount_ == 0) {
        currentPage_ = 0;
        return;
    }
    currentPage_ = currentPage < 0 ? 0 : currentPage;
    if (currentPage_ >= pageCount_) currentPage_ = pageCount_ - 1;
}

bool PageNavigation::Move(int delta) {
    return SetPage(currentPage_ + delta);
}

bool PageNavigation::SetPage(int page) {
    if (pageCount_ <= 0) return false;
    if (page < 0) page = 0;
    if (page >= pageCount_) page = pageCount_ - 1;
    if (page == currentPage_) return false;
    currentPage_ = page;
    return true;
}
