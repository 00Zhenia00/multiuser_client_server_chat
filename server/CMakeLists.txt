add_executable (server  "server.cpp"
                        "Server.cpp"
)

target_link_libraries(server ${Boost_LIBRARIES})