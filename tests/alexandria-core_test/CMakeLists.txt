find_package(bettertest REQUIRED)

set(NAME alexandria-core_test)
set(TYPE test)
set(INCLUDE_DIR "include/alexandria-core_test")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/library/create_library.h

    ${INCLUDE_DIR}/member_types/member_type_blob.h
    ${INCLUDE_DIR}/member_types/member_type_blob_custom.h
    ${INCLUDE_DIR}/member_types/member_type_blob_array.h
    ${INCLUDE_DIR}/member_types/member_type_blob_array_custom.h
    #${INCLUDE_DIR}/member_types/member_type_instance_id.h
    ${INCLUDE_DIR}/member_types/member_type_primitive_array.h
    ${INCLUDE_DIR}/member_types/member_type_primitive_array_custom.h
    ${INCLUDE_DIR}/member_types/member_type_primitive_blob.h
    ${INCLUDE_DIR}/member_types/member_type_primitive_blob_custom.h
    #${INCLUDE_DIR}/member_types/member_type_reference.h
    #${INCLUDE_DIR}/member_types/member_type_reference_array.h
    ${INCLUDE_DIR}/member_types/member_type_string_array.h
    ${INCLUDE_DIR}/member_types/member_type_string_array_custom.h

    ${INCLUDE_DIR}/types/create_type.h
    ${INCLUDE_DIR}/types/create_type_blob.h
    ${INCLUDE_DIR}/types/create_type_blob_array.h
    ${INCLUDE_DIR}/types/create_type_nested.h
    ${INCLUDE_DIR}/types/create_type_primitive.h
    ${INCLUDE_DIR}/types/create_type_primitive_array.h
    ${INCLUDE_DIR}/types/create_type_primitive_blob.h
    ${INCLUDE_DIR}/types/create_type_reference.h
    ${INCLUDE_DIR}/types/create_type_reference_array.h
    ${INCLUDE_DIR}/types/create_type_string.h
    ${INCLUDE_DIR}/types/create_type_string_array.h
)

set(SOURCES
    ${SRC_DIR}/main.cpp

    ${SRC_DIR}/library/create_library.cpp

    ${SRC_DIR}/member_types/member_type_blob.cpp
    ${SRC_DIR}/member_types/member_type_blob_custom.cpp
    ${SRC_DIR}/member_types/member_type_blob_array.cpp
    ${SRC_DIR}/member_types/member_type_blob_array_custom.cpp
    #${SRC_DIR}/member_types/member_type_instance_id.cpp
    ${SRC_DIR}/member_types/member_type_primitive_array.cpp
    ${SRC_DIR}/member_types/member_type_primitive_array_custom.cpp
    ${SRC_DIR}/member_types/member_type_primitive_blob.cpp
    ${SRC_DIR}/member_types/member_type_primitive_blob_custom.cpp
    #${SRC_DIR}/member_types/member_type_reference.cpp
    #${SRC_DIR}/member_types/member_type_reference_array.cpp
    ${SRC_DIR}/member_types/member_type_string_array.cpp
    ${SRC_DIR}/member_types/member_type_string_array_custom.cpp

    ${SRC_DIR}/types/create_type.cpp
    ${SRC_DIR}/types/create_type_blob.cpp
    ${SRC_DIR}/types/create_type_blob_array.cpp
    ${SRC_DIR}/types/create_type_nested.cpp
    ${SRC_DIR}/types/create_type_primitive.cpp
    ${SRC_DIR}/types/create_type_primitive_array.cpp
    ${SRC_DIR}/types/create_type_primitive_blob.cpp
    ${SRC_DIR}/types/create_type_reference.cpp
    ${SRC_DIR}/types/create_type_reference_array.cpp
    ${SRC_DIR}/types/create_type_string.cpp
    ${SRC_DIR}/types/create_type_string_array.cpp
)

set(DEPS_PRIVATE
    alexandria_testutils
    alexandria-core
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
