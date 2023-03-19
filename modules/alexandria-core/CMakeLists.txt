find_package(common REQUIRED)
find_package(cppql REQUIRED)
find_package(dot REQUIRED)
find_package(stduuid REQUIRED)

include(alexandriaVersionString)

set(NAME alexandria-core)
set(TYPE module)
set(INCLUDE_DIR "include/alexandria-core")
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
