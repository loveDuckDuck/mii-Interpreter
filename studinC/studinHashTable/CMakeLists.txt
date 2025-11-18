cmake_minimum_required(VERSION 3.10)
project(studinHashTable C)

set(CMAKE_C_STANDARD 99)

add_executable(studinHashTable
    main.c
    hash_table.c
    prime.c
)

# Link math library for pow, sqrt, etc.
target_link_libraries(studinHashTable m)