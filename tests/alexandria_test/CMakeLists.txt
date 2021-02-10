set(NAME alexandria_test)
set(TYPE test)
set(INCLUDE_DIR "include/alexandria_test")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/create_library.h
    ${INCLUDE_DIR}/create_property.h
    ${INCLUDE_DIR}/create_property_blob.h
    ${INCLUDE_DIR}/create_property_nested.h
    ${INCLUDE_DIR}/create_property_primitive.h
    ${INCLUDE_DIR}/create_type.h
    ${INCLUDE_DIR}/create_type_blob.h
    ${INCLUDE_DIR}/create_type_blob_array.h
    ${INCLUDE_DIR}/create_type_nested.h
    ${INCLUDE_DIR}/create_type_nested_array.h
    ${INCLUDE_DIR}/create_type_nested_reference.h
    ${INCLUDE_DIR}/create_type_primitive.h
    ${INCLUDE_DIR}/create_type_primitive_array.h
    ${INCLUDE_DIR}/create_type_primitive_blob.h
    ${INCLUDE_DIR}/type_add_property.h
    ${INCLUDE_DIR}/utils.h
)

set(SOURCES
    ${SRC_DIR}/create_library.cpp
    ${SRC_DIR}/create_property.cpp
    ${SRC_DIR}/create_property_blob.cpp
    ${SRC_DIR}/create_property_nested.cpp
    ${SRC_DIR}/create_property_primitive.cpp
    ${SRC_DIR}/create_type.cpp
    ${SRC_DIR}/create_type_blob.cpp
    ${SRC_DIR}/create_type_blob_array.cpp
    ${SRC_DIR}/create_type_nested.cpp
    ${SRC_DIR}/create_type_nested_array.cpp
    ${SRC_DIR}/create_type_nested_reference.cpp
    ${SRC_DIR}/create_type_primitive.cpp
    ${SRC_DIR}/create_type_primitive_array.cpp
    ${SRC_DIR}/create_type_primitive_blob.cpp
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/type_add_property.cpp
    ${SRC_DIR}/utils.cpp
)

set(DEPS_PRIVATE
    alexandria
	bettertest
)

make_target(
    TYPE ${TYPE}
    NAME ${NAME}
    OUTDIR "tests"
    # WARNINGS WERROR
    STARTUP
    HEADERS "${HEADERS}"
    SOURCES "${SOURCES}"
    DEPS_PRIVATE "${DEPS_PRIVATE}"
)
