# dir : input directory

function(add_git_submodule dir)
    # if git not installed on computer. this function will error out
    # find git in computer. set variable
    find_package(Git REQUIRED)

    # if cmakelist.txt not exist in dir
    #    - recursivley add submodule we listed in gitmodules file
    # ex) find cmake file in external/json & add that as sub_directory
    #     & find very last cmakelist.txt
    if(NOT EXISTS ${dir}/CMakeLists.txt)
        # GIT_EXECUTABLE : set by find_package(GIT REQUIRED)
        execute_process(
            COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive --
                    ${dir}
            # PROJECT_SOURCE_DIR : absolute path to root directory
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
    endif()

    # CMAKE_SOURCE_DIR : absoulte path to directory where cmake is executed
    if(EXISTS ${CMAKE_SOURCE_DIR}/${dir}/CMakeLists.txt)
        message("Adding : ${dir}/CMakeLists.txt")
        add_subdirectory(${CMAKE_SOURCE_DIR}/${dir}/)
    else()
        message("${dir}/CMakeLists.txt does not exist. Cannot Add It")
    endif()

endfunction(add_git_submodule)
