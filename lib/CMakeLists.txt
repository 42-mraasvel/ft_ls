set(NAME ls)

add_library(${NAME} STATIC
parse_args.c
utils.c
)

target_include_directories(${NAME} PUBLIC ../include)
target_link_libraries(${NAME} PUBLIC ft)
target_compile_options(${NAME} PRIVATE -Wall -Wextra -Werror)
