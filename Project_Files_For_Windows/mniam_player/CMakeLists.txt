cmake_minimum_required (VERSION 3.8)

project (mniam_player C)

add_compile_options(-Wall -Wextra -pedantic)

add_executable(mniam_player main.c amcom.c)
target_link_libraries(mniam_player Ws2_32.lib)

target_compile_features(mniam_player PUBLIC c_std_11  c_static_assert)