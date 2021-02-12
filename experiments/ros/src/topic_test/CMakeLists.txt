cmake_minimum_required(VERSION 3.0.2)
project(topic_test)

## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
)

catkin_package()

include_directories(include ${catkin_INCLUDE_DIRS})

add_executable(pub src/publisher.cpp)
target_link_libraries(pub ${catkin_LIBRARIES})

add_executable(sub src/subscriber.cpp)
target_link_libraries(sub ${catkin_LIBRARIES})