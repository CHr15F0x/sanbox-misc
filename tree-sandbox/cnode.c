#include "cnode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NODE_REALLOC_INCREMENT 4
#define NODE_DEBUG 0

#if NODE_DEBUG
static void node_dump(const char *func, int line, node_t *self)
{
    int i = 0;

    if (self == NULL)
    {
        printf("(nil)");
        return;
    }

    if (self->num_children > 0)
    {
        printf(
            "%s:%d (%p, %s, %p, %p, {",
            func,
            line,
            (void *)self,
            self->object_name,
            (void *)self->parent,
            (void *)self->children);

        for (; i < self->num_children; ++i)
        {
            printf("%p", (void *)self->children[i]);

            if (i < self->num_children - 1)
            {
                printf(",");
            }
        }

        printf(
            "}, %d, %d)\n",
            self->num_children,
            self->capacity);
    }
    else
    {
        printf(
            "%s:%d (%p, %s, %p, %p, {}, %d, %d)\n",
            func,
            line,
            (void *)self,
            self->object_name,
            (void *)self->parent,
            (void *)self->children,
            self->num_children,
            self->capacity);
    }
}

#define NODE_DUMP(NODE) node_dump(__func__, __LINE__, NODE)
#else
#define NODE_DUMP(NODE)
#endif /* NODE_DEBUG */

value_handlers_t *value_handlers_init(
    value_handlers_t *handlers,
    value_compare_fun_t compare_fun,
    value_to_json_fun_t to_json_fun,
    value_dispose_fun_t dispose_fun)
{
    if ((handlers == NULL) || (compare_fun == NULL))
    {
        return NULL;
    }

    /* dispose_fun & to_json_fun can be NULL */

    handlers->compare_fun = compare_fun;
    handlers->to_json_fun = to_json_fun;
    handlers->dispose_fun = dispose_fun;
    return handlers;
}

node_t *node_create(
    value_handlers_t *value_handlers,
    const char *node_name,
    void *value)
{
    if ((value_handlers == NULL) || (node_name == NULL) || (strlen(node_name) == 0))
    {
        return NULL;
    }

    /* value can be NULL */

    node_t *self = malloc(sizeof(node_t));

    if (self == NULL)
    {
        return NULL;
    }

    self->object_name = malloc(sizeof(char) * (strlen(node_name) + 1));
    strcpy(self->object_name, node_name);

    self->value = value;
    self->parent = NULL;
    self->children = NULL;
    self->num_children = 0;
    self->capacity = 0;
    self->value_handlers = value_handlers;

    NODE_DUMP(self);

    return self;
}

void node_dispose(node_t *self)
{
    if (self == NULL)
    {
        return;
    }

    NODE_DUMP(self);

    if (self->value_handlers->dispose_fun != NULL)
    {
        (*self->value_handlers->dispose_fun)(self->value);
    }

    free(self->object_name);
    free(self->children);
    free(self);
}

node_t *node_get_child(node_t *self, const char *child_name)
{
    int i = 0;

    if ((self == NULL) || (child_name == NULL))
    {
        return NULL;
    }

    NODE_DUMP(self);

    for (; i < self->num_children; ++i)
    {
        if (strcmp(self->children[i]->object_name, child_name) == 0)
        {
            return self->children[i];
        }
    }

    return NULL;
}

node_t *node_add_child(node_t *self, node_t *new_child)
{
    int i = 0;
    int cmp_result = 0;

    if ((self == NULL) || (new_child == NULL))
    {
        return NULL;
    }

    NODE_DUMP(self);

    if (node_get_child(self, new_child->object_name) != NULL)
    {
        return NULL;
    }

    if (self->capacity == self->num_children)
    {
        node_t **new_children = realloc(
            self->children,
            (self->capacity + NODE_REALLOC_INCREMENT) * sizeof(node_t *));

        /* Failed to allocate more memory */
        if (new_children == NULL)
        {
            return NULL;
        }

        self->children = new_children;
        self->capacity += NODE_REALLOC_INCREMENT;
    }

    /* find place for the new one */
    /* TODO use binary search version */
    for (; i < self->num_children; ++i)
    {
        cmp_result = self->value_handlers->compare_fun(new_child->value, self->children[i]->value);

        if (((cmp_result == 0) &&
             (strcmp(new_child->object_name, self->children[i]->object_name) < 0)) ||
            (cmp_result < 0))
        {
            /* found */
            break;
        }
    }

    /* shift all children beginning from the found one to the right */
    memmove(&self->children[i + 1], &self->children[i], (self->num_children - i) * sizeof(node_t *));

    /* put the new child where it belongs */
    self->children[i] = new_child;
    new_child->parent = self;
    ++self->num_children;

    NODE_DUMP(self);

    return new_child;
}

int node_remove_child(node_t *self, const char *child_name)
{
    int i = 0;
    int j = 0;

    if ((self == NULL) || (child_name == NULL) || (self->num_children == 0))
    {
        return 0;
    }

    NODE_DUMP(self);

    /* find it */
    for (; i < self->num_children; ++i)
    {
        if (strcmp(self->children[i]->object_name, child_name) == 0)
        {
            break;
        }
    }

    /* not found */
    if (i == self->num_children)
    {
        return 0;
    }

    /* deallocate child memory */
    node_dispose(self->children[i]);

    /*
     * memmove bug - cannot shift left
     * https://github.com/fingolfin/memmove-bug/blob/master/glibc-memcpy.patch
     */
    for (j = i; j < self->num_children; ++j)
    {
        self->children[j] = self->children[j + 1];
    }

    --self->num_children;

    NODE_DUMP(self);

    return 1;
}

#define ERROR_OR_NEXT()\
    if (char_cnt < 0)\
    {\
        return 0;\
    }\
    \
    if (char_cnt >= size_left)\
    {\
        return char_pos + char_cnt;\
    }\
    \
    char_pos += char_cnt;\
    size_left -= char_cnt;\

int node_to_json(node_t *self, char *buffer, int buffer_size)
{
    int i = 0;
    int char_pos = 0;
    int char_cnt = 0;
    int size_left = buffer_size;

    if ((self == NULL) || (buffer == NULL))
    {
        return 0;
    }

    char_cnt = snprintf(buffer, size_left, "{\"object_name\":\"%s\",\"value\":", self->object_name);

    ERROR_OR_NEXT();

    if (self->value_handlers->to_json_fun == NULL)
    {
        return 0;
    }

    char_cnt = self->value_handlers->to_json_fun(self->value, &buffer[char_pos], size_left);

    ERROR_OR_NEXT();

    if (self->parent == NULL)
    {
        char_cnt = snprintf(&buffer[char_pos], size_left, ",\"parent\":null,\"children\":[");
    }
    else
    {
        char_cnt = snprintf(
            &buffer[char_pos],
            size_left,
            ",\"parent\":\"%s\",\"children\":[",
            self->parent->object_name);
    }

    ERROR_OR_NEXT();

    for (; i < self->num_children; ++i)
    {
        char_cnt = snprintf(&buffer[char_pos], size_left, "\"%s\",", self->children[i]->object_name);

        ERROR_OR_NEXT();

        if (i == self->num_children - 1)
        {
            /* remove last comma */
            --char_pos;
            ++size_left;
        }
    }

    char_cnt = snprintf(&buffer[char_pos], size_left, "]}");

    ERROR_OR_NEXT();

    return char_pos;
}
