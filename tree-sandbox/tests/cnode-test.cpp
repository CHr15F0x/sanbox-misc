#include <gtest/gtest.h>
#include "cnode.h"
#include "test-helpers.h"

TEST(value_handlers_t, value_handlers_init__on_null_structure_returns_null)
{
    EXPECT_EQ(NULL, value_handlers_init(NULL, int_compare, int_to_json, int_dispose));
}

TEST(value_handlers_t, value_handlers_init__with_null_compare_function_returns_null)
{
    value_handlers_t h;
    EXPECT_EQ(NULL, value_handlers_init(&h, NULL, int_to_json, int_dispose));
}

TEST(value_handlers_t, value_handlers_init__dispose_function_and_to_json_function_can_be_null)
{
    value_handlers_t h;
    EXPECT_EQ(&h, value_handlers_init(&h, int_compare, NULL, NULL));
    EXPECT_EQ(&int_compare, h.compare_fun);
    EXPECT_EQ(NULL, h.to_json_fun);
    EXPECT_EQ(NULL, h.dispose_fun);
}

TEST(node_t, node_create__with_null_handlers_returns_null)
{
    EXPECT_EQ(NULL, node_create(NULL, "node", &_1));
}

TEST(node_t, node_create__with_null_name_returns_null)
{
    EXPECT_EQ(NULL, node_create(INT, NULL, &_1));
}

TEST(node_t, node_create__with_empty_name_returns_null)
{
    EXPECT_EQ(NULL, node_create(INT, "", &_1));
}

TEST(node_t, node_create__with_null_value_is_ok)
{
    node_t *node = node_create(INT, "node", NULL);

    EXPECT_NODE(node, "node", NULL, NULL, 0, 0, NULL);

    node_dispose(node);
}

TEST(node_t, node_dispose__on_null_is_safe)
{
    EXPECT_NO_FATAL_FAILURE(node_dispose(NULL));
}

TEST(node_t, node_get_child__when_self_is_null_returns_null)
{
    EXPECT_EQ(NULL, node_get_child(NULL, "child"));
}

TEST(node_t, node_get_child__with_null_name_returns_null)
{
    node_t *node = node_create(INT, "node", NULL);
    EXPECT_EQ(NULL, node_get_child(node, NULL));

    node_dispose(node);
}

TEST(node_t, node_get_child__from_childless_node_returns_null)
{
    node_t *node = node_create(INT, "node", NULL);
    EXPECT_EQ(NULL, node_get_child(node, "i don't exist"));

    node_dispose(node);
}

TEST(node_t, node_add_child__every_child_is_appended)
{
    /* Prepare */
    node_t *a = node_create(INT, "a", &_1);
    node_t *b = node_create(INT, "b", &_2);
    node_t *c = node_create(INT, "c", &_3);
    node_t *d = node_create(INT, "d", &_4);
    node_t *expected_order[] =
    {
        b, c, d
    };
    /* Run */
    EXPECT_EQ(b, node_add_child(a, b));
    EXPECT_EQ(c, node_add_child(a, c));
    EXPECT_EQ(d, node_add_child(a, d));
    /* Check a */
    EXPECT_NODE(a, "a", NULL, expected_order, 3, 4, &_1);
    EXPECT_EQ(d, node_get_child(a, "d"));
    EXPECT_EQ(c, node_get_child(a, "c"));
    EXPECT_EQ(b, node_get_child(a, "b"));
    /* Check others */
    EXPECT_NODE(b, "b", a, NULL, 0, 0, &_2);
    EXPECT_NODE(c, "c", a, NULL, 0, 0, &_3);
    EXPECT_NODE(d, "d", a, NULL, 0, 0, &_4);
    /* Cleanup */
    node_dispose(a);
    node_dispose(b);
    node_dispose(c);
    node_dispose(d);
}

TEST(node_t, node_add_child__every_child_is_prepended)
{
    /* Prepare */
    node_t *a = node_create(INT, "a", &_1);
    node_t *b = node_create(INT, "b", &_4);
    node_t *c = node_create(INT, "c", &_3);
    node_t *d = node_create(INT, "d", &_2);
    node_t *expected_order[] =
    {
        d, c, b
    };
    /* Run */
    EXPECT_EQ(b, node_add_child(a, b));
    EXPECT_EQ(c, node_add_child(a, c));
    EXPECT_EQ(d, node_add_child(a, d));
    /* Check a */
    EXPECT_NODE(a, "a", NULL, expected_order, 3, 4, &_1);
    EXPECT_EQ(d, node_get_child(a, "d"));
    EXPECT_EQ(c, node_get_child(a, "c"));
    EXPECT_EQ(b, node_get_child(a, "b"));
    /* Check others */
    EXPECT_NODE(b, "b", a, NULL, 0, 0, &_4);
    EXPECT_NODE(c, "c", a, NULL, 0, 0, &_3);
    EXPECT_NODE(d, "d", a, NULL, 0, 0, &_2);
    /* Cleanup */
    node_dispose(a);
    node_dispose(b);
    node_dispose(c);
    node_dispose(d);
}

TEST(node_t, node_add_child__at_various_positions)
{
    /* Prepare */
    node_t *a = node_create(INT, "a", &_1);
    node_t *b = node_create(INT, "b", &_4);
    node_t *c = node_create(INT, "c", &_2);
    node_t *d = node_create(INT, "d", &_6);
    node_t *e = node_create(INT, "e", &_3);
    node_t *f = node_create(INT, "f", &_5);
    node_t *g = node_create(INT, "g", &_5);
    node_t *expected_order[] =
    {
        c, e, b, f, g, d
    };
    /* Run */
    EXPECT_EQ(b, node_add_child(a, b));
    EXPECT_EQ(c, node_add_child(a, c));
    EXPECT_EQ(d, node_add_child(a, d));
    EXPECT_EQ(e, node_add_child(a, e));
    EXPECT_EQ(f, node_add_child(a, f));
    EXPECT_EQ(g, node_add_child(a, g));
    /* Check a */
    EXPECT_NODE(a, "a", NULL, expected_order, 6, 8, &_1);
    EXPECT_EQ(b, node_get_child(a, "b"));
    EXPECT_EQ(c, node_get_child(a, "c"));
    EXPECT_EQ(d, node_get_child(a, "d"));
    EXPECT_EQ(e, node_get_child(a, "e"));
    EXPECT_EQ(f, node_get_child(a, "f"));
    EXPECT_EQ(g, node_get_child(a, "g"));
    /* Check others */
    EXPECT_NODE(b, "b", a, NULL, 0, 0, &_4);
    EXPECT_NODE(c, "c", a, NULL, 0, 0, &_2);
    EXPECT_NODE(d, "d", a, NULL, 0, 0, &_6);
    EXPECT_NODE(e, "e", a, NULL, 0, 0, &_3);
    EXPECT_NODE(f, "f", a, NULL, 0, 0, &_5);
    EXPECT_NODE(g, "g", a, NULL, 0, 0, &_5);
    /* Cleanup */
    node_dispose(a);
    node_dispose(b);
    node_dispose(c);
    node_dispose(d);
    node_dispose(e);
    node_dispose(f);
    node_dispose(g);
}

TEST(node_t, node_remove_child__when_self_is_null_returns_0)
{
    EXPECT_EQ(0, node_remove_child(NULL, "whatever"));
}

TEST(node_t, node_remove_child__when_child_name_is_null_returns_0)
{
    node_t *a = node_create(INT, "a", &_1);
    EXPECT_EQ(0, node_remove_child(a, NULL));
    node_dispose(a);
}

TEST(node_t, node_remove_child__when_child_does_not_exist_returns_0)
{
    node_t *a = node_create(INT, "a", &_1);
    node_t *b = node_create(INT, "b", &_2);
    EXPECT_EQ(b, node_add_child(a, b));
    EXPECT_EQ(0, node_remove_child(a, "i don't exist"));
    node_dispose(a);
    node_dispose(b);
}

TEST(node_t, node_remove_child__at_various_positions)
{
    node_t *a = node_create(INT, "a", &_1);
    node_t *b = node_create(INT, "b", &_2);
    node_t *c = node_create(INT, "c", &_3);
    node_t *d = node_create(INT, "d", &_4);
    node_t *e = node_create(INT, "e", &_5);
    node_t *f = node_create(INT, "f", &_6);
    node_t *expected_order_0[] =
    {
        b, c, d, e, f
    };

    EXPECT_EQ(b, node_add_child(a, b));
    EXPECT_EQ(d, node_add_child(a, d));
    EXPECT_EQ(f, node_add_child(a, f));
    EXPECT_EQ(e, node_add_child(a, e));
    EXPECT_EQ(c, node_add_child(a, c));
    EXPECT_EQ(0, memcmp(expected_order_0, a->children, sizeof(expected_order_0)));

    /* Remove the first one */
    EXPECT_EQ(1, node_remove_child(a, "b"));
    node_t *expected_order_1[] =
    {
        c, d, e, f
    };
    EXPECT_EQ(0, memcmp(expected_order_1, a->children, sizeof(expected_order_1)));

    /* Remove the last one */
    EXPECT_EQ(1, node_remove_child(a, "f"));
    node_t *expected_order_2[] =
    {
        c, d, e
    };
    EXPECT_EQ(0, memcmp(expected_order_2, a->children, sizeof(expected_order_2)));

    /* Remove the middle one */
    EXPECT_EQ(1, node_remove_child(a, "d"));
    node_t *expected_order_3[] =
    {
        c, e
    };
    EXPECT_EQ(0, memcmp(expected_order_3, a->children, sizeof(expected_order_3)));

    /* Remove the rest */
    EXPECT_EQ(1, node_remove_child(a, "e"));
    EXPECT_EQ(1, node_remove_child(a, "c"));
    EXPECT_EQ(0, a->num_children);

    /* b, c, d, e, f have been already destroyed */
    node_dispose(a);
}

TEST(node_t, node_to_json__on_null_node_returns_0)
{
    char buffer[] = "unchanged";
    EXPECT_EQ(0, node_to_json(NULL, buffer, sizeof(buffer)));
    EXPECT_STREQ("unchanged", buffer);
}

TEST(node_t, node_to_json__on_null_buffer_returns_0)
{
    node_t *a = node_create(INT, "a", NULL);
    EXPECT_EQ(0, node_to_json(a, NULL, 0));
    node_dispose(a);
}

TEST(node_t, node_to_json__when_value_is_null_parent_is_null_and_no_children)
{
    const char *expected = "{\"object_name\":\"a\",\"value\":null,\"parent\":null,\"children\":[]}";
    char buffer[61];
    node_t *a = node_create(INT, "a", NULL);
    int len = node_to_json(a, buffer, sizeof(buffer));
    EXPECT_EQ(strlen(expected), len);
    EXPECT_STREQ(expected, buffer);
    node_dispose(a);
}

TEST(node_t, node_to_json__when_value_parent_and_children_are_valid_with_various_buffer_sizes)
{
    const char *expected = "{\"object_name\":\"b\",\"value\":123,\"parent\":\"a\",\"children\":[\"d\",\"c\"]}";
    char buffer[66];
    node_t *a = node_create(INT, "a", NULL);
    node_t *b = node_create(INT, "b", &_123);
    node_t *c = node_create(INT, "c", &_2);
    node_t *d = node_create(INT, "d", &_1);
    node_add_child(a, b);
    node_add_child(b, c);
    node_add_child(b, d);
    /* Buffer size is ok */
    int len = node_to_json(b, buffer, sizeof(buffer));
    EXPECT_EQ(strlen(expected), len);
    EXPECT_STREQ(expected, buffer);
    /* Buffer is too short - object_name doesn't make it */
    EXPECT_GE(node_to_json(b, buffer, 16), 16);
    EXPECT_STREQ("{\"object_name\":", buffer);
    /* Buffer is too short - value doesn't make it */
    EXPECT_GE(node_to_json(b, buffer, 29), 29);
    EXPECT_STREQ("{\"object_name\":\"b\",\"value\":1", buffer);
    /* Buffer is too short - parent's ending quote doesn't make it */
    EXPECT_GE(node_to_json(b, buffer, 43), 43);
    EXPECT_STREQ("{\"object_name\":\"b\",\"value\":123,\"parent\":\"a", buffer);
    /* Buffer is too short - some children don't make it */
    EXPECT_GE(node_to_json(b, buffer, 61), 61);
    EXPECT_STREQ("{\"object_name\":\"b\",\"value\":123,\"parent\":\"a\",\"children\":[\"d\",", buffer);
    node_dispose(a);
    node_dispose(b);
    node_dispose(c);
    node_dispose(d);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
