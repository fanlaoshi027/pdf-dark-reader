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

# Keep each responsibility in its own translation unit.
add_executable(PDFDarkReader WIN32
    src/main.cpp
    src/AppWindow.h
    src/AppWindow.cpp
    src/AppWindowPdf.h
    src/AppWindowPdf.cpp
    src/AppWindowInput.h
    src/AppWindowInput.cpp
    src/AppWindowCommands.h
    src/AppWindowPaint.h
    src/AppWindowToolbar.h

    src/InvertSettings.h
    src/InvertSettings.cpp
    src/PdfDocument.h
    src/PdfDocument.cpp
    src/LayerSystem.h
    src/LayerSystem.cpp

    src/MosuanUi.cpp
    src/MosuanUiState.h
    src/MosuanUiState.cpp
    src/MosuanUiPrimitives.h
    src/MosuanUiPrimitives.cpp
    src/MosuanUiTopBar.h
    src/MosuanUiTopBar.cpp
    src/MosuanUiRightRail.h
    src/MosuanUiRightRail.cpp
    src/MosuanUiPanels.h
    src/MosuanUiPanels.cpp
)

target_include_directories(PDFDarkReader PRIVATE "${PDFIUM_INCLUDE_DIR}")
target_compile_definitions(PDFDarkReader PRIVATE UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN _WIN32_WINNT=0x0A00)
target_compile_options(PDFDarkReader PRIVATE /utf-8)
target_link_libraries(PDFDarkReader PRIVATE "${PDFIUM_LIBRARY}" user32 gdi32 shell32 comdlg32)

add_custom_command(TARGET PDFDarkReader POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PDFIUM_DIR}/bin/pdfium.dll"
        "$<TARGET_FILE_DIR:PDFDarkReader>/pdfium.dll")

install(TARGETS PDFDarkReader RUNTIME DESTINATION .)
