cmake_minimum_required(VERSION 3.14)
project("ls" C)
set(CMAKE_VErBOSE_MAKEFILE ON)

# add_compile_options(-fsanitize=address)
# add_link_options(-fsanitize=address)

add_subdirectory(libft)
add_subdirectory(examples)
add_subdirectory(lib)
add_subdirectory(src)
# add_subdirectory(test)
