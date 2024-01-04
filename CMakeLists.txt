cmake_minimum_required(VERSION 3.16)

# Boost
set(BOOST_ENABLE_CMAKE ON)
find_package(Boost COMPONENTS system asio thread)
include_directories(${Boost_INCLUDE_DIR})

add_subdirectory(client)
add_subdirectory(server)