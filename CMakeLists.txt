#cmake - wybór elementów do kompilacji
cmake_minimum_required (VERSION 3.8)

project ("Daemon")
set (CMAKE_CXX_STANDARD 17)

add_executable (Daemon "Daemon.c" "Headers.h")

