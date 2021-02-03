set(NAME alexandria_test)
set(TYPE test)
set(INCLUDE_DIR "include/alexandria_test")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/create_library.h
)

set(SOURCES
    ${SRC_DIR}/create_library.cpp
    ${SRC_DIR}/main.cpp
)

set(DEPS_PRIVATE
    alexandria
	bettertest
)

make_target(
    TYPE ${TYPE}
    NAME ${NAME}
    OUTDIR "tests"
    WARNINGS WERROR
    STARTUP
    HEADERS "${HEADERS}"
    SOURCES "${SOURCES}"
    DEPS_PRIVATE "${DEPS_PRIVATE}"
)
