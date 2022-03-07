add_library(shader shader.cpp)
target_include_directories(shader PRIVATE ${PROJECT_INCLUDE_DIR}/include/utils/shader.hpp)
target_link_libraries(shader ${ALL_LIBS})