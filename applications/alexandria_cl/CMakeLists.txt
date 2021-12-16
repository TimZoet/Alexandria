find_package(parsertongue REQUIRED)

set(NAME alexandria_cl)
set(TYPE application)
set(INCLUDE_DIR "include/alexandria_cl")
set(SRC_DIR "src")

set(HEADERS
	${INCLUDE_DIR}/specification.h
)
 
set(SOURCES
	${SRC_DIR}/main.cpp
	${SRC_DIR}/specification.cpp
)

set(DEPS_PUBLIC
	alexandria
	parsertongue::parsertongue
)

make_target(TYPE ${TYPE} NAME ${NAME} HEADERS "${HEADERS}" SOURCES "${SOURCES}" DEPS_PUBLIC "${DEPS_PUBLIC}")