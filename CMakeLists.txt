cmake_minimum_required(VERSION 3.20)
project(ClothingClient)

# Установка политик CMake
if(POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW) # Для правильного поиска Qt
endif()

find_package(Vulkan QUIET)  # QUIET подавляет ошибки если не найден
if(NOT Vulkan_FOUND)
    message(WARNING "Vulkan not found - skipping Vulkan-related features")
endif()

## Настройка компилятора
#if(WIN32 AND NOT DEFINED ENV{QT_DIR})
#    set(CMAKE_PREFIX_PATH "C:/Qt/6.9.0/mingw_64")
#    set(CMAKE_C_COMPILER "C:/Qt/Tools/mingw1310_64/bin/gcc.exe")
#    set(CMAKE_CXX_COMPILER "C:/Qt/Tools/mingw1310_64/bin/g++.exe")
#endif()

if(NOT DEFINED ENV{QT_DIR})
    set(CMAKE_PREFIX_PATH "C:/Qt/6.9.0/mingw_64")
endif()

# Принудительно используем MinGW (если CMake выбирает MSVC)
set(CMAKE_C_COMPILER "C:/Qt/Tools/mingw1310_64/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "C:/Qt/Tools/mingw1310_64/bin/g++.exe")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

#set(Qt6_ROOT "C:/vcpkg/installed/x64-windows")
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network Test LinguistTools)

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/*.h")

add_executable(${PROJECT_NAME}
        ${SOURCES}
        ${HEADERS}
        src/main.cpp
)

qt_add_resources(RESOURCES resources/resources.qrc)
target_sources(${PROJECT_NAME} PRIVATE ${RESOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Network
)

# Установка путей для include файлов
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Опция для включения тестов
option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    find_package(Qt6 REQUIRED COMPONENTS Test)

    set(TEST_SOURCES
            tests/all_tests.cpp
    )

    set(TEST_APP_SOURCES
            src/ApiClient.cpp
            src/ClothingItemModel.cpp
            src/MainWindow.cpp
            src/RoleSelectionDialog.cpp
            include/ApiClient.h
            include/ClothingItemModel.h
            include/MainWindow.h
            include/RoleSelectionDialog.h
    )

    add_executable(${PROJECT_NAME}Tests
            ${TEST_SOURCES}
            ${TEST_APP_SOURCES}
    )


    target_include_directories(${PROJECT_NAME}Tests PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/tests
    )

    target_link_libraries(${PROJECT_NAME}Tests PRIVATE
            Qt6::Core
            Qt6::Gui
            Qt6::Widgets
            Qt6::Network
            Qt6::Test
    )

    enable_testing()
    add_test(NAME AllTests COMMAND ${PROJECT_NAME}Tests)
endif()

# Установка (опционально)
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
        BUNDLE DESTINATION .
)



## файлы переводов
#qt_add_translations(ClothingClient TS_FILES
#        translations/ru.ts
#)