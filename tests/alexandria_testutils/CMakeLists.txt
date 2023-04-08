find_package(bettertest REQUIRED)

set(NAME alexandria_testutils)
set(TYPE module)
set(INCLUDE_DIR "include/alexandria_testutils")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/utils.h
)

set(SOURCES
    ${SRC_DIR}/utils.cpp
)

set(DEPS_PRIVATE
    alexandria-core
	bettertest::bettertest
)

make_target(
    TYPE ${TYPE}
    NAME ${NAME}
    OUTDIR "tests"
    FOLDER "tests"
    WARNINGS WERROR
    STARTUP
    HEADERS "${HEADERS}"
    SOURCES "${SOURCES}"
    DEPS_PRIVATE "${DEPS_PRIVATE}"
)
