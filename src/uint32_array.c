#ifndef UINT32_ARRAY_C
#define UINT32_ARRAY_C

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
    uint32_t *items;
    size_t length;
    size_t capacity;
} UInt32Array;

UInt32Array UInt32Array_create(const size_t length, const size_t capacity) {
    uint32_t *raw_array = calloc(capacity, sizeof(uint32_t));
    if (!raw_array) {
        return (UInt32Array){.items = NULL, .length = 0, .capacity = 0};
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
    if (index < array.length) {
        return array.items[index];
    }
    return 0;
}

bool UInt32Array_set(const UInt32Array *array, size_t index, uint32_t value) {
    if (!array || !array->items || index >= array->length) {
        return false;
    }
    array->items[index] = value;
    return true;
}

UInt32Array UInt32Array_copy(UInt32Array array) {
    size_t length = array.length;
    size_t capacity = array.capacity;
    uint32_t *raw_array_copy = calloc(capacity, sizeof(uint32_t));
    if (!raw_array_copy) {
        return (UInt32Array){.items = NULL, .length = 0, .capacity = 0};
    }
    for (size_t i = 0; i < length; ++i) {
        raw_array_copy[i] = UInt32Array_get(array, i);
    }
    return (UInt32Array){
        .items = raw_array_copy,
        .length = length,
        .capacity = capacity,
    };
}

#endif // UINT32_ARRAY_C
