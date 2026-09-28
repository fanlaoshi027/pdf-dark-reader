cmake_minimum_required(VERSION 3.25)
project(PDFDarkReader LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT WIN32)
    message(FATAL_ERROR "PDF Dark Reader is currently Windows-only.")
endif()

add_executable(PDFDarkReader WIN32
    src/main.cpp
    src/AppWindow.h
    src/AppWindow.cpp
    src/InvertSettings.h
    src/InvertSettings.cpp
)

target_compile_definitions(PDFDarkReader PRIVATE UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN)

target_link_libraries(PDFDarkReader PRIVATE
    user32
    gdi32
    shell32
    comdlg32
)

install(TARGETS PDFDarkReader RUNTIME DESTINATION .)
