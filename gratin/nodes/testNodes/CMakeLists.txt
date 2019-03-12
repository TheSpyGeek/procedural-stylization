# required - minimum cmake version
cmake_minimum_required(VERSION 2.8)

# set the path to the gratin folder here
#set(PATH_TO_GRATIN ${CMAKE_SOURCE_DIR}/../../../../gratin/trunk/build)
#set(PATH_TO_GRATIN ${CMAKE_SOURCE_DIR}/../../../../projects/gratin-dev/trunk/build/)
set(PATH_TO_GRATIN /home/misnel/gratin/build)

# set destination in which plugins will be located
set(DESTINATION_DIR ${CMAKE_SOURCE_DIR}/../lib-nodes)

# include default modules and compiler options
include(${PATH_TO_GRATIN}/../cmake/gratin.conf.in)

# find and add default packages and directories (called only once)
find_default_packages()
add_default_paths()

# convenient function that adds all the subdirectories containing CMakeLists.txt
add_sub_dirs(${CMAKE_CURRENT_SOURCE_DIR})
