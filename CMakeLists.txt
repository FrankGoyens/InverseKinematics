project(inversekinematics)

file(GLOB source
	"*.cpp"
)

add_executable(inversekinematics ${source})

find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)

target_include_directories(inversekinematics PUBLIC include
	PRIVATE glm)

target_link_libraries(inversekinematics glfw opengl32 GLU32 GLEW::GLEW)

install(TARGETS inversekinematics RUNTIME DESTINATION bin)
