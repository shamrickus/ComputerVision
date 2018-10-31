IF(WIN32)
	set(GLEW_INCLUDE_DIR lib/glew/include)

	FILE(GLOB GLEW_LIBRARY ${CMAKE_SOURCE_DIR}/${GLEW_DIR}/lib/*.lib)

	set(GLEW_DLLS ${CMAKE_SOURCE_DIR}/${GLEW_DIR}/bin/glew32.dll)

	SET(DLLS
		${DLLS}
		${GLEW_DLLS})

endif()