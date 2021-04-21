set(NAME alexandria_test)
set(TYPE test)
set(INCLUDE_DIR "include/alexandria_test")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/utils.h

    # ${INCLUDE_DIR}/create_library.h
    # ${INCLUDE_DIR}/create_property.h
    # ${INCLUDE_DIR}/create_property_blob.h
    # ${INCLUDE_DIR}/create_property_nested.h
    # ${INCLUDE_DIR}/create_property_primitive.h
    # ${INCLUDE_DIR}/type_add_property.h

    ${INCLUDE_DIR}/get/get_blob.h
	${INCLUDE_DIR}/get/get_blob_array.h
	${INCLUDE_DIR}/get/get_primitive.h
	${INCLUDE_DIR}/get/get_primitive_array.h
	${INCLUDE_DIR}/get/get_primitive_blob.h
	${INCLUDE_DIR}/get/get_reference.h
	${INCLUDE_DIR}/get/get_reference_array.h
	${INCLUDE_DIR}/get/get_string.h
	${INCLUDE_DIR}/get/get_string_array.h

    ${INCLUDE_DIR}/insert/insert_blob.h
	${INCLUDE_DIR}/insert/insert_blob_array.h
	${INCLUDE_DIR}/insert/insert_primitive.h
	${INCLUDE_DIR}/insert/insert_primitive_array.h
	${INCLUDE_DIR}/insert/insert_primitive_blob.h
	${INCLUDE_DIR}/insert/insert_reference.h
	${INCLUDE_DIR}/insert/insert_reference_array.h
	${INCLUDE_DIR}/insert/insert_string.h
	${INCLUDE_DIR}/insert/insert_string_array.h

    ${INCLUDE_DIR}/types/create_type.h
    ${INCLUDE_DIR}/types/create_type_blob.h
    ${INCLUDE_DIR}/types/create_type_blob_array.h
    ${INCLUDE_DIR}/types/create_type_primitive.h
    ${INCLUDE_DIR}/types/create_type_primitive_array.h
    ${INCLUDE_DIR}/types/create_type_primitive_blob.h
    ${INCLUDE_DIR}/types/create_type_reference.h
    ${INCLUDE_DIR}/types/create_type_reference_array.h
    ${INCLUDE_DIR}/types/create_type_string.h
    ${INCLUDE_DIR}/types/create_type_string_array.h
    ${INCLUDE_DIR}/types/create_type_type.h
)

set(SOURCES
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/utils.cpp
    # ${SRC_DIR}/create_library.cpp
    # ${SRC_DIR}/create_property.cpp
    # ${SRC_DIR}/create_property_blob.cpp
    # ${SRC_DIR}/create_property_nested.cpp
    # ${SRC_DIR}/create_property_primitive.cpp
    
    # ${SRC_DIR}/type_add_property.cpp

    ${SRC_DIR}/get/get_blob.cpp
	${SRC_DIR}/get/get_blob_array.cpp
	${SRC_DIR}/get/get_primitive.cpp
	${SRC_DIR}/get/get_primitive_array.cpp
	${SRC_DIR}/get/get_primitive_blob.cpp
	${SRC_DIR}/get/get_reference.cpp
	${SRC_DIR}/get/get_reference_array.cpp
	${SRC_DIR}/get/get_string.cpp
	${SRC_DIR}/get/get_string_array.cpp

    ${SRC_DIR}/insert/insert_blob.cpp
	${SRC_DIR}/insert/insert_blob_array.cpp
	${SRC_DIR}/insert/insert_primitive.cpp
	${SRC_DIR}/insert/insert_primitive_array.cpp
	${SRC_DIR}/insert/insert_primitive_blob.cpp
	${SRC_DIR}/insert/insert_reference.cpp
	${SRC_DIR}/insert/insert_reference_array.cpp
	${SRC_DIR}/insert/insert_string.cpp
	${SRC_DIR}/insert/insert_string_array.cpp

    ${SRC_DIR}/types/create_type.cpp
    ${SRC_DIR}/types/create_type_blob.cpp
    ${SRC_DIR}/types/create_type_blob_array.cpp
    ${SRC_DIR}/types/create_type_primitive.cpp
    ${SRC_DIR}/types/create_type_primitive_array.cpp
    ${SRC_DIR}/types/create_type_primitive_blob.cpp
    ${SRC_DIR}/types/create_type_reference.cpp
    ${SRC_DIR}/types/create_type_reference_array.cpp
    ${SRC_DIR}/types/create_type_string.cpp
    ${SRC_DIR}/types/create_type_string_array.cpp
    ${SRC_DIR}/types/create_type_type.cpp
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
