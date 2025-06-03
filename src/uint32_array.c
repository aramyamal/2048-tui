#ifndef UINT32_ARRAY_C
#define UINT32_ARRAY_C

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    uint32_t *items;
    size_t length;
    size_t capacity;
} UInt32Array;

static inline UInt32Array UInt32Array_null() {
    return (UInt32Array){.items = NULL, .length = 0, .capacity = 0};
}

UInt32Array UInt32Array_create(const size_t length, const size_t capacity) {

    if (length > capacity || capacity == 0) {
        return UInt32Array_null();
    }

    uint32_t *raw_array = calloc(capacity, sizeof(uint32_t));
    if (!raw_array) {
        return UInt32Array_null();
    }

    return (UInt32Array){
        .items = raw_array,
        .length = length,
        .capacity = capacity,
    };
}

void UInt32Array_destroy(UInt32Array *array) {
    if (array && array->items) {
        free(array->items);
        array->items = NULL;
        array->length = 0;
        array->capacity = 0;
    }
}

uint32_t UInt32Array_get(const UInt32Array array, size_t index) {
    if (index < array.length && array.items) {
        return array.items[index];
    }
    return UINT32_MAX;
}

bool UInt32Array_set(UInt32Array *array, size_t index, uint32_t value) {
    if (!array || !array->items || index >= array->length) {
        return false;
    }
    array->items[index] = value;
    return true;
}

UInt32Array UInt32Array_copy(const UInt32Array array) {
    if (!array.items || array.capacity == 0) {
        return UInt32Array_null();
    }

    uint32_t *raw_array_copy = calloc(array.capacity, sizeof(uint32_t));
    if (!raw_array_copy) {
        return UInt32Array_null();
    }

    if (array.length > 0) {
        memcpy(raw_array_copy, array.items, array.length * sizeof(uint32_t));
    }

    return (UInt32Array){
        .items = raw_array_copy,
        .length = array.length,
        .capacity = array.capacity,
    };
}

#endif // UINT32_ARRAY_C
