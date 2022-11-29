set(NAME ft_ls)
add_executable(${NAME}
	main.c
)

target_link_libraries(${NAME} PUBLIC ls)

if(DEFINED TestBuild)
	target_compile_definitions(${NAME} PRIVATE TEST_BUILD=1)
endif()
