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

# Указываем переменную с путем к иконке
set(ICON_PATH ${CMAKE_SOURCE_DIR}/img/icon.ico)

# Add source to this project's executable.
add_executable (HouseholdAppliancesStore WIN32 ${SOURCES} "src/GUI.cpp" "include/GUI.h" "src/SQLController.cpp" "include/SQLController.h" "src/MainWindow.cpp" "include/MainWindow.h" "src/Exception.cpp" "include/Exception.h")

# Указываем свойства для добавления иконки
set_target_properties(HouseholdAppliancesStore PROPERTIES WIN32_EXECUTABLE TRUE)
set_target_properties(HouseholdAppliancesStore PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
#set_target_properties(HouseholdAppliancesStore PROPERTIES LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\"")

# Устанавливаем ссылку на иконку
set(RESOURCES ${CMAKE_SOURCE_DIR}/resources.rc)
file(WRITE ${RESOURCES} "IDI_ICON1 ICON DISCARDABLE \"${ICON_PATH}\"\n")

# Указываем иконку в проекте
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
