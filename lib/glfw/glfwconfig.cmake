IF(WIN32)
	set(GLFW_INCLUDE_DIR ${GLFW_DIR}/include)

	FILE(GLOB GLFW_LIBS ${GLFW_DIR}/lib/*.lib)

	FILE(GLOB GLFW_DLLS ${GLFW_DIR}/bin/*.dll)

	SET(DLLS
		${DLLS}
		${GLFW_DLLS})
endif()