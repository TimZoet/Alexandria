set(NAME geometry)
set(TYPE application)
set(INCLUDE_DIR "include/geometry")
set(SRC_DIR "src")

set(HEADERS
	${INCLUDE_DIR}/export.h
	${INCLUDE_DIR}/import.h
	${INCLUDE_DIR}/tiny_obj_loader.h
	${INCLUDE_DIR}/types.h
	${INCLUDE_DIR}/winutils.h
)
 
set(SOURCES
	${SRC_DIR}/export.cpp
	${SRC_DIR}/import.cpp
	${SRC_DIR}/main.cpp
	${SRC_DIR}/types.cpp
	${SRC_DIR}/winutils.cpp
)

set(DEPS_PUBLIC
	alexandria
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