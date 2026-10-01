#pragma once

#include <windows.h>
#include <memory>
#include <unordered_map>

class PdfPageCache {
public:
    void Clear();
    void Remove(int pageIndex);
    bool Contains(int pageIndex) const;
    HBITMAP Get(int pageIndex) const;
    void Put(int pageIndex, HBITMAP bitmap);

private:
    std::unordered_map<int, HBITMAP> cache_;
};
