add_executable(sdl-render-ctree
    main.c)
target_link_libraries(sdl-render-ctree ctree SDL2 SDL2_gfx SDL2_ttf)

add_custom_command(
    TARGET sdl-render-ctree POST_BUILD
    COMMAND ${CMAKE_COMMAND}
        -E copy ${CMAKE_CURRENT_SOURCE_DIR}/DejaVuSansMono.ttf ${PROJECT_BINARY_DIR})
