# Dependencies

The C++ backend uses only the C++17 standard library plus platform sockets (Winsock on Windows, BSD sockets on macOS/Linux). CMake is used for builds. The frontend is plain HTML/CSS/JavaScript and uses no downloaded runtime libraries or network requests.

Build-time requirements: CMake 3.16+ and a C++17 compiler. These are not bundled; therefore the current Windows launcher is a build-and-run script, not the assignment's dependency-free prebuilt `Launch.exe` package.
