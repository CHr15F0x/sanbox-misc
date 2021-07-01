#include <gtest/gtest.h>
#include "tree.h"
#include <sstream>

struct Data
{
    static int leak_counter;

    int payload;

    Data() :
        payload(0)
    {
        ++leak_counter;
    }

    Data(int p) :
        payload(p)
    {
        ++leak_counter;
    }

    Data(const Data &other) :
        payload(other.payload)
    {
        ++leak_counter;
    }

    ~Data()
    {
        --leak_counter;
    }

    bool operator < (const Data &other) const
    {
        return payload < other.payload;
    }
};

std::ostream &operator << (std::ostream& os, const Data &d)
{
    return os << '(' << d.payload << ')';
}

TEST(Data, test_operator_print_to_stream_to_keep_coverage_high)
{
    Data data(123);
    std::stringstream stream;
    stream << data;
    EXPECT_EQ("(123)", stream.str());
}

int Data::leak_counter = 0;

class TreeTest : public ::testing::Test
{
protected:

    virtual void SetUp()
    {
        Data::leak_counter = 0;
    }

    virtual void TearDown()
    {
        // Each test case should end without Node leaks
        EXPECT_EQ(0, Data::leak_counter);
    }
};

TEST_F(TreeTest, DefaultConstructor_tree_is_empty)
{
    Tree<Data> tree;

    EXPECT_TRUE(tree.IsEmpty());
    EXPECT_EQ(0u, tree.Size());
    EXPECT_EQ(nullptr, tree.Root());
    EXPECT_EQ(0, tree.Depth());
}

TEST_F(TreeTest, Find_null_name)
{
    Tree<Data> tree;

    EXPECT_EQ(nullptr, tree.Find(nullptr));
}

TEST_F(TreeTest, AddNode_to_empty_tree)
{
    Tree<Data> tree;
    Node<Data> *node(new Node<Data>());

    EXPECT_TRUE(tree.AddNode(node));

    EXPECT_FALSE(tree.IsEmpty());
    EXPECT_EQ(1u, tree.Size());
    EXPECT_EQ(node, tree.Root());
    EXPECT_EQ(node, tree.Find(""));
}

TEST_F(TreeTest, AddNode_to_empty_tree_parent_name_is_ignored)
{
    Tree<Data> tree;
    Node<Data> *node(new Node<Data>());

    EXPECT_TRUE(tree.AddNode(node, "parent name is ignored"));

    EXPECT_FALSE(tree.IsEmpty());
    EXPECT_EQ(1u, tree.Size());
    EXPECT_EQ(1, tree.Depth());
    EXPECT_EQ(node, tree.Root());
    EXPECT_EQ(node, tree.Find(""));
}

TEST_F(TreeTest, AddNode_cannot_add_node_with_same_name)
{
    Tree<Data> tree;
    Node<Data> *a(new Node<Data>("a", Data{2}));
    Node<Data> *a_evil_twin(new Node<Data>("a", Data{3}));

    EXPECT_TRUE(tree.AddNode(a));
    EXPECT_FALSE(tree.AddNode(a_evil_twin));

    EXPECT_FALSE(tree.IsEmpty());
    EXPECT_EQ(1u, tree.Size());
    EXPECT_EQ(a, tree.Root());
    EXPECT_EQ(a, tree.Find("a"));

    delete a_evil_twin;
}

TEST_F(TreeTest, AddNode_cannot_add_node_to_null_parent)
{
    Tree<Data> tree;
    Node<Data> *a(new Node<Data>("a", Data{2}));
    Node<Data> *b(new Node<Data>("b", Data{3}));

    EXPECT_TRUE(tree.AddNode(a));
    EXPECT_FALSE(tree.AddNode(b));

    delete b;
}

TEST_F(TreeTest, AddNode_cannot_add_node_to_nonexistent_parent)
{
    Tree<Data> tree;
    Node<Data> *a(new Node<Data>("a", Data{2}));
    Node<Data> *b(new Node<Data>("b", Data{3}));

    EXPECT_TRUE(tree.AddNode(a));
    EXPECT_FALSE(tree.AddNode(b, "i don't exist"));

    delete b;
}

TEST_F(TreeTest, AddNode_below_root)
{
    Tree<Data> tree;
    Node<Data> *a(new Node<Data>("a", Data{2}));
    Node<Data> *b(new Node<Data>("b", Data{3}));

    EXPECT_TRUE(tree.AddNode(a));
    EXPECT_TRUE(tree.AddNode(b, "a"));

    // Check tree
    EXPECT_FALSE(tree.IsEmpty());
    EXPECT_EQ(2u, tree.Size());
    EXPECT_EQ(a, tree.Root());
    EXPECT_EQ(a, tree.Find("a"));
    EXPECT_EQ(b, tree.Find("b"));

    // Check a
    EXPECT_EQ(nullptr, a->Parent());
    Node<Data>::ChildVec expected_a_children {b};
    EXPECT_EQ(expected_a_children, a->ChildrenToVector());

    // Check b
    EXPECT_EQ(a, b->Parent());
    EXPECT_EQ(0u, b->Children().size());
}

TEST_F(TreeTest, RemoveNode_null_name)
{
    Tree<Data> tree;

    EXPECT_FALSE(tree.RemoveNode(nullptr));
}

TEST_F(TreeTest, RemoveNode_from_empty_tree)
{
    Tree<Data> tree;

    EXPECT_FALSE(tree.RemoveNode("any name"));
}

TEST_F(TreeTest, RemoveNode_cannot_remove_root)
{
    Tree<Data> tree;
    Node<Data> *a(new Node<Data>("a", Data{2}));

    EXPECT_TRUE(tree.AddNode(a));
    EXPECT_FALSE(tree.RemoveNode("a"));
    EXPECT_EQ(a, tree.Find("a"));
    EXPECT_EQ(1u, tree.Size());
}

TEST_F(TreeTest, RemoveNode_which_is_not_in_the_tree)
{
    Tree<Data> tree;
    Node<Data> *a(new Node<Data>("a", Data{2}));
    Node<Data> *b(new Node<Data>("b", Data{3}));

    EXPECT_TRUE(tree.AddNode(a));
    EXPECT_TRUE(tree.AddNode(b, "a"));
    EXPECT_FALSE(tree.RemoveNode("you won't find me"));
    EXPECT_EQ(a, tree.Find("a"));
    EXPECT_EQ(b, tree.Find("b"));
    EXPECT_EQ(2u, tree.Size());
}

TEST_F(TreeTest, RemoveNode_which_is_leaf)
{
    Tree<Data> tree;
    Node<Data> *a(new Node<Data>("a", Data{2}));
    Node<Data> *b(new Node<Data>("b", Data{3}));

    EXPECT_TRUE(tree.AddNode(a));
    EXPECT_TRUE(tree.AddNode(b, "a"));
    EXPECT_TRUE(tree.RemoveNode("b"));

    // Check tree
    EXPECT_EQ(a, tree.Find("a"));
    EXPECT_EQ(nullptr, tree.Find("b"));
    EXPECT_EQ(1u, tree.Size());

    // Check a
    EXPECT_EQ(nullptr, a->Parent());
    EXPECT_EQ(0u, a->Children().size());
}

TEST_F(TreeTest, RemoveNode_which_is_not_leaf)
{
    Tree<Data> tree;
    Node<Data> *a(new Node<Data>("a", Data{2}));
    Node<Data> *b(new Node<Data>("b", Data{3}));
    Node<Data> *c(new Node<Data>("c", Data{4}));

    EXPECT_TRUE(tree.AddNode(a));
    EXPECT_TRUE(tree.AddNode(b, "a"));
    EXPECT_TRUE(tree.AddNode(c, "b"));
    EXPECT_TRUE(tree.RemoveNode("b"));

    // Check tree
    EXPECT_EQ(a, tree.Find("a"));
    EXPECT_EQ(nullptr, tree.Find("b"));
    EXPECT_EQ(c, tree.Find("c"));
    EXPECT_EQ(2u, tree.Size());

    // Check a
    EXPECT_EQ(nullptr, a->Parent());
    Node<Data>::ChildVec expected_a_children {c};
    EXPECT_EQ(expected_a_children, a->ChildrenToVector());

    // Check c
    EXPECT_EQ(a, c->Parent());
    EXPECT_EQ(0u, c->Children().size());
}

TEST_F(TreeTest, AddTree_which_is_empty)
{
    Tree<Data> tree1;
    Tree<Data> tree2;
    EXPECT_TRUE(tree1.AddTree(tree2));
}

TEST_F(TreeTest, AddTree_steal_the_other_one)
{
    Tree<Data> tree1;
    Tree<Data> tree2;
    Node<Data> *a(new Node<Data>("a", Data{2}));

    // tree2 is not empty
    EXPECT_TRUE(tree2.AddNode(a));

    // tree1 steals contents of tree2
    EXPECT_TRUE(tree1.AddTree(tree2));

    EXPECT_EQ(a, tree1.Find("a"));
    EXPECT_EQ(1u, tree1.Size());
    EXPECT_EQ(0u, tree2.Size());
}

TEST_F(TreeTest, AddTree_parent_name_is_null)
{
    Tree<Data> tree1;
    Tree<Data> tree2;
    Node<Data> *a(new Node<Data>("a", Data{2}));
    Node<Data> *b(new Node<Data>("b", Data{3}));

    EXPECT_TRUE(tree1.AddNode(a));
    EXPECT_TRUE(tree2.AddNode(b));

    EXPECT_FALSE(tree1.AddTree(tree2));

    // Both trees remain unchanged
    EXPECT_EQ(a, tree1.Find("a"));
    EXPECT_EQ(1u, tree1.Size());

    EXPECT_EQ(b, tree2.Find("b"));
    EXPECT_EQ(1u, tree2.Size());
}

TEST_F(TreeTest, AddTree_without_conflicts)
{
    Tree<Data> tree1;
    Tree<Data> tree2;
    Node<Data> *a(new Node<Data>("a", Data{2}));
    Node<Data> *b(new Node<Data>("b", Data{3}));

    EXPECT_TRUE(tree1.AddNode(a));
    EXPECT_TRUE(tree2.AddNode(b));

    EXPECT_TRUE(tree1.AddTree(tree2, "a"));

    // tree1 gets all nodes
    EXPECT_EQ(a, tree1.Find("a"));
    EXPECT_EQ(b, tree1.Find("b"));
    EXPECT_EQ(2u, tree1.Size());

    EXPECT_EQ(a, tree1.Root());

    EXPECT_EQ(nullptr, a->Parent());
    Node<Data>::ChildVec expected_a_children {b};
    EXPECT_EQ(expected_a_children, a->ChildrenToVector());

    EXPECT_EQ(a, b->Parent());
    EXPECT_EQ(0u, b->Children().size());

    // tree2 is empty now
    EXPECT_EQ(nullptr, tree2.Root());
    EXPECT_EQ(0u, tree2.Size());
}

TEST_F(TreeTest, AddTree_other_trees_root_has_conflicting_name)
{
    Tree<Data> tree1;
    Tree<Data> tree2;
    Node<Data> *a(new Node<Data>("a", Data{2}));
    Node<Data> *b(new Node<Data>("b", Data{3}));
    Node<Data> *b_evil_twin(new Node<Data>("b", Data{4}));

    EXPECT_TRUE(tree1.AddNode(a));
    EXPECT_TRUE(tree1.AddNode(b, "a"));
    EXPECT_TRUE(tree2.AddNode(b_evil_twin));

    EXPECT_FALSE(tree1.AddTree(tree2, "a"));

    // Both trees remain unchanged
    EXPECT_EQ(a, tree1.Find("a"));
    EXPECT_EQ(b, tree1.Find("b"));
    EXPECT_EQ(2u, tree1.Size());

    EXPECT_EQ(b_evil_twin, tree2.Find("b"));
    EXPECT_EQ(1u, tree2.Size());
}

//
// t1  t2       t1  t2
//
// a   d   =>   a   {}
// |   |        |
// b   a*       b
// |   |       /|
// c   b*     c d
//    /|       /|
//   e f      e f
//
// * - evil twins (ie. the same name, could be different value)
//
TEST_F(TreeTest, AddTree_other_tree_has_conflicting_nodes_below_root)
{
    Tree<Data> tree1;
    Tree<Data> tree2;
    Node<Data> *a(new Node<Data>("a", Data{2}));
    Node<Data> *b(new Node<Data>("b", Data{3}));
    Node<Data> *c(new Node<Data>("c", Data{4}));
    Node<Data> *d(new Node<Data>("d", Data{5}));
    Node<Data> *e(new Node<Data>("e", Data{6}));
    Node<Data> *f(new Node<Data>("f", Data{7}));
    Node<Data> *a_evil_twin(new Node<Data>("a", Data{8}));
    Node<Data> *b_evil_twin(new Node<Data>("b", Data{9}));

    EXPECT_TRUE(tree1.AddNode(a));
    EXPECT_TRUE(tree1.AddNode(b, "a"));
    EXPECT_TRUE(tree1.AddNode(c, "b"));

    EXPECT_TRUE(tree2.AddNode(d));
    EXPECT_TRUE(tree2.AddNode(a_evil_twin, "d"));
    EXPECT_TRUE(tree2.AddNode(b_evil_twin, "a"));
    EXPECT_TRUE(tree2.AddNode(e, "b"));
    EXPECT_TRUE(tree2.AddNode(f, "b"));

    EXPECT_TRUE(tree1.AddTree(tree2, "b"));

    // tree1 gets all nodes, except those with conflicting names
    EXPECT_EQ(a, tree1.Find("a"));
    EXPECT_EQ(b, tree1.Find("b"));
    EXPECT_EQ(c, tree1.Find("c"));
    EXPECT_EQ(d, tree1.Find("d"));
    EXPECT_EQ(e, tree1.Find("e"));
    EXPECT_EQ(f, tree1.Find("f"));
    EXPECT_EQ(6u, tree1.Size());

    EXPECT_EQ(a, tree1.Root());

    EXPECT_EQ(nullptr, a->Parent());
    Node<Data>::ChildVec expected_a_children {b};
    EXPECT_EQ(expected_a_children, a->ChildrenToVector());

    EXPECT_EQ(a, b->Parent());
    Node<Data>::ChildVec expected_b_children {c, d};
    EXPECT_EQ(expected_b_children, b->ChildrenToVector());

    EXPECT_EQ(b, c->Parent());
    EXPECT_EQ(0u, c->Children().size());

    EXPECT_EQ(b, d->Parent());
    Node<Data>::ChildVec expected_d_children {e, f};
    EXPECT_EQ(expected_d_children, d->ChildrenToVector());

    EXPECT_EQ(d, e->Parent());
    EXPECT_EQ(0u, e->Children().size());

    EXPECT_EQ(d, e->Parent());
    EXPECT_EQ(0u, f->Children().size());

    // tree2 is empty now
    EXPECT_EQ(nullptr, tree2.Root());
    EXPECT_EQ(0u, tree2.Size());
}

TEST_F(TreeTest, RemoveTree_sub_tree_root_name_is_null)
{
    Tree<Data> tree1;
    Node<Data> *a(new Node<Data>("a", Data{2}));

    EXPECT_TRUE(tree1.AddNode(a));
    EXPECT_FALSE(tree1.RemoveTree(nullptr));

    // tree1 remains unchanged
    EXPECT_EQ(a, tree1.Find("a"));
    EXPECT_EQ(1u, tree1.Size());
}

TEST_F(TreeTest, RemoveTree_our_tree_is_empty)
{
    Tree<Data> tree1;
    EXPECT_FALSE(tree1.RemoveTree("it really does not matter if i exist"));

    // tree1 remains empty
    EXPECT_EQ(nullptr, tree1.Root());
    EXPECT_EQ(0u, tree1.Size());
}

TEST_F(TreeTest, RemoveTree_which_begins_at_our_root)
{
    Tree<Data> tree1;
    Node<Data> *a(new Node<Data>("a", Data{2}));

    EXPECT_TRUE(tree1.AddNode(a));
    EXPECT_TRUE(tree1.RemoveTree("a"));

    // tree1 is empty now
    EXPECT_EQ(nullptr, tree1.Root());
    EXPECT_EQ(0u, tree1.Size());
}

TEST_F(TreeTest, RemoveTree_sub_tree_root_name_does_not_exist_in_our_tree)
{
    Tree<Data> tree1;
    Node<Data> *a(new Node<Data>("a", Data{2}));

    EXPECT_TRUE(tree1.AddNode(a));
    EXPECT_FALSE(tree1.RemoveTree("i don't exist in tree1"));

    // tree1 remains unchanged
    EXPECT_EQ(a, tree1.Find("a"));
    EXPECT_EQ(1u, tree1.Size());
}

//
//     a  =>   a
//    /|      /|
//   b c     b c
//    /|       |
//   d e       d
//    /|
//   f g
//
TEST_F(TreeTest, RemoveTree_sub_tree_with_more_than_one_node)
{
    Tree<Data> tree1;
    Node<Data> *a(new Node<Data>("a", Data{2}));
    Node<Data> *b(new Node<Data>("b", Data{3}));
    Node<Data> *c(new Node<Data>("c", Data{4}));
    Node<Data> *d(new Node<Data>("d", Data{5}));
    Node<Data> *e(new Node<Data>("e", Data{6}));
    Node<Data> *f(new Node<Data>("f", Data{7}));
    Node<Data> *g(new Node<Data>("g", Data{8}));

    EXPECT_TRUE(tree1.AddNode(a));
    EXPECT_TRUE(tree1.AddNode(b, "a"));
    EXPECT_TRUE(tree1.AddNode(c, "a"));
    EXPECT_TRUE(tree1.AddNode(d, "c"));
    EXPECT_TRUE(tree1.AddNode(e, "c"));
    EXPECT_TRUE(tree1.AddNode(f, "e"));
    EXPECT_TRUE(tree1.AddNode(g, "e"));
    EXPECT_EQ(7u, tree1.Size());
    EXPECT_EQ(4, tree1.Depth());

    EXPECT_TRUE(tree1.RemoveTree("e"));

    // tree1 has changed
    EXPECT_EQ(a, tree1.Find("a"));
    EXPECT_EQ(b, tree1.Find("b"));
    EXPECT_EQ(c, tree1.Find("c"));
    EXPECT_EQ(d, tree1.Find("d"));
    EXPECT_EQ(nullptr, tree1.Find("e"));
    EXPECT_EQ(nullptr, tree1.Find("f"));
    EXPECT_EQ(nullptr, tree1.Find("g"));
    EXPECT_EQ(4u, tree1.Size());
    EXPECT_EQ(3, tree1.Depth());

    EXPECT_EQ(a, tree1.Root());

    EXPECT_EQ(nullptr, a->Parent());
    Node<Data>::ChildVec expected_a_children {b, c};
    EXPECT_EQ(expected_a_children, a->ChildrenToVector());

    EXPECT_EQ(a, b->Parent());
    EXPECT_EQ(0u, b->Children().size());

    EXPECT_EQ(a, c->Parent());
    Node<Data>::ChildVec expected_c_children {d};
    EXPECT_EQ(expected_c_children, c->ChildrenToVector());

    EXPECT_EQ(c, d->Parent());
    EXPECT_EQ(0u, d->Children().size());
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
