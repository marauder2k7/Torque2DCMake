# JTH: We require CMake 3.1.4 for MSVC14 compatibility check
cmake_minimum_required(VERSION 3.1.4)

include(basics.cmake)

setupVersionNumbers()

# the main engine, should come last
include(torque2d.cmake)

#setupPackaging()