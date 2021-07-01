#ifndef TEST_HELPERS_H_
#define TEST_HELPERS_H_

#include <gtest/gtest.h>
#include "cnode.h"

#pragma GCC diagnostic ignored "-Wnonnull"

int int_compare(void *a, void *b);
int int_to_json(void *value, char *buffer, int buffer_size);
void int_dispose(void *);

extern value_handlers_t g_handlers;
extern value_handlers_t *INT;
extern int _1;
extern int _2;
extern int _3;
extern int _4;
extern int _5;
extern int _6;
extern int _7;
extern int _8;
extern int _9;
extern int _10;
extern int _123;

#define EXPECT_NODE(NODE_PTR, NAME, PARENT, CHILDREN, NUM_CHILDREN, CAPACITY, VALUE)\
    EXPECT_STREQ((NAME), (NODE_PTR)->object_name);\
    EXPECT_EQ((PARENT), (NODE_PTR)->parent);\
    EXPECT_EQ((NUM_CHILDREN), (NODE_PTR)->num_children);\
    EXPECT_EQ((CAPACITY), (NODE_PTR)->capacity);\
    if ((NUM_CHILDREN) > 0)\
    {\
        EXPECT_EQ(0, memcmp((CHILDREN), (NODE_PTR)->children, sizeof(node_t *) * (NUM_CHILDREN)));\
    }\
    else if ((CAPACITY) == 0)\
    {\
        EXPECT_EQ((CHILDREN), (NODE_PTR)->children);\
    }\
    EXPECT_EQ((VALUE), (NODE_PTR)->value)

#define EXPECT_TREE(TREE_PTR, ROOT, NODES, NUM_NODES, CAPACITY)\
    EXPECT_EQ((ROOT), (TREE_PTR)->root);\
    EXPECT_EQ((NUM_NODES), (TREE_PTR)->num_nodes);\
    EXPECT_EQ((CAPACITY), (TREE_PTR)->capacity);\
    if ((NUM_NODES) > 0)\
    {\
        EXPECT_EQ(0, memcmp((NODES), (TREE_PTR)->nodes, sizeof(node_t *) * (NUM_NODES)));\
    }\
    else if ((CAPACITY) == 0)\
    {\
        EXPECT_EQ((NODES), (TREE_PTR)->nodes);\
    }

#endif /* TEST_HELPERS_H_ */
