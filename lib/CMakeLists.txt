set(NAME ls)

add_library(${NAME} STATIC
	parse_args.c
	utils.c
	partition.c
	"file.c"
	process_files.c
)

target_include_directories(${NAME} PUBLIC ../include)
target_link_libraries(${NAME} PUBLIC ft)
target_compile_options(${NAME} PRIVATE -Wall -Wextra -Werror)
target_compile_options(${NAME} PRIVATE -Wno-unused -Wno-unused-parameter)


if(DEFINED TestBuild)
	target_compile_definitions(${NAME} PRIVATE TEST_BUILD=1)
endif()
