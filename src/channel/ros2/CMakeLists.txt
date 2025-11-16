set(TARGET_NAME channel_ros2)

file(GLOB_RECURSE SOURCE_FILE  CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR} *.cpp *.cc)
file(GLOB_RECURSE INCLUDE_FILE CONFIGURE_DEPENDS  ${CMAKE_SOURCE_DIR}  *.h *.hpp)
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_kdl REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(Boost REQUIRED COMPONENTS thread)

# set(sensor_DIR "/home/xyc/fishros/install/sensor/share/sensor/cmake")
# find_package(sensor REQUIRED)

add_library(${TARGET_NAME} SHARED
    ${SOURCE_FILE} 
    ${INCLUDE_FILE}
)

# include_directories(
#    /home/xyc/fishros/install/sensor/include
# )

include_directories(
    /usr/include/x86_64-linux-gnu/qt5/QtGui
)  # 替换为你的实际路径

target_include_directories(
${TARGET_NAME} 
PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR}
)

ament_target_dependencies(${TARGET_NAME} PUBLIC rclcpp  Boost nav_msgs tf2 tf2_geometry_msgs  tf2_ros sensor_msgs tf2_kdl cv_bridge )

target_link_libraries(${TARGET_NAME}
  PUBLIC
  basic
  common
  # /home/xyc/fishros/install/sensor/lib/libsensor__rosidl_typesupport_cpp.so
  # /home/xyc/fishros/install/sensor/lib/libsensor__rosidl_typesupport_fastrtps_cpp.so
  # /home/xyc/fishros/install/sensor/lib/libsensor__rosidl_typesupport_introspection_cpp.so
)

# set_target_properties(${TARGET_NAME} PROPERTIES
#   BUILD_WITH_INSTALL_RPATH TRUE
#   INSTALL_RPATH "/home/xyc/fishros/install/sensor/lib"
# )


set_target_properties(${TARGET_NAME} PROPERTIES
RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)
install(TARGETS ${TARGET_NAME} DESTINATION lib/${PROJECT_NAME})
install(TARGETS ${TARGET_NAME}
    RUNTIME DESTINATION bin/lib
    LIBRARY DESTINATION bin/lib
    ARCHIVE DESTINATION bin/lib
)
# ament_package()
# install(TARGETS ${TARGET_NAME} DESTINATION lib/${PROJECT_NAME})
