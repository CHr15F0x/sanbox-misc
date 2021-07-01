#include "test-helpers.h"

int int_compare(void *a, void *b)
{
    int a_ = *(static_cast<int *>(a));
    int b_ = *(static_cast<int *>(b));

    if (a_ < b_)
    {
        return -1;
    }

    if (a_ == b_)
    {
        return 0;
    }

    return 1;
}

int int_to_json(void *value, char *buffer, int buffer_size)
{
    if (buffer == NULL)
    {
        return -1;
    }

    if (value == NULL)
    {
        return snprintf(buffer, buffer_size, "null");
    }

    return snprintf(buffer, buffer_size, "%d", *(static_cast<int *>(value)));
}

void int_dispose(void *)
{
    /* does nothing */
}

value_handlers_t g_handlers;
value_handlers_t *INT = value_handlers_init(&g_handlers, int_compare, int_to_json, int_dispose);
int _1 = 1;
int _2 = 2;
int _3 = 3;
int _4 = 4;
int _5 = 5;
int _6 = 6;
int _7 = 7;
int _8 = 8;
int _9 = 9;
int _10 = 10;
int _123 = 123;

