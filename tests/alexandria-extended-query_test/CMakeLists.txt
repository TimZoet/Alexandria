find_package(bettertest REQUIRED)

set(NAME alexandria-extended-query_test)
set(TYPE test)
set(INCLUDE_DIR "include/alexandria-extended-query_test")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/table_sets/table_sets_blob.h
	${INCLUDE_DIR}/table_sets/table_sets_blob_array.h
	${INCLUDE_DIR}/table_sets/table_sets_nested.h
	${INCLUDE_DIR}/table_sets/table_sets_primitive.h
	${INCLUDE_DIR}/table_sets/table_sets_primitive_array.h
	${INCLUDE_DIR}/table_sets/table_sets_primitive_blob.h
	${INCLUDE_DIR}/table_sets/table_sets_query.h
	${INCLUDE_DIR}/table_sets/table_sets_reference.h
	${INCLUDE_DIR}/table_sets/table_sets_reference_array.h
	${INCLUDE_DIR}/table_sets/table_sets_string.h
	${INCLUDE_DIR}/table_sets/table_sets_string_array.h
)

set(SOURCES
    ${SRC_DIR}/main.cpp

    ${SRC_DIR}/table_sets/table_sets_blob.cpp
	${SRC_DIR}/table_sets/table_sets_blob_array.cpp
	${SRC_DIR}/table_sets/table_sets_nested.cpp
	${SRC_DIR}/table_sets/table_sets_primitive.cpp
	${SRC_DIR}/table_sets/table_sets_primitive_array.cpp
	${SRC_DIR}/table_sets/table_sets_primitive_blob.cpp
	${SRC_DIR}/table_sets/table_sets_query.cpp
	${SRC_DIR}/table_sets/table_sets_reference.cpp
	${SRC_DIR}/table_sets/table_sets_reference_array.cpp
	${SRC_DIR}/table_sets/table_sets_string.cpp
	${SRC_DIR}/table_sets/table_sets_string_array.cpp
)

set(DEPS_PRIVATE
    alexandria_testutils
    alexandria-core
    alexandria-basic-query
    alexandria-extended-query
	bettertest::bettertest
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
