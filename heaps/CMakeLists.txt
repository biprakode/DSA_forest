cmake_minimum_required(VERSION 3.10) # Set the minimum required CMake version.
project(generic_pq C)

# Add the source files and create an executable.
add_executable(gen_heap heap.c  user.c)