include(alexandriaVersionString)

set(NAME alexandria-extended-query)
set(TYPE module)
set(INCLUDE_DIR "include/alexandria-extended-query")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/search_query.h
    ${INCLUDE_DIR}/table_sets.h

    ${INCLUDE_DIR}/search_queries/primitive_search.h
    ${INCLUDE_DIR}/search_queries/reference_search.h

    ${INCLUDE_DIR}/table_sets/blob_array_table_set.h
    ${INCLUDE_DIR}/table_sets/primitive_array_table_set.h
    ${INCLUDE_DIR}/table_sets/primitive_table_set.h
    ${INCLUDE_DIR}/table_sets/reference_array_table_set.h
)

set(SOURCES
    src/liboutput.cpp
)

set(DEPS_PUBLIC
    alexandria-core
    alexandria-basic-query
)

set(DEPS_PRIVATE
)

make_target(
    NAME ${NAME}
    TYPE ${TYPE}
    VERSION ${ALEXANDRIA_VERSION}
    WARNINGS WERROR
    HEADERS "${HEADERS}"
    SOURCES "${SOURCES}"
    DEPS_PUBLIC "${DEPS_PUBLIC}"
    DEPS_PRIVATE "${DEPS_PRIVATE}"
)

install_target(
    NAME ${NAME}
    TYPE ${TYPE}
    VERSION ${ALEXANDRIA_VERSION}
    HEADERS "${HEADERS}"
)
