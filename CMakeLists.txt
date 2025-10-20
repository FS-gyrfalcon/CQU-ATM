cmake_minimum_required(VERSION 3.11)
project(ATM)

set(CMAKE_CXX_STANDARD 17)

# 使用FetchContent引入FTXUI
include(FetchContent)
FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
    GIT_TAG v5.0.0
)
FetchContent_MakeAvailable(ftxui)

# 添加可执行文件
add_executable(atm_with_ftxui 
    main.cpp
    simple_json.cpp
    atm_ui.cpp
)

# 链接FTXUI库
target_link_libraries(atm_with_ftxui 
    PRIVATE ftxui::screen 
    PRIVATE ftxui::dom 
    PRIVATE ftxui::component
)