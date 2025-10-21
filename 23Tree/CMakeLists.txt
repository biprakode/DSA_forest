cmake_minimum_required(VERSION 3.10) # Set the minimum required CMake version.
project(generic_23Tree C)

# Add the source files and create an executable.
add_executable(gen_23tree tree.c  main.c)

# Link the 'm' library to your executable (for math)
target_link_libraries(gen_23tree PRIVATE m)