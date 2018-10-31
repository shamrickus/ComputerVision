cmake_minimum_required (VERSION 3.1)

project(main C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
option(DEBUG "output debug info")

IF(WIN32)
	#ONLY 64BIT BINARIES
	set(OpenCV_DIR lib/cv)
	set(GLFW3_DIR lib/glfw)
	set(GLEW_DIR lib/glew)
endif()

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
	${GLFW3_INCLUDE_DIR})

set(SRC_HDR_FILES ${CMAKE_SOURCE_DIR}/*.h)
set(SRC_CPP_FILES ${CMAKE_SOURCE_DIR}/*.cpp)

file(GLOB_RECURSE SRC_HDR ${SRC_HDR_FILES})
file(GLOB_RECURSE SRC_CPP ${SRC_CPP_FILES})

add_executable (main final.cpp ${SRC_HDR}
		${SRC_CPP})
TARGET_LINK_LIBRARIES(main ${OpenCV_LIBS}
	${OPENGL_LIBRARY}
	${GLEW_LIBRARY}
	${GLFW3_LIBS})

IF(WIN32)
	foreach(dll ${DLLS})
		file(COPY ${dll} DESTINATION ${CMAKE_BINARY_DIR})
	endforeach()
endif()

#Copy our assets to bulid dir
add_custom_command(TARGET main POST_BUILD 
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:main>/assets)