#pragma once

#include "PdfRenderController.h"
#include <atomic>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>

class PdfPreloadController {
public:
    explicit PdfPreloadController(PdfRenderController& renderer);
    ~PdfPreloadController();

    void Start();
    void Stop();
    void Request(int currentPage, int pageCount, int pixelWidth, int pixelHeight);

private:
    struct RequestData {
        int currentPage = 0;
        int pageCount = 0;
        int pixelWidth = 0;
        int pixelHeight = 0;
    };

    void Worker();

    PdfRenderController& renderer_;
    std::thread worker_;
    std::mutex mutex_;
    std::condition_variable condition_;
    std::deque<RequestData> requests_;
    std::atomic<bool> stopping_{false};
};
