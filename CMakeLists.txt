cmake_minimum_required (VERSION 3.1)

project(final LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(OpenGL_GL_PREFERENCE GLVND)
option(DEBUG "output debug info")

IF(WIN32)
	#ONLY 64BIT BINARIES
	set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/lib/cv)
	set(GLFW3_DIR lib/glfw)
	set(GLEW_DIR lib/glew)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W4")

else()
	set(GLEW_DIR /usr/share/cmake-3.12/Modules)	
	find_package(PkgConfig REQUIRED)
	pkg_search_module(GLFW REQUIRED glfw3)
	set(GLFW3_DIR ${GLFW_INCLUDE_DIRS})
	set(GLFW3_LIBS ${GLFW_LIBRARIES})
endif()

set(GLM_INCLUDE_DIR lib/glm)
set(REACTPHYSICS3D_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/lib/rp3d/src")
FILE(GLOB REACTPHYSICS3D_LIBS ${CMAKE_SOURCE_DIR}/lib/rp3d/*.lib)


find_PACKAGE(OpenGL REQUIRED)
find_package(OpenCV REQUIRED
	HINTS ${OpenCV_DIR})
find_package(GLEW REQUIRED
	HINTS ${GLEW_DIR})
find_package(GLFW3 REQUIRED
	HINTS ${GLFW3_DIR})

IF(WIN32)
	#open cv dll location
	FILE(GLOB d "${_OpenCV_LIB_PATH}/*.dll")
	SET(DLLS ${DLLS} ${d})
endif()

if(DEBUG)
	get_cmake_property(_variableNames VARIABLES)
	list (SORT _variableNames)
	foreach (_variableName ${_variableNames})
		message(STATUS "${_variableName}=${${_variableName}}")
	endforeach()
endif()

include_directories(${OpenCV_INCLUDE_DIRS}
	${OPENGL_INCLUDE_DIR}
	${GLEW_INCLUDE_DIR}
	${GLFW3_INCLUDE_DIR}
	${GLM_INCLUDE_DIR}
	${REACTPHYSICS3D_INCLUDE_DIR}
	)

set(LINK_LIBS ${OpenCV_LIBS}
	${OPENGL_LIBRARY}
	${GLEW_LIBRARY}
	${GLFW3_LIBS}
	${REACTPHYSICS3D_LIBS}
	)

set(SRC_HDR_FILES ${CMAKE_SOURCE_DIR}/src/*.h)
set(SRC_CPP_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE SRC_HDR ${SRC_HDR_FILES})
file(GLOB_RECURSE SRC_CPP ${SRC_CPP_FILES})

add_executable (main ${SRC_HDR}
		${SRC_CPP})
TARGET_LINK_LIBRARIES(main 
		${LINK_LIBS})


include(CheckFunctionExists)
add_subdirectory(lib/Catch2)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/lib/Catch2/contrib/")
check_function_exists(catch_discover_tests result)
if(${result})
	add_executable(tests "${CMAKE_SOURCE_DIR}/tests/PoC.cpp" "${CMAKE_SOURCE_DIR}/src/Logger.h")
	target_link_libraries(tests Catch2::Catch2 ${LINK_LIBS})
	include (CTest)
	include(Catch)
	catch_discover_tests(tests)
endif()




IF(WIN32)
	foreach(dll ${DLLS})
		file(COPY ${dll} DESTINATION ${CMAKE_BINARY_DIR})
	endforeach()
endif()

#Copy our assets to bulid dir
add_custom_command(TARGET main POST_BUILD 
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:main>assets)
