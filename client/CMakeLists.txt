file(GLOB_RECURSE SOURCE_FILES *.cpp)

add_executable (client ${SOURCE_FILES})

target_include_directories(client PRIVATE include)
target_link_libraries(client ${Boost_LIBRARIES})
