cmake_minimum_required(VERSION 3.16)

project(DERDesigner VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(SOURCES
    src/main.cpp
    src/model/BasicElement.cpp
    src/model/BasicElement.h
    src/model/Entity.cpp
    src/model/Entity.h
    src/model/Attribute.cpp
    src/model/Attribute.h
    src/model/AttributeType.h
    src/model/AttributeType.cpp
    src/model/ConnectionPoint.h
    src/model/ConnectionPoint.cpp
    src/model/ConnectionLine.h
    src/model/ConnectionLine.cpp
    src/model/Relationship.h
    src/model/Relationship.cpp
    src/viewer/BezierControlPoint.h
    src/viewer/BezierControlPoint.cpp
    src/viewer/DraggableButton.h
    src/viewer/DraggableButton.cpp
    src/viewer/DiagramView.h
    src/viewer/DiagramView.cpp
    src/viewer/QuickAcessToolBar.cpp
    src/viewer/QuickAcessToolBar.h
    src/viewer/MainWindow.cpp
    src/viewer/MainWindow.h
    src/viewer/ConnectionGraphicsItem.cpp
    src/viewer/ConnectionGraphicsItem.h
    src/viewer/ElementGraphicsItem.cpp
    src/viewer/ElementGraphicsItem.h
    src/viewer/ElementRenderer.h 
    src/viewer/ElementRenderer.cpp
    src/viewer/EntityRenderer.cpp
    src/viewer/EntityRenderer.h
    src/viewer/AttributeRenderer.cpp
    src/viewer/AttributeRenderer.h
    src/viewer/RelationshipRenderer.cpp
    src/viewer/RelationshipRenderer.h
    src/controller/DiagramScene.cpp
    src/controller/DiagramScene.h
    src/controller/PropertyCommand.cpp
    src/controller/PropertyCommand.h
    src/app.rc
    src/recursos.qrc
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE src)
target_link_libraries(${PROJECT_NAME} 
    Qt6::Core
    Qt6::Widgets
)

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Qt6::Core>
        $<TARGET_FILE:Qt6::Widgets>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()