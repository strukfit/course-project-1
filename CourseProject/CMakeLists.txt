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

# Specify a variable with the path to the icon
set(ICON_PATH ${CMAKE_SOURCE_DIR}/img/icon.ico)

# Add source to this project's executable.
add_executable (HouseholdAppliancesStore WIN32 ${SOURCES} "src/GUI.cpp" "include/GUI.h" "src/SQLController.cpp" "include/SQLController.h" "src/MainWindow.cpp" "include/MainWindow.h" "src/Exception.cpp" "include/Exception.h")

# Specify properties for adding an icon
set_target_properties(HouseholdAppliancesStore PROPERTIES WIN32_EXECUTABLE TRUE)
set_target_properties(HouseholdAppliancesStore PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
#set_target_properties(HouseholdAppliancesStore PROPERTIES LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\"")

# Set a link to the icon
set(RESOURCES ${CMAKE_SOURCE_DIR}/resources.rc)
file(WRITE ${RESOURCES} "IDI_ICON1 ICON DISCARDABLE \"${ICON_PATH}\"\n")

# Specify the icon in the project
target_sources(HouseholdAppliancesStore PRIVATE ${RESOURCES})

find_package(wxWidgets REQUIRED gl core base OPTIONAL_COMPONENTS net)
include(${wxWidgets_USE_FILE})
find_package(unofficial-sqlite3 CONFIG REQUIRED)

target_link_libraries(HouseholdAppliancesStore PRIVATE unofficial::sqlite3::sqlite3 ${wxWidgets_LIBRARIES})

target_include_directories(HouseholdAppliancesStore PRIVATE "include")

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/img)
set(DESTINATION_DIR ${CMAKE_BINARY_DIR}/CourseProject)
file(COPY ${SOURCE_DIR} DESTINATION ${DESTINATION_DIR})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET HouseholdAppliancesStore PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.

# Project version
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")

# Determine the package type (in this case, NSIS - for Windows)
set(CPACK_GENERATOR NSIS)

# Set up package meta information
set(CPACK_PACKAGE_NAME "HouseholdAppliancesStore")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A software product for working with a file database of household appliances store products.")
set(CPACK_PACKAGE_VENDOR "Borysenko Rostyslav Vitaliyovych")

# Specify which files to include in the package
install(TARGETS HouseholdAppliancesStore DESTINATION .)
install(DIRECTORY ${SOURCE_DIR} DESTINATION .)
file(GLOB DLL_FILES ${DESTINATION_DIR}/*.dll)
install(FILES ${DLL_FILES} DESTINATION .)

# Enable CPack after all settings
include(CPack)