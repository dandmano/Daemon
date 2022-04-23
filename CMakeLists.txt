# CMakeList.txt : CMake project for Daemon, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("Daemon")

# Add source to this project's executable.
add_executable (Daemon "Daemon.c" "Daemon.h")

# TODO: Add tests and install targets if needed.
