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
    ${INCLUDE_DIR}/data_type.h
    ${INCLUDE_DIR}/fwd.h
    ${INCLUDE_DIR}/library.h
    ${INCLUDE_DIR}/member.h
    ${INCLUDE_DIR}/namespace.h
    ${INCLUDE_DIR}/property_layout.h
    ${INCLUDE_DIR}/type.h
    ${INCLUDE_DIR}/type_descriptor.h
    ${INCLUDE_DIR}/type_layout.h
    ${INCLUDE_DIR}/properties/blob.h
    ${INCLUDE_DIR}/properties/blob_array.h
    ${INCLUDE_DIR}/properties/instance_id.h
    ${INCLUDE_DIR}/properties/primitive.h
    ${INCLUDE_DIR}/properties/primitive_array.h
    ${INCLUDE_DIR}/properties/primitive_blob.h
    ${INCLUDE_DIR}/properties/reference.h
    ${INCLUDE_DIR}/properties/reference_array.h
    ${INCLUDE_DIR}/properties/string.h
    ${INCLUDE_DIR}/properties/string_array.h
)

set(SOURCES
    ${SRC_DIR}/data_type.cpp
    ${SRC_DIR}/library.cpp
    ${SRC_DIR}/namespace.cpp
    ${SRC_DIR}/property_layout.cpp
    ${SRC_DIR}/type.cpp
    ${SRC_DIR}/type_layout.cpp
    ${SRC_DIR}/properties/instance_id.cpp
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
