set(NAME test)
add_executable(${NAME}
	test_parse_args.c
)

target_link_libraries(${NAME} PUBLIC criterion)
target_link_libraries(${NAME} PUBLIC ls)
