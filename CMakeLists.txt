cmake_minimum_required(VERSION 3.24)

project(cpplox)

set(CMAKE_CXX_STANDARD 11)

file(GLOB TREE_WALK_SRC "tree-walk/*.h" "tree-walk/*.cpp")

add_executable(tree-walk ${TREE_WALK_SRC})

file(GLOB VM_SRC "vm/*.h" "vm/*.cpp")

add_executable(vm ${VM_SRC})