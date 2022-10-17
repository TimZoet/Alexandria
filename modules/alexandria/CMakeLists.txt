find_package(common REQUIRED)
find_package(cppql REQUIRED)
find_package(dot REQUIRED)

include(alexandriaVersionString)

set(NAME alexandria)
set(TYPE module)
set(INCLUDE_DIR "include/alexandria")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/data_type.h
    ${INCLUDE_DIR}/library.h
    ${INCLUDE_DIR}/property.h
    ${INCLUDE_DIR}/type.h
    ${INCLUDE_DIR}/member_types/blob.h
    ${INCLUDE_DIR}/member_types/blob_array.h
    ${INCLUDE_DIR}/member_types/instance_id.h
    ${INCLUDE_DIR}/member_types/member.h
    ${INCLUDE_DIR}/member_types/primitive.h
    ${INCLUDE_DIR}/member_types/primitive_array.h
    ${INCLUDE_DIR}/member_types/primitive_blob.h
    ${INCLUDE_DIR}/member_types/reference.h
    ${INCLUDE_DIR}/member_types/reference_array.h
    ${INCLUDE_DIR}/member_types/string.h
    ${INCLUDE_DIR}/member_types/string_array.h
    ${INCLUDE_DIR}/object_handler/blob_array_handler.h
    ${INCLUDE_DIR}/object_handler/object_handler.h
    ${INCLUDE_DIR}/object_handler/primitive_array_handler.h
    ${INCLUDE_DIR}/object_handler/primitive_handler.h
    ${INCLUDE_DIR}/object_handler/reference_array_handler.h
    ${INCLUDE_DIR}/object_handler/type_traits.h
    ${INCLUDE_DIR}/queries/query.h
)
 
set(SOURCES
    ${SRC_DIR}/data_type.cpp
    ${SRC_DIR}/library.cpp
    ${SRC_DIR}/property.cpp
    ${SRC_DIR}/type.cpp
)

set(DEPS_PUBLIC
    common::common
    cppql::core
    cppql::typed
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
