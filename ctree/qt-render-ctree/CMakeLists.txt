add_executable(qt-render-ctree
    dialog.cpp
    main.cpp)
target_link_libraries(qt-render-ctree ctree Qt5::Widgets)
