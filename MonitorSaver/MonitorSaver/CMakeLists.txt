project(MonitorSaver)

# Find Qt6 Widgets component
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Glob all header, source, UI files
file(GLOB_RECURSE SAVER_HEADERS include/*.h)
file(GLOB_RECURSE SAVER_SOURCE src/*.cpp)
file(GLOB_RECURSE SAVER_RESOURCES ui/*.ui)

# Add resources
qt_add_resources(RESOURCES resources.qrc)

# Add the executable
add_executable(MonitorSaver WIN32 ${SAVER_HEADERS} ${SAVER_SOURCE} ${SAVER_RESOURCES} ${RESOURCES} src/main.cpp
    src/backEnd.cpp
    include/backend.h
    include/MonitorConfig.h
)

# Include directories
target_include_directories(MonitorSaver PRIVATE include ${CMAKE_BINARY_DIR})

# Add a custom command to copy the save file
add_custom_command(TARGET MonitorSaver POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E make_directory
                   $<TARGET_FILE_DIR:MonitorSaver>/save
                   COMMAND ${CMAKE_COMMAND} -E copy_if_different
                   ${CMAKE_SOURCE_DIR}/MonitorSaver/save/save.txt
                   $<TARGET_FILE_DIR:MonitorSaver>/save/save.txt
               )

# Define a preprocessor macro SAVE_FILE_PATH with the path to the file
target_compile_definitions(MonitorSaver PRIVATE SAVE_FILE_PATH="save/save.txt")

# Link the Qt6 Widgets library
target_link_libraries(MonitorSaver PRIVATE Qt6::Widgets)

# Set target properties
set_target_properties(MonitorSaver PROPERTIES
                      AUTOMOC ON
                      AUTOUIC ON
                      AUTORCC ON
                      AUTOUIC_SEARCH_PATHS ui)

# Deploy Qt dependencies on Windows
if(WIN32)
    add_custom_command(TARGET MonitorSaver POST_BUILD
                       COMMAND $<TARGET_FILE_DIR:Qt6::qmake>/windeployqt $<TARGET_FILE:MonitorSaver>)
endif(WIN32)
