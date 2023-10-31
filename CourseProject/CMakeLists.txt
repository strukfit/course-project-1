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

set(VCPKG_PATH "D:/vcpkg")
set(VCPKG_INCLUDE_DIR "${VCPKG_PATH}/installed/${VCPKG_TARGET_TRIPLET}/include")
set(VCPKG_LIBRARY_DIR "${VCPKG_PATH}/installed/${VCPKG_TARGET_TRIPLET}/lib")
set(VCPKG_BINARY_DIR "${VCPKG_PATH}/installed/${VCPKG_TARGET_TRIPLET}/bin")

# Add source to this project's executable.
add_executable (CourseProject WIN32 ${SOURCES}  "src/GUI.cpp" "include/GUI.h" "src/SQLController.cpp" "include/SQLController.h" "src/MainWindow.cpp" "include/MainWindow.h" "src/Exception.cpp" "include/Exception.h")

find_package(wxWidgets REQUIRED gl core base OPTIONAL_COMPONENTS net)
include(${wxWidgets_USE_FILE})
find_package(unofficial-sqlite3 CONFIG REQUIRED)

target_link_libraries(CourseProject PRIVATE unofficial::sqlite3::sqlite3 ${wxWidgets_LIBRARIES})

target_include_directories(CourseProject PRIVATE "include")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CourseProject PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
