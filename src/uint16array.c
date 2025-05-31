#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
    uint16_t *items;
    size_t length;
    size_t capacity;
} UInt16Array;

UInt16Array *UInt16Array_Init(const size_t length, const size_t capacity) {
    UInt16Array *array = malloc(sizeof(UInt16Array));
    if (!array)
        return NULL;

    array->items = calloc(capacity, sizeof(uint16_t));
    if (!array->items) {
        free(array);
        return NULL;
    }

    array->length = length;
    array->capacity = capacity;

    return array;
}

void UInt16Array_Free(UInt16Array *array) {
    if (array) {
        free(array->items);
        free(array);
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
