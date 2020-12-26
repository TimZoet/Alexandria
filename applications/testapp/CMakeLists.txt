set(NAME testapp)
set(TYPE application)
set(INCLUDE_DIR "include/testapp")
set(SRC_DIR "src")

set(HEADERS
	${INCLUDE_DIR}/header.h
)
 
set(SOURCES
	${SRC_DIR}/main.cpp
)

set(DEPS_PUBLIC
	alexandria
	cppql
)

make_target(TYPE ${TYPE} NAME ${NAME} STARTUP HEADERS "${HEADERS}" SOURCES "${SOURCES}" DEPS_PUBLIC "${DEPS_PUBLIC}")