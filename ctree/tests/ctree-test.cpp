#include <gtest/gtest.h>
#include "ctree.h"
#include "test-helpers.h"

TEST(tree_t, tree_init__on_null_does_nothing)
{
    EXPECT_NO_FATAL_FAILURE(tree_init(NULL));
}

TEST(tree_t, tree_init)
{
    tree_t t;
    tree_init(&t);
    EXPECT_EQ(NULL, t.root);
    EXPECT_EQ(NULL, t.nodes);
    EXPECT_EQ(0, t.num_nodes);
    EXPECT_EQ(0, t.capacity);
    EXPECT_EQ(0, t.depth);
}

TEST(tree_t, tree_clear__on_null_does_nothing)
{
    EXPECT_NO_FATAL_FAILURE(tree_clear(NULL));
}

TEST(tree_t, tree_get_node__on_null_self_returns_null)
{
    EXPECT_EQ(NULL, tree_get_node(NULL, "whatever"));
}

TEST(tree_t, tree_get_node__with_null_node_name_returns_null)
{
    tree_t t;
    tree_init(&t);
    EXPECT_EQ(NULL, tree_get_node(&t, NULL));
    tree_clear(&t);
}

TEST(tree_t, tree_get_node__with_empty_node_name_returns_null)
{
    tree_t t;
    tree_init(&t);
    EXPECT_EQ(NULL, tree_get_node(&t, ""));
    tree_clear(&t);
}

TEST(tree_t, tree_get_node__on_empty_tree_returns_null)
{
    tree_t t;
    tree_init(&t);
    EXPECT_EQ(NULL, tree_get_node(&t, "somebody"));
    tree_clear(&t);
}

TEST(tree_t, tree_add_node__to_null_self_returns_null)
{
    node_t *a = node_create(INT, "a", &_1);
    EXPECT_EQ(NULL, tree_add_node(NULL, a, "whatever"));
    node_dispose(a);
}

TEST(tree_t, tree_add_node__which_is_null_returns_null)
{
    tree_t t;
    tree_init(&t);
    EXPECT_EQ(NULL, tree_add_node(&t, NULL, "whatever"));
    tree_clear(&t);
}

TEST(tree_t, tree_add_node__to_empty_tree_then_node_becomes_root)
{
    tree_t t;
    node_t *a = node_create(INT, "a", &_1);
    tree_init(&t);
    EXPECT_EQ(a, tree_add_node(&t, a, NULL));
    EXPECT_TREE(&t, a, NULL, 0, 0);
    EXPECT_EQ(a, t.root);
    tree_clear(&t);
}

TEST(tree_t, tree_add_node__when_parent_node_name_is_null_returns_null)
{
    tree_t t;
    node_t *a = node_create(INT, "a", &_1);
    node_t *b = node_create(INT, "b", &_2);
    tree_init(&t);
    EXPECT_EQ(a, tree_add_node(&t, a, NULL));
    EXPECT_EQ(NULL, tree_add_node(&t, b, NULL));
    EXPECT_TREE(&t, a, NULL, 0, 0);
    EXPECT_EQ(a, t.root);
    tree_clear(&t);
    node_dispose(b);
}

TEST(tree_t, tree_add_node__to_parent_which_does_not_exist_returns_null)
{
    tree_t t;
    node_t *a = node_create(INT, "a", &_1);
    node_t *b = node_create(INT, "b", &_2);
    tree_init(&t);
    EXPECT_EQ(a, tree_add_node(&t, a, NULL));
    EXPECT_EQ(NULL, tree_add_node(&t, b, "i don't exist"));
    EXPECT_TREE(&t, a, NULL, 0, 0);
    EXPECT_EQ(a, t.root);
    tree_clear(&t);
    node_dispose(b);
}

TEST(tree_t, tree_add_node__which_already_exists_in_the_tree_returns_null)
{
    tree_t t;
    node_t *a = node_create(INT, "a", &_1);
    node_t *a_evil_twin = node_create(INT, "a", &_2);
    tree_init(&t);
    EXPECT_EQ(a, tree_add_node(&t, a, NULL));
    EXPECT_EQ(NULL, tree_add_node(&t, a_evil_twin, "a"));
    EXPECT_TREE(&t, a, NULL, 0, 0);
    EXPECT_EQ(a, t.root);
    tree_clear(&t);
    node_dispose(a_evil_twin);
}

//
//        a
//        |
//    +---+-+
//    |     |
//    b     g
//    |     |
//  +-+-+   |
//  | | |   |
//  c e d   h
//    |     |
//    |   +-+-+
//    |   |   |
//    f   j   i
//            |
//            k
//
TEST(tree_t, tree_add_node__add_many_nodes_on_various_levels)
{
    tree_t t;
    tree_init(&t);
    node_t *a = node_create(INT, "a", &_1);
    node_t *b = node_create(INT, "b", &_2);
    node_t *c = node_create(INT, "c", &_3);
    node_t *d = node_create(INT, "d", &_4);
    node_t *e = node_create(INT, "e", &_3);
    node_t *f = node_create(INT, "f", &_5);
    node_t *g = node_create(INT, "g", &_6);
    node_t *h = node_create(INT, "h", &_7);
    node_t *i = node_create(INT, "i", &_9);
    node_t *j = node_create(INT, "j", &_8);
    node_t *k = node_create(INT, "k", &_10);
    EXPECT_EQ(a, tree_add_node(&t, a, NULL));
    EXPECT_EQ(g, tree_add_node(&t, g, "a"));
    EXPECT_EQ(h, tree_add_node(&t, h, "g"));
    EXPECT_EQ(b, tree_add_node(&t, b, "a"));
    EXPECT_EQ(e, tree_add_node(&t, e, "b"));
    EXPECT_EQ(f, tree_add_node(&t, f, "e"));
    EXPECT_EQ(d, tree_add_node(&t, d, "b"));
    EXPECT_EQ(c, tree_add_node(&t, c, "b"));
    EXPECT_EQ(i, tree_add_node(&t, i, "h"));
    EXPECT_EQ(k, tree_add_node(&t, k, "i"));
    EXPECT_EQ(j, tree_add_node(&t, j, "h"));

    /* check tree */
    node_t *expected_nodes[] =
    {
        b, c, d, e, f, g, h, i, j, k
    };
    EXPECT_TREE(&t, a, expected_nodes, 10, 12);
    /* check root */
    node_t *expected_a_children[] =
    {
        b, g
    };
    EXPECT_NODE(a, "a", NULL, expected_a_children, 2, 4, &_1);
    /* check the rest */
    node_t *expected_b_children[] =
    {
        c, e, d
    };
    EXPECT_NODE(b, "b", a, expected_b_children, 3, 4, &_2);
    EXPECT_NODE(c, "c", b, NULL, 0, 0, &_3);
    EXPECT_NODE(d, "d", b, NULL, 0, 0, &_4);
    node_t *expected_e_children[] =
    {
        f
    };
    EXPECT_NODE(e, "e", b, expected_e_children, 1, 4, &_3);
    EXPECT_NODE(f, "f", e, NULL, 0, 0, &_5);
    node_t *expected_g_children[] =
    {
        h
    };
    EXPECT_NODE(g, "g", a, expected_g_children, 1, 4, &_6);
    node_t *expected_h_children[] =
    {
        j, i
    };
    EXPECT_NODE(h, "h", g, expected_h_children, 2, 4, &_7);
    node_t *expected_i_children[] =
    {
        k
    };
    EXPECT_NODE(i, "i", h, expected_i_children, 1, 4, &_9);
    EXPECT_NODE(j, "j", h, NULL, 0, 0, &_8);
    EXPECT_NODE(k, "k", i, NULL, 0, 0, &_10);
    tree_clear(&t);
}

TEST(tree_t, tree_add_tree__to_null_self_returns_null)
{
    tree_t t;
    EXPECT_EQ(NULL, tree_add_tree(NULL, &t, "wherever"));
}

TEST(tree_t, tree_add_tree__which_is_null_returns_null)
{
    tree_t t;
    tree_init(&t);
    EXPECT_EQ(NULL, tree_add_tree(&t, NULL, "wherever"));
    tree_clear(&t);
}

TEST(tree_t, tree_add_tree__which_is_empty)
{
    tree_t t;
    tree_t u;
    tree_init(&t);
    tree_init(&u);
    node_t *a = node_create(INT, "a", &_1);
    tree_add_node(&t, a, NULL);
    EXPECT_EQ(NULL, tree_add_tree(&t, &u, "wherever"));
    tree_clear(&t);
    tree_clear(&u);
}

TEST(tree_t, tree_add_tree__when_our_tree_is_empty)
{
    tree_t t;
    tree_t u;
    tree_init(&t);
    tree_init(&u);
    node_t *a = node_create(INT, "a", &_1);
    tree_add_node(&u, a, NULL);
    EXPECT_EQ(a, tree_add_tree(&t, &u, NULL));
    EXPECT_TREE(&t, a, NULL, 0, 0);
    EXPECT_TREE(&u, NULL, NULL, 0, 0);
    tree_clear(&t);
    tree_clear(&u);
}

TEST(tree_t, tree_add_tree__when_parent_name_is_null)
{
    tree_t t;
    tree_t u;
    tree_init(&t);
    tree_init(&u);
    node_t *a = node_create(INT, "a", &_1);
    node_t *b = node_create(INT, "b", &_2);
    tree_add_node(&t, a, NULL);
    tree_add_node(&u, b, NULL);
    EXPECT_EQ(NULL, tree_add_tree(&t, &u, NULL));
    EXPECT_TREE(&t, a, NULL, 0, 0);
    EXPECT_TREE(&u, b, NULL, 0, 0);
    tree_clear(&t);
    tree_clear(&u);
}

TEST(tree_t, tree_add_tree__to_parent_which_does_not_exist)
{
    tree_t t;
    tree_t u;
    tree_init(&t);
    tree_init(&u);
    node_t *a = node_create(INT, "a", &_1);
    node_t *b = node_create(INT, "b", &_2);
    tree_add_node(&t, a, NULL);
    tree_add_node(&u, b, NULL);
    EXPECT_EQ(NULL, tree_add_tree(&t, &u, "i don't exist"));
    EXPECT_TREE(&t, a, NULL, 0, 0);
    EXPECT_TREE(&u, b, NULL, 0, 0);
    tree_clear(&t);
    tree_clear(&u);
}

TEST(tree_t, tree_add_tree__when_subtree_root_name_is_in_conflict_with_existing_node)
{
    tree_t t;
    tree_t u;
    tree_init(&t);
    tree_init(&u);
    node_t *a = node_create(INT, "a", &_1);
    node_t *a_evil_twin = node_create(INT, "a", &_2);
    tree_add_node(&t, a, NULL);
    tree_add_node(&u, a_evil_twin, NULL);
    EXPECT_EQ(NULL, tree_add_tree(&t, &u, "a"));
    EXPECT_TREE(&t, a, NULL, 0, 0);
    EXPECT_TREE(&u, a_evil_twin, NULL, 0, 0);
    tree_clear(&t);
    tree_clear(&u);
}

//
//        a    +    x     ==        a
//        |         |               |
//    +---+-+     +-+-+         +---+-+
//    |     |     | | |         |     |
//    b     g     e'y z         b     g
//    |             | |         |
//  +-+-+           | +-+     +-+-+-+
//  | | |           | | |     | | | |
//  c e d           a'f'd'    c e x d
//    |                         | |
//    |                         | +-+
//    |                         | | |
//    f                         f y z
//
//  node' == evil twin of node
//
// WARNING
// Valgrind produces a false positive for this test case
//
TEST(tree_t, tree_add_tree__conflicts_are_removed_from_subtree)
{
    tree_t t;
    tree_t u;
    tree_init(&t);
    tree_init(&u);
    node_t *a = node_create(INT, "a", &_1);
    node_t *a_et = node_create(INT, "a", &_10);
    node_t *b = node_create(INT, "b", &_2);
    node_t *c = node_create(INT, "c", &_3);
    node_t *d = node_create(INT, "d", &_5);
    node_t *d_et = node_create(INT, "d", &_9);
    node_t *e = node_create(INT, "e", &_3);
    node_t *e_et = node_create(INT, "e", &_123);
    node_t *f = node_create(INT, "f", &_6);
    node_t *f_et = node_create(INT, "f", &_8);
    node_t *g = node_create(INT, "g", &_7);
    node_t *x = node_create(INT, "x", &_4);
    node_t *y = node_create(INT, "y", &_123);
    node_t *z = node_create(INT, "z", &_123);

    EXPECT_EQ(a, tree_add_node(&t, a, NULL));
    EXPECT_EQ(b, tree_add_node(&t, b, "a"));
    EXPECT_EQ(c, tree_add_node(&t, c, "b"));
    EXPECT_EQ(d, tree_add_node(&t, d, "b"));
    EXPECT_EQ(e, tree_add_node(&t, e, "b"));
    EXPECT_EQ(f, tree_add_node(&t, f, "e"));
    EXPECT_EQ(g, tree_add_node(&t, g, "a"));

    node_t *expected_t_nodes[] =
    {
        b, c, d, e, f, g
    };
    EXPECT_TREE(&t, a, expected_t_nodes, 6, 8);

    EXPECT_EQ(x, tree_add_node(&u, x, NULL));
    EXPECT_EQ(e_et, tree_add_node(&u, e_et, "x"));
    EXPECT_EQ(y, tree_add_node(&u, y, "x"));
    EXPECT_EQ(z, tree_add_node(&u, z, "x"));
    EXPECT_EQ(a_et, tree_add_node(&u, a_et, "y"));
    EXPECT_EQ(d_et, tree_add_node(&u, d_et, "z"));
    EXPECT_EQ(f_et, tree_add_node(&u, f_et, "z"));

    node_t *expected_u_nodes[] =
    {
        a_et, d_et, e_et, f_et, y, z
    };
    EXPECT_TREE(&u, x, expected_u_nodes, 6, 8);

    /* run the tested function */
    EXPECT_EQ(x, tree_add_tree(&t, &u, "b"));

    /* check tree */
    node_t *expected_nodes[] =
    {
        b, c, d, e, f, g, x, y, z
    };
    EXPECT_TREE(&t, a, expected_nodes, 9, 12);
    /* check root */
    node_t *expected_a_children[] =
    {
        b, g
    };
    EXPECT_NODE(a, "a", NULL, expected_a_children, 2, 4, &_1);
    /* check the rest */
    node_t *expected_b_children[] =
    {
        c, e, x, d
    };
    EXPECT_NODE(b, "b", a, expected_b_children, 4, 4, &_2);
    EXPECT_NODE(c, "c", b, NULL, 0, 0, &_3);
    EXPECT_NODE(d, "d", b, NULL, 0, 0, &_5);
    node_t *expected_e_children[] =
    {
        f
    };
    EXPECT_NODE(e, "e", b, expected_e_children, 1, 4, &_3);
    EXPECT_NODE(f, "f", e, NULL, 0, 0, &_6);
    EXPECT_NODE(g, "g", a, NULL, 0, 0, &_7);
    node_t *expected_x_children[] =
    {
        y, z
    };
    node_t **IGNORED = NULL;
    EXPECT_NODE(x, "x", b, expected_x_children, 2, 4, &_4);
    EXPECT_NODE(y, "y", x, IGNORED, 0, 4, &_123);
    EXPECT_NODE(z, "z", x, IGNORED, 0, 4, &_123);

    tree_clear(&t);
    tree_clear(&u);
}

TEST(tree_t, tree_remove_node__from_null_tree_returns_0)
{
    EXPECT_EQ(0, tree_remove_node(NULL, "whatever"));
}

TEST(tree_t, tree_remove_node__with_null_name_returns_0)
{
    tree_t t;
    tree_init(&t);
    EXPECT_EQ(0, tree_remove_node(&t, NULL));
    tree_clear(&t);
}

TEST(tree_t, tree_remove_node__from_empty_tree_returns_0)
{
    tree_t t;
    tree_init(&t);
    EXPECT_EQ(0, tree_remove_node(&t, "i don't  exist anyway"));
    tree_clear(&t);
}

TEST(tree_t, tree_remove_node__cannot_remove_root_if_it_has_children)
{
    tree_t t;
    tree_init(&t);
    node_t *a = node_create(INT, "a", &_1);
    node_t *b = node_create(INT, "b", &_2);
    EXPECT_EQ(a, tree_add_node(&t, a, NULL));
    EXPECT_EQ(b, tree_add_node(&t, b, "a"));
    EXPECT_EQ(0, tree_remove_node(&t, "a"));
    node_t *expected_nodes[] =
    {
        b
    };
    EXPECT_TREE(&t, a, expected_nodes, 1, 4);
    tree_clear(&t);
}

TEST(tree_t, tree_remove_node__can_remove_root_if_it_is_childless)
{
    tree_t t;
    tree_init(&t);
    node_t *a = node_create(INT, "a", &_1);
    EXPECT_EQ(a, tree_add_node(&t, a, NULL));
    EXPECT_EQ(1, tree_remove_node(&t, "a"));
    tree_clear(&t);
}

//
//        a     =>      a
//        |             |
//    +---+-+       +-+-+-+
//    |     |       | | | |
//    b     g       c d g f
//    |
//  +-+-+
//  | | |
//  c e d
//    |
//    |
//    |
//    f
//
// WARNING
// Valgrind produces a false positive here: "Invalid read of size 8"
//
TEST(tree_t, tree_remove_node__on_various_levels)
{
    tree_t t;
    tree_init(&t);
    node_t *a = node_create(INT, "a", &_1);
    node_t *b = node_create(INT, "b", &_2);
    node_t *c = node_create(INT, "c", &_3);
    node_t *d = node_create(INT, "d", &_5);
    node_t *e = node_create(INT, "e", &_3);
    node_t *f = node_create(INT, "f", &_7);
    node_t *g = node_create(INT, "g", &_6);

    EXPECT_EQ(a, tree_add_node(&t, a, NULL));
    EXPECT_EQ(b, tree_add_node(&t, b, "a"));
    EXPECT_EQ(c, tree_add_node(&t, c, "b"));
    EXPECT_EQ(d, tree_add_node(&t, d, "b"));
    EXPECT_EQ(e, tree_add_node(&t, e, "b"));
    EXPECT_EQ(f, tree_add_node(&t, f, "e"));
    EXPECT_EQ(g, tree_add_node(&t, g, "a"));

    EXPECT_EQ(1, tree_remove_node(&t, "e"));
    EXPECT_EQ(1, tree_remove_node(&t, "b"));

    /* check tree */
    node_t *expected_nodes[] =
    {
        c, d, f, g
    };
    EXPECT_TREE(&t, a, expected_nodes, 4, 8);
    /* check nodes */
    node_t *expected_a_children[] =
    {
        c, d, g, f
    };
    EXPECT_NODE(a, "a", NULL, expected_a_children, 4, 8, &_1);
    EXPECT_NODE(c, "c", a, NULL, 0, 0, &_3);
    EXPECT_NODE(d, "d", a, NULL, 0, 0, &_5);
    EXPECT_NODE(f, "f", a, NULL, 0, 0, &_7);
    EXPECT_NODE(g, "g", a, NULL, 0, 0, &_6);
    tree_clear(&t);
}

TEST(tree_t, tree_remove_tree__from_null_self_return_0)
{
    EXPECT_EQ(0, tree_remove_tree(NULL, "whatever"));
}

TEST(tree_t, tree_remove_tree__when_sub_tree_root_name_is_null_returns_0)
{
    tree_t t;
    tree_init(&t);
    EXPECT_EQ(0, tree_remove_tree(&t, NULL));
    tree_clear(&t);
}

TEST(tree_t, tree_remove_tree__when_sub_tree_root_name_is_not_found_returns_0)
{
    tree_t t;
    tree_init(&t);
    node_t *a = node_create(INT, "a", &_1);
    tree_add_node(&t, a, NULL);
    EXPECT_EQ(0, tree_remove_tree(&t, "i don't exist"));
    tree_clear(&t);
}

TEST(tree_t, tree_remove_tree__remove_entire_one_node_tree)
{
    tree_t t;
    tree_init(&t);
    node_t *a = node_create(INT, "a", &_1);
    tree_add_node(&t, a, NULL);
    EXPECT_EQ(1, tree_remove_tree(&t, "a"));
    EXPECT_TREE(&t, NULL, NULL, 0, 0);
    tree_clear(&t);
}

TEST(tree_t, tree_remove_tree__remove_entire_tree_with_more_than_just_root)
{
    tree_t t;
    tree_init(&t);
    node_t *a = node_create(INT, "a", &_1);
    node_t *b = node_create(INT, "b", &_2);
    node_t *c = node_create(INT, "c", &_3);
    node_t *d = node_create(INT, "d", &_4);
    tree_add_node(&t, a, NULL);
    tree_add_node(&t, b, "a");
    tree_add_node(&t, c, "b");
    tree_add_node(&t, d, "a");
    EXPECT_EQ(4, tree_remove_tree(&t, "a"));
    EXPECT_TREE(&t, NULL, NULL, 0, 0);
    tree_clear(&t);
}

TEST(tree_t, tree_remove_tree__remove_sub_tree_below_root)
{
    tree_t t;
    tree_init(&t);
    node_t *a = node_create(INT, "a", &_1);
    node_t *b = node_create(INT, "b", &_2);
    node_t *c = node_create(INT, "c", &_3);
    node_t *d = node_create(INT, "d", &_4);
    node_t *e = node_create(INT, "e", &_5);
    tree_add_node(&t, a, NULL);
    tree_add_node(&t, b, "a");
    tree_add_node(&t, c, "b");
    tree_add_node(&t, d, "b");
    tree_add_node(&t, e, "a");
    EXPECT_EQ(3, tree_remove_tree(&t, "b"));
    node_t *expected_nodes[] =
    {
        e
    };
    EXPECT_TREE(&t, a, expected_nodes, 1, 4);
    tree_clear(&t);
}

TEST(tree_t, tree_depth__for_null_self_is_0)
{
    EXPECT_EQ(0, tree_depth(NULL));
}

TEST(tree_t, tree_depth__for_empty_tree_is_0)
{
    tree_t t;
    tree_init(&t);
    EXPECT_EQ(0, tree_depth(&t));
}

TEST(tree_t, tree_depth__with_childless_root_is_1)
{
    tree_t t;
    tree_init(&t);
    node_t *a = node_create(INT, "a", NULL);
    tree_add_node(&t, a, NULL);
    EXPECT_EQ(1, tree_depth(&t));
    tree_clear(&t);
}

//
// a---+
// |   |
// b-+ e
// | | |
// c d f
//     |
//     g
//
TEST(tree_t, tree_depth__for_deeper_tree)
{
    tree_t t;
    tree_init(&t);
    node_t *a = node_create(INT, "a", &_1);
    node_t *b = node_create(INT, "b", &_2);
    node_t *c = node_create(INT, "c", &_3);
    node_t *d = node_create(INT, "d", &_4);
    node_t *e = node_create(INT, "e", &_5);
    node_t *f = node_create(INT, "f", &_6);
    node_t *g = node_create(INT, "g", &_7);
    tree_add_node(&t, a, NULL);
    tree_add_node(&t, b, "a");
    tree_add_node(&t, c, "b");
    tree_add_node(&t, d, "b");
    tree_add_node(&t, e, "a");
    tree_add_node(&t, f, "e");
    tree_add_node(&t, g, "f");
    EXPECT_EQ(4, tree_depth(&t));
    tree_clear(&t);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
