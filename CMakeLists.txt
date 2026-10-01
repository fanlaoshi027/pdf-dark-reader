cmake_minimum_required(VERSION 3.25)
project(PDFDarkReader LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(NOT WIN32)
    message(FATAL_ERROR "PDF Dark Reader is currently Windows-only.")
endif()
set(PDFIUM_DIR "" CACHE PATH "PDFium package directory")
if(NOT PDFIUM_DIR)
    message(FATAL_ERROR "PDFIUM_DIR must point to a PDFium package containing include/, lib/, and bin/.")
endif()
find_path(PDFIUM_INCLUDE_DIR fpdfview.h PATHS "${PDFIUM_DIR}/include" NO_DEFAULT_PATH REQUIRED)
find_library(PDFIUM_LIBRARY NAMES pdfium pdfium.dll PATHS "${PDFIUM_DIR}/lib" NO_DEFAULT_PATH REQUIRED)
set(PDF_DARK_READER_SOURCES
    src/main.cpp src/AppWindow.h src/AppWindow.cpp
    src/InvertSettings.h src/InvertSettings.cpp
    src/PdfDocument.h src/PdfDocument.cpp src/PdfRenderSettings.h
    src/PdfPageCache.h src/PdfPageCache.cpp
    src/PdfRenderController.h src/PdfRenderController.cpp
    src/PdfPreloadController.h src/PdfPreloadController.cpp src/PdfCachePolicy.h
    src/PdfViewState.h src/PdfViewStateController.h src/PdfViewStateController.cpp
    src/PdfViewRenderCoordinator.h src/PdfViewRenderCoordinator.cpp
    src/PdfViewController.h src/PdfViewController.cpp
    src/PdfViewStateBridge.h src/PdfViewStateBridge.cpp
    src/DocumentViewport.h src/DocumentViewport.cpp
    src/DocumentViewportAdapter.h src/DocumentViewportAdapter.cpp
    src/VectorStroke.h src/VectorStrokeBuffer.h src/VectorStrokeBuffer.cpp
    src/InkSample.h src/InkSampleBuffer.h src/InkSampleBuffer.cpp
    src/InkInput.h src/InkInput.cpp src/InkPressure.h
    src/InkSmoother.h src/InkSmoother.cpp src/InkTaper.h src/InkTaper.cpp
    src/InkStrokeBuilder.h src/InkStrokeBuilder.cpp src/InkPipeline.h src/InkPipeline.cpp
    src/InkRenderPoint.h src/InkStrokeGeometry.h src/InkStrokeGeometry.cpp
    src/InkStrokeTessellator.h src/InkStrokeTessellator.cpp
    src/InkStrokeCap.h src/InkStrokeCap.cpp src/InkStrokeJoin.h src/InkStrokeJoin.cpp
    src/InkVectorGeometry.h src/InkVectorGeometry.cpp src/InkVectorRenderer.h
    src/InkRenderController.h src/InkRenderController.cpp
    src/InkStrokeSession.h src/InkStrokeSession.cpp
    src/InkInputAdapter.h src/InkInputAdapter.cpp
    src/InkStrokeStore.h src/InkStrokeStore.cpp
    src/InkPointerRouter.h src/InkPointerRouter.cpp
    src/InkPointerSampleExtractor.h src/InkPointerSampleExtractor.cpp
    src/InkHistorySampler.h src/InkHistorySampler.cpp
    src/InkPresenterBridge.h src/InkPresenterBridge.cpp
    src/InkRealtimeBuffer.h src/InkRealtimeBuffer.cpp src/InkRealtimeStroke.h src/InkRealtimeStroke.cpp
    src/InkRenderSnapshot.h src/InkRenderSnapshot.cpp
    src/InkStrokeRenderer.h src/InkStrokeRenderer.cpp
    src/InkStrokeLayer.h src/InkStrokeLayer.cpp
    src/InkStrokeRasterizer.h src/InkStrokeRasterizer.cpp
    src/InkStrokeHitTest.h src/InkStrokeHitTest.cpp
    src/InkStrokeSelection.h src/InkStrokeSelection.cpp
    src/InkStrokeEraser.h src/InkStrokeEraser.cpp
    src/InkStrokeHistory.h src/InkStrokeHistory.cpp
    src/InkStrokeHistoryController.h src/InkStrokeHistoryController.cpp
    src/InkStrokeTransform.h src/InkStrokeTransform.cpp
    src/InkStrokeTransformController.h src/InkStrokeTransformController.cpp
    src/InkCoordinateSpace.h src/InkCoordinateTransform.h src/InkCoordinateTransform.cpp
    src/InkPageAnchor.h src/InkPageAnchor.cpp
    src/InkPageCoordinateMapper.h src/InkPageCoordinateMapper.cpp
    src/DocumentInkBinding.h src/DocumentInkBinding.cpp
    src/DocumentInkViewportSync.h src/DocumentInkViewportSync.cpp
    src/InkLatencyProfile.h src/InkLatencyProfile.cpp
    src/InkPrediction.h src/InkPrediction.cpp src/InkPredictionFilter.h src/InkPredictionFilter.cpp
    src/InkVelocityEstimator.h src/InkVelocityEstimator.cpp
    src/InkAdaptiveSmoothing.h src/InkAdaptiveSmoothing.cpp
    src/WindowsInkPointerAdapter.h src/WindowsInkPointerAdapter.cpp
    src/WindowsInkPointerRouter.h src/WindowsInkPointerRouter.cpp
    src/WindowsInkPointerCapture.h src/WindowsInkPointerCapture.cpp
    src/WindowsInkPenInfo.h src/WindowsInkPenInfo.cpp
    src/WindowsInkHistoryReader.h src/WindowsInkHistoryReader.cpp
    src/WindowsInkInputSession.h src/WindowsInkInputSession.cpp
    src/WindowsInkHistoryAdapter.h src/WindowsInkHistoryAdapter.cpp
    src/WindowsInkVectorBridge.h src/WindowsInkVectorBridge.cpp
    src/LayerSystem.h src/LayerSystem.cpp
    src/WindowsInkHost.h src/WindowsInkHost.cpp
    src/ViewTransform.h src/ViewTransformState.h src/ViewTransformMath.h
    src/ViewTransformController.h src/ViewTransformController.cpp
    src/ZoomController.h src/ZoomController.cpp src/PanController.h src/PanController.cpp
    src/PageNavigation.h src/PageNavigation.cpp src/PdfViewport.h src/PdfViewport.cpp
    src/MosuanUi.cpp src/FavoriteBar.cpp src/ToolbarSkin.cpp src/TopToolbar.cpp
)
add_executable(PDFDarkReader WIN32 ${PDF_DARK_READER_SOURCES})
target_include_directories(PDFDarkReader PRIVATE "${PDFIUM_INCLUDE_DIR}")
target_compile_definitions(PDFDarkReader PRIVATE UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN _WIN32_WINNT=0x0A00)
target_compile_options(PDFDarkReader PRIVATE /utf-8)
target_link_libraries(PDFDarkReader PRIVATE "${PDFIUM_LIBRARY}" user32 gdi32 shell32 comdlg32 d3d11 dxgi dcomp)
add_custom_command(TARGET PDFDarkReader POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PDFIUM_DIR}/bin/pdfium.dll" "$<TARGET_FILE_DIR:PDFDarkReader>/pdfium.dll")
install(TARGETS PDFDarkReader RUNTIME DESTINATION .)
