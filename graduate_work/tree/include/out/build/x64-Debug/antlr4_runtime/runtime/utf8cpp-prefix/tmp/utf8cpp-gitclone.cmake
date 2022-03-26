
if(NOT "C:/Users/jonid/source/repos/StasLuts/praktikum/graduate_work/tree/include/out/build/x64-Debug/antlr4_runtime/runtime/utf8cpp-prefix/src/utf8cpp-stamp/utf8cpp-gitinfo.txt" IS_NEWER_THAN "C:/Users/jonid/source/repos/StasLuts/praktikum/graduate_work/tree/include/out/build/x64-Debug/antlr4_runtime/runtime/utf8cpp-prefix/src/utf8cpp-stamp/utf8cpp-gitclone-lastrun.txt")
  message(STATUS "Avoiding repeated git clone, stamp file is up to date: 'C:/Users/jonid/source/repos/StasLuts/praktikum/graduate_work/tree/include/out/build/x64-Debug/antlr4_runtime/runtime/utf8cpp-prefix/src/utf8cpp-stamp/utf8cpp-gitclone-lastrun.txt'")
  return()
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} -E rm -rf "C:/Users/jonid/source/repos/StasLuts/praktikum/graduate_work/tree/include/out/build/x64-Debug/runtime/thirdparty/utfcpp"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to remove directory: 'C:/Users/jonid/source/repos/StasLuts/praktikum/graduate_work/tree/include/out/build/x64-Debug/runtime/thirdparty/utfcpp'")
endif()

# try the clone 3 times in case there is an odd git clone issue
set(error_code 1)
set(number_of_tries 0)
while(error_code AND number_of_tries LESS 3)
  execute_process(
    COMMAND "C:/Program Files/Git/cmd/git.exe"  clone --no-checkout --config "advice.detachedHead=false" "https://github.com/nemtrif/utfcpp" "utfcpp"
    WORKING_DIRECTORY "C:/Users/jonid/source/repos/StasLuts/praktikum/graduate_work/tree/include/out/build/x64-Debug/runtime/thirdparty"
    RESULT_VARIABLE error_code
    )
  math(EXPR number_of_tries "${number_of_tries} + 1")
endwhile()
if(number_of_tries GREATER 1)
  message(STATUS "Had to git clone more than once:
          ${number_of_tries} times.")
endif()
if(error_code)
  message(FATAL_ERROR "Failed to clone repository: 'https://github.com/nemtrif/utfcpp'")
endif()

execute_process(
  COMMAND "C:/Program Files/Git/cmd/git.exe"  checkout v3.1.1 --
  WORKING_DIRECTORY "C:/Users/jonid/source/repos/StasLuts/praktikum/graduate_work/tree/include/out/build/x64-Debug/runtime/thirdparty/utfcpp"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to checkout tag: 'v3.1.1'")
endif()

set(init_submodules TRUE)
if(init_submodules)
  execute_process(
    COMMAND "C:/Program Files/Git/cmd/git.exe"  submodule update --recursive --init 
    WORKING_DIRECTORY "C:/Users/jonid/source/repos/StasLuts/praktikum/graduate_work/tree/include/out/build/x64-Debug/runtime/thirdparty/utfcpp"
    RESULT_VARIABLE error_code
    )
endif()
if(error_code)
  message(FATAL_ERROR "Failed to update submodules in: 'C:/Users/jonid/source/repos/StasLuts/praktikum/graduate_work/tree/include/out/build/x64-Debug/runtime/thirdparty/utfcpp'")
endif()

# Complete success, update the script-last-run stamp file:
#
execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy
    "C:/Users/jonid/source/repos/StasLuts/praktikum/graduate_work/tree/include/out/build/x64-Debug/antlr4_runtime/runtime/utf8cpp-prefix/src/utf8cpp-stamp/utf8cpp-gitinfo.txt"
    "C:/Users/jonid/source/repos/StasLuts/praktikum/graduate_work/tree/include/out/build/x64-Debug/antlr4_runtime/runtime/utf8cpp-prefix/src/utf8cpp-stamp/utf8cpp-gitclone-lastrun.txt"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to copy script-last-run stamp file: 'C:/Users/jonid/source/repos/StasLuts/praktikum/graduate_work/tree/include/out/build/x64-Debug/antlr4_runtime/runtime/utf8cpp-prefix/src/utf8cpp-stamp/utf8cpp-gitclone-lastrun.txt'")
endif()

