# CMakeList.txt : CMake project for CourseProject, include source and define
# project specific logic here.
#

file(GLOB_RECURSE SOURCES "src/*.cpp"  "include/*.h")

if(WIN32)
    execute_process(COMMAND cmd /c echo %USERPROFILE% OUTPUT_VARIABLE USER_PROFILE)
    string(STRIP "${USER_PROFILE}" USER_PROFILE)
else()
    message(FATAL_ERROR "This script is designed for Windows only.")
endif()

# Add source to this project's executable.
add_executable (CourseProject WIN32 ${SOURCES}  "src/GUI.cpp" "include/GUI.h")

find_package(wxWidgets REQUIRED gl core base OPTIONAL_COMPONENTS net)
include(${wxWidgets_USE_FILE})
# and for each of your dependent executable/library targets:
target_link_libraries(CourseProject ${wxWidgets_LIBRARIES})

target_include_directories(CourseProject PRIVATE "include")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CourseProject PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
