add_executable(example example.c)
target_link_libraries(example ft)

add_executable(permissions permissions.c)
target_link_libraries(permissions ls)
target_link_libraries(permissions ft)

add_executable(symlink symlink.c)
target_link_libraries(symlink ls)
target_link_libraries(symlink ft)
