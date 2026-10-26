add_executable(app main.c)

target_include_directories(app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(app PRIVATE
    cc_draw_static
    SDL3::SDL3-static
)
