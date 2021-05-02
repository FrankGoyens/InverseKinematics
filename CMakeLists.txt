project(InverseKinematics)

set(InverseKinematics_Headers
	include/CJoint.h
	include/CLink.h
	include/CSkeleton.h
	include/Camera.h
	include/ControlPointSphere.h
	include/DisplayList.h
	include/Drawable.h
	include/JacobianElement.h
	include/MainWindow.h
	include/Movable.h
	include/MoveHandler.h
	include/Pickable.h
	include/Picking.h
)

set(InverseKinematics_Sources
	main.cpp
	CJoint.cpp
	CLink.cpp
	CSkeleton.cpp
	Camera.cpp
	ControlPointSphere.cpp
	DisplayList.cpp
	Drawable.cpp
	JacobianElement.cpp
	MainWindow.cpp
	MoveHandler.cpp
	Pickable.cpp
	Picking.cpp
)

add_executable(InverseKinematics ${InverseKinematics_Headers} ${InverseKinematics_Sources})

find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)

target_include_directories(InverseKinematics PUBLIC include
	PRIVATE glm)

target_link_libraries(InverseKinematics glfw opengl32 GLU32 GLEW::GLEW)

install(TARGETS InverseKinematics RUNTIME DESTINATION bin)
