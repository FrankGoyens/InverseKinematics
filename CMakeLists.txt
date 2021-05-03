project(InverseKinematics)

set(InverseKinematics_Headers
	#include/CJoint.h
	#include/CLink.h
	#include/CSkeleton.h
	#include/Camera.h
	#include/ControlPointSphere.h
	#include/DisplayList.h
	#include/Drawable.h
	#include/JacobianElement.h
	#include/MainWindow.h
	#include/Movable.h
	#include/MoveHandler.h
	#include/Pickable.h
	#include/Picking.h
	#include/Renderer.h
	
	OgreMainWindow.h
)

set(InverseKinematics_Sources
	main.cpp
	#CJoint.cpp
	#CLink.cpp
	#CSkeleton.cpp
	#Camera.cpp
	#ControlPointSphere.cpp
	#DisplayList.cpp
	#Drawable.cpp
	#JacobianElement.cpp
	#MainWindow.cpp
	#MoveHandler.cpp
	#Pickable.cpp
	#Picking.cpp
	#Renderer.cpp
	OgreMainWindow.cpp
)

add_executable(InverseKinematics ${InverseKinematics_Headers} ${InverseKinematics_Sources})

find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(OGRE CONFIG REQUIRED)
find_package(CEGUI REQUIRED COMPONENTS Ogre)

target_include_directories(InverseKinematics PUBLIC include
	PRIVATE glm ${CEGUI_INCLUDE_DIRS} ${CEGUI_OgreRenderer_INCLUDE_DIR})

target_link_libraries(InverseKinematics OgreHLMS OgreMain OgreBites ${CEGUI_OgreRenderer_LIBRARIES} ${CEGUI_LIBRARIES})

install(TARGETS InverseKinematics RUNTIME DESTINATION bin)
