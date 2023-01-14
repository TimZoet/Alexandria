find_package(common REQUIRED)
find_package(cppql REQUIRED)
find_package(dot REQUIRED)
find_package(stduuid REQUIRED)

include(alexandriaVersionString)

set(NAME alexandria)
set(TYPE module)
set(INCLUDE_DIR "include/alexandria")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/core/data_type.h
    ${INCLUDE_DIR}/core/fwd.h
    ${INCLUDE_DIR}/core/library.h
    ${INCLUDE_DIR}/core/member.h
    ${INCLUDE_DIR}/core/namespace.h
    ${INCLUDE_DIR}/core/property.h
    ${INCLUDE_DIR}/core/type.h
    ${INCLUDE_DIR}/core/type_descriptor.h
    ${INCLUDE_DIR}/member_types/blob.h
    ${INCLUDE_DIR}/member_types/blob_array.h
    ${INCLUDE_DIR}/member_types/instance_id.h
    ${INCLUDE_DIR}/member_types/primitive.h
    ${INCLUDE_DIR}/member_types/primitive_array.h
    ${INCLUDE_DIR}/member_types/primitive_blob.h
    ${INCLUDE_DIR}/member_types/reference.h
    ${INCLUDE_DIR}/member_types/reference_array.h
    ${INCLUDE_DIR}/member_types/string.h
    ${INCLUDE_DIR}/member_types/string_array.h
    ${INCLUDE_DIR}/queries/delete_query.h
    ${INCLUDE_DIR}/queries/get_query.h
    ${INCLUDE_DIR}/queries/insert_query.h
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
    ${INCLUDE_DIR}/queries/types/member_extractor.h
)

set(SOURCES
    ${SRC_DIR}/core/data_type.cpp
    ${SRC_DIR}/core/library.cpp
    ${SRC_DIR}/core/namespace.cpp
    ${SRC_DIR}/core/property.cpp
    ${SRC_DIR}/core/type.cpp
    ${SRC_DIR}/member_types/instance_id.cpp
)

set(DEPS_PUBLIC
    common::common
    cppql::cppql
    stduuid::stduuid
)

set(DEPS_PRIVATE
    dot::dot
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
