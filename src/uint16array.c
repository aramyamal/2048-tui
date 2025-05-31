#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
    uint16_t *items;
    size_t length;
    size_t capacity;
} UInt16Array;

UInt16Array UInt16Array_Init(const size_t length, const size_t capacity) {
    uint16_t *raw_array = calloc(capacity, sizeof(uint16_t));
    if (!raw_array) {
        return (UInt16Array){.items = NULL, .length = 0, .capacity = 0};
    }

    return (UInt16Array){
        .items = raw_array,
        .length = length,
        .capacity = capacity,
    };
}

void UInt16Array_Free(UInt16Array *array) {
    if (array && array->items) {
        free(array->items);
        array->items = NULL;
        array->length = 0;
        array->capacity = 0;
    }
}

uint16_t UInt16Array_Get(const UInt16Array *array, size_t index) {
    if (index < array->length) {
        return array->items[index];
    }
    return 0;
}

bool UInt16Array_Set(const UInt16Array *array, size_t index, uint16_t value) {
    if (index < array->length) {
        array->items[index] = value;
        return true;
    }
    return false;
}
