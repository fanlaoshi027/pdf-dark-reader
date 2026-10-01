#include "PdfPreloadController.h"
#include <algorithm>

PdfPreloadController::PdfPreloadController(PdfRenderController& renderer)
    : renderer_(renderer) {
    Start();
}

PdfPreloadController::~PdfPreloadController() {
    Stop();
}

void PdfPreloadController::Start() {
    if (worker_.joinable()) return;
    stopping_.store(false);
    worker_ = std::thread(&PdfPreloadController::Worker, this);
}

void PdfPreloadController::Stop() {
    stopping_.store(true);
    condition_.notify_all();
    if (worker_.joinable()) worker_.join();
    std::lock_guard lock(mutex_);
    requests_.clear();
}

void PdfPreloadController::Request(int currentPage, int pageCount,
                                   int pixelWidth, int pixelHeight) {
    if (pageCount <= 0 || pixelWidth <= 0 || pixelHeight <= 0) return;
    currentPage = std::clamp(currentPage, 0, pageCount - 1);

    std::lock_guard lock(mutex_);
    requests_.clear();
    requests_.push_back({currentPage, pageCount, pixelWidth, pixelHeight});
    condition_.notify_one();
}

void PdfPreloadController::Worker() {
    while (!stopping_.load()) {
        RequestData request;
        {
            std::unique_lock lock(mutex_);
            condition_.wait(lock, [this] {
                return stopping_.load() || !requests_.empty();
            });
            if (stopping_.load()) return;
            request = requests_.back();
            requests_.clear();
        }

        // Preload the nearest pages first. The current page is rendered by the
        // foreground path; this worker prepares adjacent pages for navigation.
        const int candidates[] = {
            request.currentPage - 1,
            request.currentPage + 1,
            request.currentPage - 2,
            request.currentPage + 2
        };

        for (int page : candidates) {
            if (stopping_.load()) return;
            if (page < 0 || page >= request.pageCount) continue;
            renderer_.RenderPage(page, request.pixelWidth, request.pixelHeight);
        }
    }
}
