set(NAME ft_ls)
add_executable(${NAME}
main.c
)

target_link_libraries(${NAME} PUBLIC ls)
