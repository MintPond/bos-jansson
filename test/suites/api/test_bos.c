/*
 * Copyright (c) 2018 JCThePants <github.com/JCThePants>
 *
 * Bos-Jansson is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See LICENSE for details.
 */

#include <stdint.h>
#include <string.h>
#include <bosjansson.h>
#include "util.h"

static void read(void *destination, bos_t *serialized, size_t pos, size_t size) {
    memcpy(destination, serialized->data + pos, size);
}

/*** serialize->deserialize tests ***/

static void test_object_header(bos_t *serialized) {

    uint32_t size;
    uint8_t obj_dataType;
    uint8_t obj_keyLen;

    read(&size, serialized, 0, sizeof(uint32_t));
    if (size != serialized->size)
        fail("Serialized size does not match returned size");

    /* should have correct data type (OBJ) */
    read(&obj_dataType, serialized, 4, sizeof(uint8_t));
    if (obj_dataType != 0x0F/*OBJ*/)
        fail("Incorrect object data type");

    /* should have correct object key length*/
    read(&obj_keyLen, serialized, 5, sizeof(uint8_t));
    if (obj_keyLen != 12)
        fail("Incorrect number of object keys");
}

/* deserialized object key tests */

static void test_key_bool(json_t *object) {

    json_t *key = json_object_get(object, "bool");
    if (key == NULL)
        fail("deserialized object missing 'bool' key");

    if (!json_is_boolean(key))
        fail("deserialized 'bool' value should be boolean");

    if (!json_boolean_value(key))
        fail("deserialized 'bool' key has incorrect value");
}

static void test_key_int8(json_t *object) {

    json_t *key = json_object_get(object, "int8");
    if (key == NULL)
        fail("deserialized object missing 'int8' key");

    if (!json_is_integer(key))
        fail("deserialized 'int8' value should be integer");

    if (json_integer_value(key) != -1)
        fail("deserialized 'int8' key has incorrect value");
}

static void test_key_int16(json_t *object) {

    json_t *key = json_object_get(object, "int16");
    if (key == NULL)
        fail("deserialized object missing 'int16' key");

    if (!json_is_integer(key))
        fail("deserialized 'int16' value should be integer");

    if (json_integer_value(key) != -300)
        fail("deserialized 'int16' key has incorrect value");
}

static void test_key_int32(json_t *object) {

    json_t *key = json_object_get(object, "int32");
    if (key == NULL)
        fail("deserialized object missing 'int32' key");

    if (!json_is_integer(key))
        fail("deserialized 'int32' value should be integer");

    if (json_integer_value(key) != -2147483640)
        fail("deserialized 'int32' key has incorrect value");
}

static void test_key_uint8(json_t *object) {

    json_t *key = json_object_get(object, "uint8");
    if (key == NULL)
        fail("deserialized object missing 'uint8' key");

    if (!json_is_integer(key))
        fail("deserialized 'uint8' value should be integer");

    if (json_integer_value(key) != 254)
        fail("deserialized 'uint8' key has incorrect value");
}

static void test_key_uint16(json_t *object) {

    json_t *key = json_object_get(object, "uint16");
    if (key == NULL)
        fail("deserialized object missing 'uint16' key");

    if (!json_is_integer(key))
        fail("deserialized 'uint16' value should be integer");

    if (json_integer_value(key) != 4000)
        fail("deserialized 'uint16' key has incorrect value");
}

static void test_key_uint32(json_t *object) {

    json_t *key = json_object_get(object, "uint32");
    if (key == NULL)
        fail("deserialized object missing 'uint32' key");

    if (!json_is_integer(key))
        fail("deserialized 'uint32' value should be integer");

    if (json_integer_value(key) != 4294967290)
        fail("deserialized 'uint32' key has incorrect value");
}

static void test_key_float(json_t *object) {

    double zero;
    json_t *key = json_object_get(object, "float");
    if (key == NULL)
        fail("deserialized object missing 'float' key");

    if (!json_is_real(key))
        fail("deserialized 'float' value should be real");

    zero = json_real_value(key) - 5.5;
    if (zero > 0.000001 || zero < -0.000001)
        fail("deserialized 'float' key has incorrect value");
}

static void test_key_string(json_t *object) {

    json_t *key = json_object_get(object, "string");
    if (key == NULL)
        fail("deserialized object missing 'string' key");

    if (!json_is_string(key))
        fail("deserialized 'string' value should be string");

    if (strcmp(json_string_value(key), "this is a string") != 0)
        fail("deserialized 'string' key has incorrect value");
}

static void test_key_bytes(json_t *object) {

    json_t *key = json_object_get(object, "bytes");
    size_t len;
    void *value;

    if (key == NULL)
        fail("deserialized object missing 'bytes' key");

    if (!json_is_bytes(key))
        fail("deserialized 'bytes' value should be bytes");

    value = (void *)json_bytes_value(key);
    len = json_bytes_size(key);

    if (len != 2)
        fail("deserialized 'bytes' key has incorrect number of bytes");

    if (*(uint8_t *)value != 1)
        fail("deserialized 'bytes' key has incorrect value in [0]");

    if (*((uint8_t *)value + 1) != 1)
        fail("deserialized 'bytes' key has incorrect value in [1]");
}

static void test_key_array(json_t *object) {

    json_t *key = json_object_get(object, "array");
    size_t len;
    json_t *index0;
    json_t *index1;
    json_t *index2;
    json_t *index3;
    double real;

    if (key == NULL)
        fail("deserialized object missing 'array' key");

    if (!json_is_array(key))
        fail("deserialized 'array' value should be array");

    len = json_array_size(key);
    if (len != 4)
        fail("deserialized 'array' key has incorrect number of elements");

    /* index 0 */

    index0 = json_array_get(key, 0);
    if (index0 == NULL)
        fail("deserialized object array missing index 0 value");

    if (!json_is_string(index0))
        fail("deserialized array index 0 value should be string");

    if (strcmp(json_string_value(index0), "string") != 0)
        fail("deserialized array index 0 has incorrect value");

    /* index 1 */

    index1 = json_array_get(key, 1);
    if (index1 == NULL)
        fail("deserialized object array missing index 1 value");

    if (!json_is_integer(index1))
        fail("deserialized array index 1 value should be integer");

    if (json_integer_value(index1) != 1)
        fail("deserialized array index 1 has incorrect value");

    /* index 2 */

    index2 = json_array_get(key, 2);
    if (index2 == NULL)
        fail("deserialized object array missing index 2 value");

    if (!json_is_real(index2))
        fail("deserialized array index 2 value should be real");

    real = json_real_value(index2) - 2.2;
    if (real > 0.000001 || real < -0.000001)
        fail("deserialized array index 2 has incorrect value");

    /* index 3 */

    index3 = json_array_get(key, 3);
    if (index3 == NULL)
        fail("deserialized object array missing index 3 value");

    if (!json_is_boolean(index3))
        fail("deserialized array index 3 value should be boolean");

    if (json_boolean_value(index3))
        fail("deserialized array index 3 has incorrect value");
}

static void test_key_object(json_t *object) {

    json_t *key = json_object_get(object, "obj");
    size_t len;
    json_t *str_key;
    json_t *int_key;
    json_t *float_key;
    json_t *bool_key;
    double real;

    if (key == NULL)
        fail("deserialized object missing 'obj' key");

    if (!json_is_object(key))
        fail("deserialized 'obj' value should be object");

    len = json_object_size(key);
    if (len != 4)
        fail("deserialized 'obj' key has incorrect number of properties");

    /* str key */

    str_key = json_object_get(key, "str");
    if (str_key == NULL)
        fail("deserialized object 'obj' key missing 'str' key");

    if (!json_is_string(str_key))
        fail("deserialized object 'obj' key 'str' value should be string");

    if (strcmp(json_string_value(str_key), "str") != 0)
        fail("deserialized object 'obj' key 'str' has incorrect value");

    /* int key */

    int_key = json_object_get(key, "int");
    if (int_key == NULL)
        fail("deserialized object 'obj' key missing 'int' key");

    if (!json_is_integer(int_key))
        fail("deserialized object 'obj' key 'int' value should be integer");

    if (json_integer_value(int_key) != 1)
        fail("deserialized object 'obj' key 'int' has incorrect value");

    /* float key */

    float_key = json_object_get(key, "float");
    if (float_key == NULL)
        fail("deserialized object 'obj' key missing 'float' key");

    if (!json_is_real(float_key))
        fail("deserialized object 'obj' key 'float' value should be real");

    real = json_real_value(float_key) - 2.3;
    if (real > 0.000001 || real < -0.000001)
        fail("deserialized object 'obj' key 'float' has incorrect value");

    /* bool key */

    bool_key = json_object_get(key, "bool");
    if (bool_key == NULL)
        fail("deserialized object 'obj' key missing 'bool' key");

    if (!json_is_boolean(bool_key))
        fail("deserialized object 'obj' key 'bool' value should be boolean");

    if (!json_boolean_value(bool_key))
        fail("deserialized object 'obj' key 'bool' has incorrect value");
}


static void test_deserialize(bos_t *serialized) {

    size_t key_count;
    json_error_t *error = (json_error_t *)malloc(sizeof(json_error_t));

    json_t *object = bos_deserialize(serialized->data, error);
    if (object == NULL) {
        fail("deserialize failed");
        return;
    }

    key_count = json_object_size(object);
    if (key_count != 12)
        fail("deserialized incorrect key count");

    test_key_bool(object);
    test_key_int8(object);
    test_key_int16(object);
    test_key_int32(object);
    test_key_uint8(object);
    test_key_uint16(object);
    test_key_uint32(object);
    test_key_float(object);
    test_key_string(object);
    test_key_bytes(object);
    test_key_array(object);
    test_key_object(object);

    json_decref(object);
    free(error);
}

static void test_serialize_deserialize(void) {

    json_error_t *error = (json_error_t *)malloc(sizeof(json_error_t));
    json_t *object = json_object();
    json_t *array = json_array();
    json_t *inner_object = json_object();
    bos_t *serialized;
    void *bytes = malloc(2);
    memset(bytes, 1, 2);

    json_array_append_new(array, json_string("string"));
    json_array_append_new(array, json_integer(1));
    json_array_append_new(array, json_real(2.2));
    json_array_append_new(array, json_boolean(0));

    json_object_set_new(inner_object, "str", json_string("str"));
    json_object_set_new(inner_object, "int", json_integer(1));
    json_object_set_new(inner_object, "float", json_real(2.3));
    json_object_set_new(inner_object, "bool", json_boolean(1));

    json_object_set_new(object, "bool", json_boolean(1));
    json_object_set_new(object, "int8", json_integer(-1));
    json_object_set_new(object, "int16", json_integer(-300));
    json_object_set_new(object, "int32", json_integer(-2147483640));
    json_object_set_new(object, "uint8", json_integer(254));
    json_object_set_new(object, "uint16", json_integer(4000));
    json_object_set_new(object, "uint32", json_integer(4294967290));
    json_object_set_new(object, "float", json_real(5.5));
    json_object_set_new(object, "string", json_string("this is a string"));
    json_object_set_new(object, "bytes", json_bytes(bytes, 2));
    json_object_set(object, "array", array);
    json_object_set(object, "obj", inner_object);

    serialized = bos_serialize(object, error);
    if (serialized == NULL) {
        fail("serialize failed");
        return;
    }

    json_decref(object);
    json_decref(array);
    json_decref(inner_object);

    test_object_header(serialized);
    test_deserialize(serialized);

    bos_free(serialized);
    free(bytes);
    free(error);
}

/*** BOS format tests ***/

static void test_format_null() {

    json_t *root = json_null();
    json_error_t *error = (json_error_t *)malloc(sizeof(json_error_t));
    bos_t *serialized = bos_serialize(root, error);
    uint32_t data_size;
    uint8_t data_type;

    read(&data_size, serialized, 0, sizeof(uint32_t));
    if (data_size != 5)
        fail("format null: incorrect data size");

    read(&data_type, serialized, 4, sizeof(uint8_t));
    if (data_type != 0x00)
        fail("format null: incorrect data type");

    json_decref(root);
    bos_free(serialized);
    free(error);
}

static void test_format_boolean(unsigned int value) {

    uint32_t data_size;
    uint8_t data_type;
    uint8_t data_value;
    json_t *root = json_boolean(value);
    json_error_t *error = (json_error_t *)malloc(sizeof(json_error_t));
    bos_t *serialized = bos_serialize(root, error);

    read(&data_size, serialized, 0, sizeof(uint32_t));
    if (data_size != 6)
        fail("format boolean: incorrect data size");

    read(&data_type, serialized, 4, sizeof(uint8_t));
    if (data_type != 0x01)
        fail("format boolean: incorrect data type");

    read(&data_value, serialized, 5, sizeof(uint8_t));
    if (data_value != value)
        fail("format boolean: incorrect data value");

    json_decref(root);
    bos_free(serialized);
    free(error);
}

static void test_format_int8() {

    uint32_t data_size;
    uint8_t data_type;
    int8_t data_value;
    json_t *root = json_integer(-1);
    json_error_t *error = (json_error_t *)malloc(sizeof(json_error_t));
    bos_t *serialized = bos_serialize(root, error);

    read(&data_size, serialized, 0, sizeof(uint32_t));
    if (data_size != 6)
        fail("format int8: incorrect data size");

    read(&data_type, serialized, 4, sizeof(uint8_t));
    if (data_type != 0x02)
        fail("format int8: incorrect data type");

    read(&data_value, serialized, 5, sizeof(int8_t));
    if (data_value != -1)
        fail("format int8: incorrect data value");

    json_decref(root);
    bos_free(serialized);
    free(error);
}

static void test_format_int16() {

    uint32_t data_size;
    uint8_t data_type;
    int16_t data_value;
    json_t *root = json_integer(-300);
    json_error_t *error = (json_error_t *)malloc(sizeof(json_error_t));
    bos_t *serialized = bos_serialize(root, error);

    read(&data_size, serialized, 0, sizeof(uint32_t));
    if (data_size != 7)
        fail("format int16: incorrect data size");

    read(&data_type, serialized, 4, sizeof(uint8_t));
    if (data_type != 0x03)
        fail("format int16: incorrect data type");

    read(&data_value, serialized, 5, sizeof(int16_t));
    if (data_value != -300)
        fail("format int16: incorrect data value");

    json_decref(root);
    bos_free(serialized);
    free(error);
}

static void test_format_int32() {

    uint32_t data_size;
    uint8_t data_type;
    int32_t data_value;
    json_t *root = json_integer(-2147483640);
    json_error_t *error = (json_error_t *)malloc(sizeof(json_error_t));
    bos_t *serialized = bos_serialize(root, error);

    read(&data_size, serialized, 0, sizeof(uint32_t));
    if (data_size != 9)
        fail("format int32: incorrect data size");

    read(&data_type, serialized, 4, sizeof(uint8_t));
    if (data_type != 0x04)
        fail("format int32: incorrect data type");

    read(&data_value, serialized, 5, sizeof(int32_t));
    if (data_value != -2147483640)
        fail("format int32: incorrect data value");

    json_decref(root);
    bos_free(serialized);
    free(error);
}

static void test_format_int64() {

    uint32_t data_size;
    uint8_t data_type;
    int64_t data_value;
    json_t *root = json_integer(-1099511627775);
    json_error_t *error = (json_error_t *)malloc(sizeof(json_error_t));
    bos_t *serialized = bos_serialize(root, error);

    read(&data_size, serialized, 0, sizeof(uint32_t));
    if (data_size != 13)
        fail("format int64: incorrect data size");

    read(&data_type, serialized, 4, sizeof(uint8_t));
    if (data_type != 0x05)
        fail("format int64: incorrect data type");

    read(&data_value, serialized, 5, sizeof(int64_t));
    if (data_value != -1099511627775)
        fail("format int64: incorrect data value");

    json_decref(root);
    bos_free(serialized);
    free(error);
}

static void test_format_uint8() {

    uint32_t data_size;
    uint8_t data_type;
    uint8_t data_value;
    json_t *root = json_integer(255);
    json_error_t *error = (json_error_t *)malloc(sizeof(json_error_t));
    bos_t *serialized = bos_serialize(root, error);

    read(&data_size, serialized, 0, sizeof(uint32_t));
    if (data_size != 6)
        fail("format uint8: incorrect data size");

    read(&data_type, serialized, 4, sizeof(uint8_t));
    if (data_type != 0x06)
        fail("format uint8: incorrect data type");

    read(&data_value, serialized, 5, sizeof(uint8_t));
    if (data_value != 255)
        fail("format uint8: incorrect data value");

    json_decref(root);
    bos_free(serialized);
    free(error);
}

static void test_format_uint16() {

    uint32_t data_size;
    uint8_t data_type;
    uint16_t data_value;
    json_t *root = json_integer(4000);
    json_error_t *error = (json_error_t *)malloc(sizeof(json_error_t));
    bos_t *serialized = bos_serialize(root, error);

    read(&data_size, serialized, 0, sizeof(uint32_t));
    if (data_size != 7)
        fail("format uint16: incorrect data size");

    read(&data_type, serialized, 4, sizeof(uint8_t));
    if (data_type != 0x07)
        fail("format uint16: incorrect data type");

    read(&data_value, serialized, 5, sizeof(uint16_t));
    if (data_value != 4000)
        fail("format uint16: incorrect data value");

    json_decref(root);
    bos_free(serialized);
    free(error);
}

static void test_format_uint32() {

    uint32_t data_size;
    uint8_t data_type;
    uint32_t data_value;
    json_t *root = json_integer(4294967290);
    json_error_t *error = (json_error_t *)malloc(sizeof(json_error_t));
    bos_t *serialized = bos_serialize(root, error);

    read(&data_size, serialized, 0, sizeof(uint32_t));
    if (data_size != 9)
        fail("format uint32: incorrect data size");

    read(&data_type, serialized, 4, sizeof(uint8_t));
    if (data_type != 0x08)
        fail("format uint32: incorrect data type");

    read(&data_value, serialized, 5, sizeof(uint32_t));
    if (data_value != 4294967290)
        fail("format uint32: incorrect data value");

    json_decref(root);
    bos_free(serialized);
    free(error);
}

static void test_format_uint64() {

    uint32_t data_size;
    uint8_t data_type;
    uint64_t data_value;
    json_t *root = json_integer(1099511627775);
    json_error_t *error = (json_error_t *)malloc(sizeof(json_error_t));
    bos_t *serialized = bos_serialize(root, error);

    read(&data_size, serialized, 0, sizeof(uint32_t));
    if (data_size != 13)
        fail("format uint64: incorrect data size");

    read(&data_type, serialized, 4, sizeof(uint8_t));
    if (data_type != 0x09)
        fail("format uint64: incorrect data type");

    read(&data_value, serialized, 5, sizeof(uint64_t));
    if (data_value != 1099511627775)
        fail("format uint64: incorrect data value");

    json_decref(root);
    bos_free(serialized);
    free(error);
}

static void test_format_float() {

    uint32_t data_size;
    uint8_t data_type;
    float data_value;
    json_t *root = json_real(5.5);
    json_error_t *error = (json_error_t *)malloc(sizeof(json_error_t));
    bos_t *serialized = bos_serialize(root, error);

    read(&data_size, serialized, 0, sizeof(uint32_t));
    if (data_size != 9)
        fail("format float: incorrect data size");

    read(&data_type, serialized, 4, sizeof(uint8_t));
    if (data_type != 0x0A)
        fail("format float: incorrect data type");

    read(&data_value, serialized, 5, sizeof(float));
    data_value -= 5.5;
    if (data_value > 0.000001 || data_value < -0.000001)
        fail("format float: incorrect data value");

    json_decref(root);
    bos_free(serialized);
    free(error);
}

static void test_format_string() {

    uint32_t data_size;
    uint8_t data_type;
    uint8_t str_len;
    char *data_value = (char *)malloc(strlen("string") + 1);
    json_t *root = json_string("string");
    json_error_t *error = (json_error_t *)malloc(sizeof(json_error_t));
    bos_t *serialized = bos_serialize(root, error);

    read(&data_size, serialized, 0, sizeof(uint32_t));
    if (data_size != 12)
        fail("format string: incorrect data size");

    read(&data_type, serialized, 4, sizeof(uint8_t));
    if (data_type != 0x0C)
        fail("format string: incorrect data type");

    read(&str_len, serialized, 5, sizeof(uint8_t));
    if (str_len != strlen("string"))
        fail("format string: incorrect string length");

    read(data_value, serialized, 6, strlen("string"));
    memset(data_value + strlen("string") + 1, (char)0, sizeof(char));
    if (strcmp(data_value, "string") != 0)
        fail("format string: incorrect data value");

    json_decref(root);
    bos_free(serialized);
    free(error);
    free(data_value);
}

static void test_format_bytes() {

    void *bytes = malloc(300);
    uint32_t data_size;
    uint8_t data_type;
    uint8_t bytes_size_var;
    uint16_t bytes_size;
    void *data_value = malloc(300);
    json_t *root = json_bytes(bytes, 300);
    json_error_t *error = (json_error_t *)malloc(sizeof(json_error_t));
    bos_t *serialized;

    memset(bytes, (uint8_t)1, sizeof(uint8_t));
    memset(bytes + 1, (uint8_t)3, sizeof(uint8_t));
    serialized = bos_serialize(root, error);

    read(&data_size, serialized, 0, sizeof(uint32_t));
    if (data_size != 308)
        fail("format bytes: incorrect data size");

    read(&data_type, serialized, 4, sizeof(uint8_t));
    if (data_type != 0x0D)
        fail("format bytes: incorrect data type");

    read(&bytes_size_var, serialized, 5, sizeof(uint8_t));
    if (bytes_size_var != 0xFD)
        fail("format bytes: var length data size prefix is incorrect");

    read(&bytes_size, serialized, 6, sizeof(uint16_t));
    if (bytes_size != 300)
        fail("format bytes: data size is incorrect");

    read(data_value, serialized, 8, 300);

    if (memcmp(data_value, bytes, 300) != 0)
        fail("format bytes: incorrect data value");

    json_decref(root);
    bos_free(serialized);
    free(error);
    free(data_value);
}

static void test_format_array() {

    uint32_t data_size;
    uint8_t data_type;
    uint8_t array_count;
    uint8_t elem0_data_type;
    uint8_t elem0_value;
    json_t *root = json_array();
    json_t *elem1 = json_integer(1);
    json_error_t *error = (json_error_t *)malloc(sizeof(json_error_t));
    bos_t *serialized;

    json_array_append_new(root, elem1);
    serialized = bos_serialize(root, error);

    read(&data_size, serialized, 0, sizeof(uint32_t));
    if (data_size != 8)
        fail("format array: incorrect data size");

    read(&data_type, serialized, 4, sizeof(uint8_t));
    if (data_type != 0x0E)
        fail("format array: incorrect data type");

    read(&array_count, serialized, 5, sizeof(uint8_t));
    if (array_count != 1)
        fail("format array: count is incorrect");

    read(&elem0_data_type, serialized, 6, sizeof(uint8_t));
    if (elem0_data_type != 0x06)
        fail("format array: element 0 data type is incorrect");

    read(&elem0_value, serialized, 7, sizeof(uint8_t));
    if (elem0_value != 1)
        fail("format array: element 0 value is incorrect");

    json_decref(root);
    bos_free(serialized);
    free(error);
}

static void test_format_obj() {

    uint32_t data_size;
    uint8_t data_type;
    uint8_t key_count;
    uint8_t key0_name_len;
    char *key0_name = malloc(strlen("key0") + 1);
    uint8_t key0_data_type;
    uint8_t key0_value;

    json_t *root = json_object();
    json_t *key0 = json_integer(1);
    json_error_t *error = (json_error_t *)malloc(sizeof(json_error_t));
    bos_t *serialized;

    json_object_set(root, "key0", key0);
    serialized = bos_serialize(root, error);

    read(&data_size, serialized, 0, sizeof(uint32_t));
    if (data_size != 13)
        fail("format object: incorrect data size");

    read(&data_type, serialized, 4, sizeof(uint8_t));
    if (data_type != 0x0F)
        fail("format object: incorrect data type");

    read(&key_count, serialized, 5, sizeof(uint8_t));
    if (key_count != 1)
        fail("format object: key count is incorrect");

    read(&key0_name_len, serialized, 6, sizeof(uint8_t));
    if (key0_name_len != strlen("key0"))
        fail("format object: key0 name length is incorrect");

    read(key0_name, serialized, 7, strlen("key0"));
    memset(key0_name + strlen("key0") + 1, (char)0, sizeof(char));
    if (strcmp(key0_name, "key0") != 0)
        fail("format object: key0 name is incorrect");

    read(&key0_data_type, serialized, 11, sizeof(uint8_t));
    if (key0_data_type != 0x06)
        fail("format object: key0 data type is incorrect");

    read(&key0_value, serialized, 12, sizeof(uint8_t));
    if (key0_value != 1)
        fail("format object: key0 value is incorrect");

    json_decref(root);
    bos_free(serialized);
    free(error);
    free(key0_name);
}

static void run_tests()
{
    test_serialize_deserialize();
    test_format_null();
    test_format_boolean(0);
    test_format_boolean(1);
    test_format_int8();
    test_format_int16();
    test_format_int32();
    test_format_int64();
    test_format_uint8();
    test_format_uint16();
    test_format_uint32();
    test_format_uint64();
    test_format_float();
    test_format_string();
    test_format_bytes();
    test_format_array();
    test_format_obj();
}
