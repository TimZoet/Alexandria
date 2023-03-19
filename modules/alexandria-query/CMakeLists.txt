include(alexandriaVersionString)

set(NAME alexandria-query)
set(TYPE module)
set(INCLUDE_DIR "include/alexandria-query")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/queries/delete_query.h
    ${INCLUDE_DIR}/queries/get_query.h
    ${INCLUDE_DIR}/queries/insert_query.h
    ${INCLUDE_DIR}/queries/table_sets.h
    ${INCLUDE_DIR}/queries/update_query.h
    ${INCLUDE_DIR}/queries/utils.h
    ${INCLUDE_DIR}/queries/deleters/blob_array_deleter.h
    ${INCLUDE_DIR}/queries/deleters/primitive_array_deleter.h
    ${INCLUDE_DIR}/queries/deleters/primitive_deleter.h
    ${INCLUDE_DIR}/queries/deleters/reference_array_deleter.h
    ${INCLUDE_DIR}/queries/getters/blob_array_getter.h
    ${INCLUDE_DIR}/queries/getters/primitive_array_getter.h
    ${INCLUDE_DIR}/queries/getters/primitive_getter.h
    ${INCLUDE_DIR}/queries/getters/reference_array_getter.h
    ${INCLUDE_DIR}/queries/inserters/blob_array_inserter.h
    ${INCLUDE_DIR}/queries/inserters/primitive_array_inserter.h
    ${INCLUDE_DIR}/queries/inserters/primitive_inserter.h
    ${INCLUDE_DIR}/queries/inserters/reference_array_inserter.h
    ${INCLUDE_DIR}/queries/updaters/primitive_updater.h
    ${INCLUDE_DIR}/queries/table_sets/blob_array_table_set.h
    ${INCLUDE_DIR}/queries/table_sets/primitive_array_table_set.h
    ${INCLUDE_DIR}/queries/table_sets/primitive_table_set.h
    ${INCLUDE_DIR}/queries/table_sets/reference_array_table_set.h
    ${INCLUDE_DIR}/queries/types/member_extractor.h
)

set(SOURCES
    src/liboutput.cpp
)

set(DEPS_PUBLIC
    alexandria-core
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
