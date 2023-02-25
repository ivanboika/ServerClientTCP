# CMakeList.txt : CMake project for ServerTCP, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable(ServerTCP ServerTCP.h ServerTCP.cpp)
add_executable(ClientTCP ClientTCP.cpp)

target_link_libraries(ServerTCP PRIVATE ws2_32 wsock32)
target_link_libraries(ClientTCP PRIVATE ws2_32 wsock32)
# TODO: Add tests and install targets if needed.
