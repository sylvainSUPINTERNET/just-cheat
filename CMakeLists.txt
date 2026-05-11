cmake_minimum_required(VERSION 3.20)
project(JustCheat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# # Executable — WIN32 / WinMain
add_executable(JustCheat WIN32
    src/WinMain.cpp
    # src/window.cpp
)


# Libs Windows nécessaires pour notre overlay
target_link_libraries(JustCheat PRIVATE
    winhttp    # HTTP pour l'API Claude
    comctl32   # contrôles communs (boutons, etc)
    user32     # CreateWindow, SetWindowDisplayAffinity...
    gdi32      # dessin
)

# external dependencies
target_include_directories(JustCheat PRIVATE deps)

target_compile_definitions(JustCheat PRIVATE
    UNICODE
    _UNICODE
)