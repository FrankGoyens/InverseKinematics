cmake_minimum_required(VERSION 3.11)

project(InverseKinematics)

set(CMAKE_CXX_STANDARD 17)

set(InverseKinematics_Headers
	include/CJoint.h
	include/CLink.h
	include/CSkeleton.h
	include/JacobianElement.h
	include/SkeletonRenderer.h
	include/SkeletonPicker.h
	MinimalOgre.h
)

set(InverseKinematics_Sources
	main.cpp
	OgreSkeletonRenderer.cpp
	OgreSkeletonPicker.cpp
	CJoint.cpp
	CLink.cpp
	CSkeleton.cpp
	JacobianElement.cpp
	MinimalOgre.cpp
)

add_executable(InverseKinematics ${InverseKinematics_Headers} ${InverseKinematics_Sources})

find_package(OGRE CONFIG REQUIRED)

target_include_directories(InverseKinematics PUBLIC include
	PRIVATE glm)

target_link_libraries(InverseKinematics OgreHLMS OgreMain OgreBites)
