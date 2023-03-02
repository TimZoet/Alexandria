find_package(parsertongue)

set(NAME geometry)
set(TYPE application)
set(INCLUDE_DIR "include/geometry")
set(SRC_DIR "src")

set(HEADERS
	#${INCLUDE_DIR}/actions.h
	${INCLUDE_DIR}/export.h
	${INCLUDE_DIR}/library.h
	#${INCLUDE_DIR}/import.h
	#${INCLUDE_DIR}/tiny_obj_loader.h
	#${INCLUDE_DIR}/types.h
	${INCLUDE_DIR}/winutils.h
	${INCLUDE_DIR}/types/cube.h
	${INCLUDE_DIR}/types/float3.h
	${INCLUDE_DIR}/types/int3.h
	${INCLUDE_DIR}/types/material.h
	${INCLUDE_DIR}/types/mesh.h
	${INCLUDE_DIR}/types/node.h
	${INCLUDE_DIR}/types/scene.h
	${INCLUDE_DIR}/types/sphere.h
)
 
set(SOURCES
	#${SRC_DIR}/actions.cpp
	${SRC_DIR}/export.cpp
	${SRC_DIR}/library.cpp
	#${SRC_DIR}/import.cpp
	${SRC_DIR}/main.cpp
	#${SRC_DIR}/types.cpp
	${SRC_DIR}/winutils.cpp
)

set(DEPS_PUBLIC
	alexandria
    parsertongue::parsertongue
)

make_target(
	TYPE ${TYPE}
	NAME ${NAME}
	WARNINGS WERROR
	EXAMPLE
	HEADERS "${HEADERS}"
	SOURCES "${SOURCES}"
	DEPS_PUBLIC "${DEPS_PUBLIC}"
)