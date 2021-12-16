find_package(bettertest REQUIRED)

set(NAME alexandria_test)
set(TYPE test)
set(INCLUDE_DIR "include/alexandria_test")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/utils.h

    ${INCLUDE_DIR}/create/create_direct.h
    ${INCLUDE_DIR}/create/create_managed.h
    ${INCLUDE_DIR}/create/create_primitive.h
    ${INCLUDE_DIR}/create/create_string.h

    ${INCLUDE_DIR}/delete/delete_blob.h
    ${INCLUDE_DIR}/delete/delete_blob_array.h
    ${INCLUDE_DIR}/delete/delete_direct.h
    ${INCLUDE_DIR}/delete/delete_managed.h
    ${INCLUDE_DIR}/delete/delete_primitive.h
    ${INCLUDE_DIR}/delete/delete_primitive_array.h
    ${INCLUDE_DIR}/delete/delete_primitive_blob.h
    ${INCLUDE_DIR}/delete/delete_reference.h
    ${INCLUDE_DIR}/delete/delete_reference_array.h
    ${INCLUDE_DIR}/delete/delete_string.h
    ${INCLUDE_DIR}/delete/delete_string_array.h

    ${INCLUDE_DIR}/get/get_blob.h
	${INCLUDE_DIR}/get/get_blob_array.h
	${INCLUDE_DIR}/get/get_direct.h
	${INCLUDE_DIR}/get/get_managed.h
	${INCLUDE_DIR}/get/get_primitive.h
	${INCLUDE_DIR}/get/get_primitive_array.h
	${INCLUDE_DIR}/get/get_primitive_blob.h
	${INCLUDE_DIR}/get/get_reference.h
	${INCLUDE_DIR}/get/get_reference_array.h
	${INCLUDE_DIR}/get/get_string.h
	${INCLUDE_DIR}/get/get_string_array.h

    ${INCLUDE_DIR}/insert/insert_blob.h
	${INCLUDE_DIR}/insert/insert_blob_array.h
	${INCLUDE_DIR}/insert/insert_direct.h
	${INCLUDE_DIR}/insert/insert_managed.h
	${INCLUDE_DIR}/insert/insert_primitive.h
	${INCLUDE_DIR}/insert/insert_primitive_array.h
	${INCLUDE_DIR}/insert/insert_primitive_blob.h
	${INCLUDE_DIR}/insert/insert_reference.h
	${INCLUDE_DIR}/insert/insert_reference_array.h
	${INCLUDE_DIR}/insert/insert_string.h
	${INCLUDE_DIR}/insert/insert_string_array.h

    ${INCLUDE_DIR}/library/create_library.h

    ${INCLUDE_DIR}/member_types/member_type_blob.h
	${INCLUDE_DIR}/member_types/member_type_blob_array.h
	${INCLUDE_DIR}/member_types/member_type_instance_id.h
	${INCLUDE_DIR}/member_types/member_type_primitive_array.h
	${INCLUDE_DIR}/member_types/member_type_primitive_blob.h
	${INCLUDE_DIR}/member_types/member_type_reference.h
	${INCLUDE_DIR}/member_types/member_type_reference_array.h
	${INCLUDE_DIR}/member_types/member_type_string_array.h

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

    ${INCLUDE_DIR}/update/update_direct.h
	${INCLUDE_DIR}/update/update_managed.h
)

set(SOURCES
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/utils.cpp

    ${SRC_DIR}/create/create_direct.cpp
    ${SRC_DIR}/create/create_managed.cpp
    ${SRC_DIR}/create/create_primitive.cpp
    ${SRC_DIR}/create/create_string.cpp

    ${SRC_DIR}/delete/delete_blob.cpp
    ${SRC_DIR}/delete/delete_blob_array.cpp
    ${SRC_DIR}/delete/delete_direct.cpp
    ${SRC_DIR}/delete/delete_managed.cpp
    ${SRC_DIR}/delete/delete_primitive.cpp
    ${SRC_DIR}/delete/delete_primitive_array.cpp
    ${SRC_DIR}/delete/delete_primitive_blob.cpp
    ${SRC_DIR}/delete/delete_reference.cpp
    ${SRC_DIR}/delete/delete_reference_array.cpp
    ${SRC_DIR}/delete/delete_string.cpp
    ${SRC_DIR}/delete/delete_string_array.cpp

    ${SRC_DIR}/get/get_blob.cpp
	${SRC_DIR}/get/get_blob_array.cpp
	${SRC_DIR}/get/get_direct.cpp
	${SRC_DIR}/get/get_managed.cpp
	${SRC_DIR}/get/get_primitive.cpp
	${SRC_DIR}/get/get_primitive_array.cpp
	${SRC_DIR}/get/get_primitive_blob.cpp
	${SRC_DIR}/get/get_reference.cpp
	${SRC_DIR}/get/get_reference_array.cpp
	${SRC_DIR}/get/get_string.cpp
	${SRC_DIR}/get/get_string_array.cpp

    ${SRC_DIR}/insert/insert_blob.cpp
	${SRC_DIR}/insert/insert_blob_array.cpp
	${SRC_DIR}/insert/insert_direct.cpp
	${SRC_DIR}/insert/insert_managed.cpp
	${SRC_DIR}/insert/insert_primitive.cpp
	${SRC_DIR}/insert/insert_primitive_array.cpp
	${SRC_DIR}/insert/insert_primitive_blob.cpp
	${SRC_DIR}/insert/insert_reference.cpp
	${SRC_DIR}/insert/insert_reference_array.cpp
	${SRC_DIR}/insert/insert_string.cpp
	${SRC_DIR}/insert/insert_string_array.cpp

    ${SRC_DIR}/library/create_library.cpp

    ${SRC_DIR}/member_types/member_type_blob.cpp
	${SRC_DIR}/member_types/member_type_blob_array.cpp
	${SRC_DIR}/member_types/member_type_instance_id.cpp
	${SRC_DIR}/member_types/member_type_primitive_array.cpp
	${SRC_DIR}/member_types/member_type_primitive_blob.cpp
	${SRC_DIR}/member_types/member_type_reference.cpp
	${SRC_DIR}/member_types/member_type_reference_array.cpp
	${SRC_DIR}/member_types/member_type_string_array.cpp

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

    ${SRC_DIR}/update/update_direct.cpp
	${SRC_DIR}/update/update_managed.cpp
)

set(DEPS_PRIVATE
    alexandria
	bettertest::bettertest
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
