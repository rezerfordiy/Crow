cmake_minimum_required(VERSION 3.16)
project(PathPlanner)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Qt6_DIR "/opt/homebrew/opt/qt/lib/cmake/Qt6")
set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt;${CMAKE_PREFIX_PATH}")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find packages
find_package(Qt6 COMPONENTS Widgets Network REQUIRED )
find_package(Boost REQUIRED)



# Source files
set(SOURCES
    AStar.cpp
    Graph.cpp

    main.cpp
    MainWidget.cpp
    MyScene.cpp
    Obstacle.cpp
    SafeZone.cpp
    ScrollableView.cpp
    
    GraphManager.cpp
    NetworkManager.cpp
    SceneData.cpp
    
)

set(HEADERS
    AStar.h
    Graph.h
    MainWidget.h
    MyScene.h
    Obstacle.h
    SafeZone.h
    ScrollableView.h
    Types.h
    
    GraphManager.h
    NetworkManager.h
    SceneData.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Qt6Widgets_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
    Qt6::Widgets
    Qt6::Network
    ${Boost_LIBRARIES}
)

if(APPLE)
    target_link_libraries(${PROJECT_NAME}
        "-framework CoreFoundation"
        "-framework AppKit"
        "-framework Security"
    )
endif()
