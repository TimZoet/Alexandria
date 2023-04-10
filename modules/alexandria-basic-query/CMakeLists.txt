include(alexandriaVersionString)

set(NAME alexandria-basic-query)
set(TYPE module)
set(INCLUDE_DIR "include/alexandria-basic-query")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/delete_query.h
    ${INCLUDE_DIR}/get_query.h
    ${INCLUDE_DIR}/insert_query.h
    ${INCLUDE_DIR}/update_query.h
    ${INCLUDE_DIR}/utils.h
    ${INCLUDE_DIR}/deleters/blob_array_deleter.h
    ${INCLUDE_DIR}/deleters/primitive_array_deleter.h
    ${INCLUDE_DIR}/deleters/primitive_deleter.h
    ${INCLUDE_DIR}/deleters/reference_array_deleter.h
    ${INCLUDE_DIR}/getters/blob_array_getter.h
    ${INCLUDE_DIR}/getters/primitive_array_getter.h
    ${INCLUDE_DIR}/getters/primitive_getter.h
    ${INCLUDE_DIR}/getters/reference_array_getter.h
    ${INCLUDE_DIR}/inserters/blob_array_inserter.h
    ${INCLUDE_DIR}/inserters/primitive_array_inserter.h
    ${INCLUDE_DIR}/inserters/primitive_inserter.h
    ${INCLUDE_DIR}/inserters/reference_array_inserter.h
    ${INCLUDE_DIR}/updaters/primitive_updater.h
    ${INCLUDE_DIR}/types/member_extractor.h
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
