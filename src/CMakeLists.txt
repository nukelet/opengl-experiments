add_executable(main main.cpp)
target_link_libraries(main shader ${ALL_LIBS})

add_subdirectory(utils)