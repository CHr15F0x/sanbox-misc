#include <gtest/gtest.h>
#include "node.h"
#include <sstream>
#include <algorithm>

TEST(Node, DefaultConstructor)
{
    Node<int> node;

    EXPECT_STREQ("", node.Name());
    // node.Value() is garbage
    EXPECT_EQ(nullptr, node.Parent());
    EXPECT_EQ(0u, node.Children().size());
    EXPECT_EQ(0u, node.ChildrenToVector().size());
}

TEST(Node, Constructor)
{
    Node<int> node("node", 2);

    EXPECT_STREQ("node", node.Name());
    EXPECT_EQ(2, node.Value());
    EXPECT_EQ(nullptr, node.Parent());
    EXPECT_EQ(0u, node.Children().size());
    EXPECT_EQ(0u, node.ChildrenToVector().size());
}

TEST(Node, SetParent)
{
    Node<int> node("node", 2);

    EXPECT_EQ(nullptr, node.Parent());

    Node<int> parent("parent", 3);

    node.SetParent(&parent);
    EXPECT_EQ(&parent, node.Parent());
}

TEST(Node, Children_and_ChildrenToVector_have_the_same_order_and_are_sorted_by_value_and_then_by_name)
{
    Node<int> a("a", 2);
    Node<int> b("b", 5);
    Node<int> c("c", 4);
    Node<int> d("d", 3);
    Node<int> e("e", 3);

    EXPECT_TRUE(a.Add(&b));
    EXPECT_TRUE(a.Add(&c));
    EXPECT_TRUE(a.Add(&d));
    EXPECT_TRUE(a.Add(&e));

    Node<int>::ChildVec expected {&d, &e, &c, &b};

    EXPECT_EQ(expected, a.ChildrenToVector());

    auto it = a.Children().cbegin();
    auto eit = expected.cbegin();

    for (; it != a.Children().cend(); ++it, ++eit)
    {
        EXPECT_EQ(std::make_pair((*eit)->Value(), std::string((*eit)->Name())), it->first);
    }
}

TEST(Node, Child_can_handle_null)
{
    Node<int> a("a", 2);
    EXPECT_EQ(nullptr, a.Child(nullptr));
}

TEST(Node, Add_can_handle_null)
{
    Node<int> a("a", 2);
    EXPECT_EQ(false, a.Add(nullptr));
}

TEST(Node, Remove_can_handle_null)
{
    Node<int> a("a", 2);
    EXPECT_EQ(false, a.Remove(nullptr));
}

TEST(Node, Child_Add_Remove)
{
    Node<int> a("a", 2);
    Node<int> b("b", 3);

    // Add child
    EXPECT_TRUE(a.Add(&b));
    EXPECT_FALSE(a.Add(&b)); // New child has to have unique name

    EXPECT_EQ(nullptr, a.Parent());
    EXPECT_EQ(&b, a.Child("b"));
    EXPECT_EQ(1u, a.Children().size());

    EXPECT_EQ(&a, b.Parent());
    EXPECT_EQ(1u, a.Children().size());

    // Remove child
    EXPECT_TRUE(a.Remove("b"));
    EXPECT_FALSE(a.Remove("b")); // No such child exists

    EXPECT_EQ(nullptr, a.Parent());
    EXPECT_EQ(0u, a.Children().size());

    EXPECT_EQ(nullptr, b.Parent());
    EXPECT_EQ(0u, b.Children().size());
}

TEST(Node, OperatorLess)
{
    Node<int> a("a", 2);
    Node<int> b("b", 2);
    Node<int> c("c", 3);

    EXPECT_TRUE(a < b);
    EXPECT_TRUE(a < c);
    EXPECT_TRUE(b < c);
}

TEST(Node, node_array_can_be_sorted_using_std_sort)
{
    Node<int> a("a", 3);
    Node<int> b("b", 4);
    Node<int> c("c", 3);
    Node<int> d("d", 6);
    Node<int> e("e", 2);
    Node<int> f("f", 5);

    std::array<Node<int> *, 6> array = {{ &a, &b, &c, &d, &e, &f }};
    std::array<Node<int> *, 6> expected = {{ &e, &a, &c, &b, &f, &d }};

    std::sort(std::begin(array), std::end(array),
        [](Node<int> *lhs, Node<int> *rhs)
        {
            return (*lhs) < (*rhs);
        });

    EXPECT_EQ(array, expected);
}

TEST(Node, OperatorPrintToStream_via_ref)
{
    Node<int> a("a", 2);

    std::stringstream stream;
    stream << a;
    EXPECT_EQ("(a,2)", stream.str());
}

TEST(Node, OperatorPrintToStream_via_pointer)
{
    Node<int> a("a", 2);

    std::stringstream stream;
    stream << &a;
    EXPECT_EQ("(a,2)", stream.str());
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
