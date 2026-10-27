add_library(cc_draw_static STATIC draw.c)
add_library(cc_colors INTERFACE)

target_include_directories(cc_draw_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(cc_colors INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(cc_draw_static PUBLIC cc_struct_utils_static cc_h_math_static)
target_link_libraries(cc_draw_static PUBLIC cc_colors)
target_link_libraries(cc_draw_static PRIVATE assimp::assimp stdc++)
