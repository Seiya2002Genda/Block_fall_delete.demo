cmake_minimum_required(VERSION 3.10)
project(tetris_gui)

set(CMAKE_CXX_STANDARD 17)

# インクルードパスとライブラリパスを手動指定
include_directories("/opt/homebrew/Cellar/sfml/3.0.0_1/include")
link_directories("/opt/homebrew/Cellar/sfml/3.0.0_1/lib")

# SFMLを探す
set(SFML_DIR "/opt/homebrew/Cellar/sfml/3.0.0_1/lib/cmake/SFML")
find_package(SFML 2 REQUIRED COMPONENTS graphics window system)

# 実行ファイルを作成
add_executable(untitled main.cpp)

# SFMLライブラリとリンク
target_link_libraries(untitled sfml-graphics sfml-window sfml-system)
