# Generate project
cmake -B build -G "MinGW Makefiles"   # MinGW
cmake -B build                           # Visual Studio (auto-détecté)

# Compile
cmake --build build ;; .\build\Debug\JustCheat.exe