find_package(bettertest REQUIRED)

set(NAME alexandria-basic-query_test)
set(TYPE test)
set(INCLUDE_DIR "include/alexandria-basic-query_test")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/delete/delete_blob.h
    ${INCLUDE_DIR}/delete/delete_blob_array.h
    ${INCLUDE_DIR}/delete/delete_invalid.h
    ${INCLUDE_DIR}/delete/delete_primitive.h
    ${INCLUDE_DIR}/delete/delete_primitive_array.h
    ${INCLUDE_DIR}/delete/delete_primitive_blob.h
    ${INCLUDE_DIR}/delete/delete_reference.h
    ${INCLUDE_DIR}/delete/delete_reference_array.h
    ${INCLUDE_DIR}/delete/delete_string.h
    ${INCLUDE_DIR}/delete/delete_string_array.h

    ${INCLUDE_DIR}/get/get_blob.h
    ${INCLUDE_DIR}/get/get_blob_array.h
    ${INCLUDE_DIR}/get/get_invalid.h
    ${INCLUDE_DIR}/get/get_primitive.h
    ${INCLUDE_DIR}/get/get_primitive_array.h
    ${INCLUDE_DIR}/get/get_primitive_blob.h
    ${INCLUDE_DIR}/get/get_reference.h
    ${INCLUDE_DIR}/get/get_reference_array.h
    ${INCLUDE_DIR}/get/get_string.h
    ${INCLUDE_DIR}/get/get_string_array.h

    ${INCLUDE_DIR}/insert/insert_blob.h
    ${INCLUDE_DIR}/insert/insert_blob_array.h
    ${INCLUDE_DIR}/insert/insert_invalid.h
    ${INCLUDE_DIR}/insert/insert_primitive.h
    ${INCLUDE_DIR}/insert/insert_primitive_array.h
    ${INCLUDE_DIR}/insert/insert_primitive_blob.h
    ${INCLUDE_DIR}/insert/insert_reference.h
    ${INCLUDE_DIR}/insert/insert_reference_array.h
    ${INCLUDE_DIR}/insert/insert_string.h
    ${INCLUDE_DIR}/insert/insert_string_array.h

    ${INCLUDE_DIR}/update/update_blob.h
    ${INCLUDE_DIR}/update/update_blob_array.h
    ${INCLUDE_DIR}/update/update_invalid.h
    ${INCLUDE_DIR}/update/update_primitive.h
    ${INCLUDE_DIR}/update/update_primitive_array.h
    ${INCLUDE_DIR}/update/update_primitive_blob.h
    ${INCLUDE_DIR}/update/update_reference.h
    ${INCLUDE_DIR}/update/update_reference_array.h
    ${INCLUDE_DIR}/update/update_string.h
    ${INCLUDE_DIR}/update/update_string_array.h
)

set(SOURCES
    ${SRC_DIR}/main.cpp

    ${SRC_DIR}/delete/delete_blob.cpp
    ${SRC_DIR}/delete/delete_blob_array.cpp
    ${SRC_DIR}/delete/delete_invalid.cpp
    ${SRC_DIR}/delete/delete_primitive.cpp
    ${SRC_DIR}/delete/delete_primitive_array.cpp
    ${SRC_DIR}/delete/delete_primitive_blob.cpp
    ${SRC_DIR}/delete/delete_reference.cpp
    ${SRC_DIR}/delete/delete_reference_array.cpp
    ${SRC_DIR}/delete/delete_string.cpp
    ${SRC_DIR}/delete/delete_string_array.cpp

    ${SRC_DIR}/get/get_blob.cpp
    ${SRC_DIR}/get/get_blob_array.cpp
    ${SRC_DIR}/get/get_invalid.cpp
    ${SRC_DIR}/get/get_primitive.cpp
    ${SRC_DIR}/get/get_primitive_array.cpp
    ${SRC_DIR}/get/get_primitive_blob.cpp
    ${SRC_DIR}/get/get_reference.cpp
    ${SRC_DIR}/get/get_reference_array.cpp
    ${SRC_DIR}/get/get_string.cpp
    ${SRC_DIR}/get/get_string_array.cpp

    ${SRC_DIR}/insert/insert_blob.cpp
    ${SRC_DIR}/insert/insert_blob_array.cpp
    ${SRC_DIR}/insert/insert_invalid.cpp
    ${SRC_DIR}/insert/insert_primitive.cpp
    ${SRC_DIR}/insert/insert_primitive_array.cpp
    ${SRC_DIR}/insert/insert_primitive_blob.cpp
    ${SRC_DIR}/insert/insert_reference.cpp
    ${SRC_DIR}/insert/insert_reference_array.cpp
    ${SRC_DIR}/insert/insert_string.cpp
    ${SRC_DIR}/insert/insert_string_array.cpp

    ${SRC_DIR}/update/update_blob.cpp
    ${SRC_DIR}/update/update_blob_array.cpp
    ${SRC_DIR}/update/update_invalid.cpp
    ${SRC_DIR}/update/update_primitive.cpp
    ${SRC_DIR}/update/update_primitive_array.cpp
    ${SRC_DIR}/update/update_primitive_blob.cpp
    ${SRC_DIR}/update/update_reference.cpp
    ${SRC_DIR}/update/update_reference_array.cpp
    ${SRC_DIR}/update/update_string.cpp
    ${SRC_DIR}/update/update_string_array.cpp
)

set(DEPS_PRIVATE
    alexandria_testutils
    alexandria-core
    alexandria-basic-query
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
