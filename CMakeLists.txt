cmake_minimum_required (VERSION 3.1)

project(main C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
option(DEBUG "output debug info")

IF(WIN32)
	#ONLY 64BIT BINARIES
	set(OpenCV_DIR lib/cv)
endif()

find_package(OpenCV REQUIRED)

IF(WIN32)
	#open cv dll location
	FILE(GLOB DLLS "${_OpenCV_LIB_PATH}/*.dll")
endif()

if(DEBUG)
	get_cmake_property(_variableNames VARIABLES)
	list (SORT _variableNames)
	foreach (_variableName ${_variableNames})
		message(STATUS "${_variableName}=${${_variableName}}")
	endforeach()
endif()

include_directories(${OpenCV_INCLUDE_DIRS})
set(SRC_HDR_FILES ${CMAKE_SOURCE_DIR}/*.h)
set(SRC_CPP_FILES ${CMAKE_SOURCE_DIR}/*.cpp)

file(GLOB_RECURSE SRC_HDR ${SRC_HDR_FILES})
file(GLOB_RECURSE SRC_CPP ${SRC_CPP_FILES})

add_executable (main final.cpp ${SRC_HDR}
		${SRC_CPP})
TARGET_LINK_LIBRARIES(main ${OpenCV_LIBS})

IF(False)
	#Copy our dlls to build dir
	foreach( dll ${DLLS})
		add_custom_command(
			TARGET main PRE_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
				${dll} $<TARGET_FILE_DIR:main>)
	endforeach(dll )
endif()

#Copy our assets to bulid dir
add_custom_command(TARGET main POST_BUILD 
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:main>/assets)