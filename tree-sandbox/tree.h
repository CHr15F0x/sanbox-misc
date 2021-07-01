#ifndef TREE_H_
#define TREE_H_

#include <map>
#include "node.h"

template<typename ValueType>
class Tree
{
public:

    explicit Tree() :
    nodes_(),
    root_(nullptr)
    {}

    ~Tree()
    {
        FreeAllNodes();
    }

    bool IsEmpty() const
    {
        return root_ == nullptr;
    }

    size_t Size() const
    {
        return nodes_.size();
    }

    Node<ValueType> *Root() const
    {
        return root_;
    }

    Node<ValueType> *Find(const char *node_name) const
    {
        if (node_name == nullptr)
        {
            return nullptr;
        }

        auto it = nodes_.find(node_name);

        if (it == nodes_.cend())
        {
            return nullptr;
        }

        return it->second;
    }

    bool AddNode(Node<ValueType> *new_node, const char *parent_name = nullptr)
    {
        if (IsEmpty())
        {
            // Our tree is empty
            root_ = new_node;
            nodes_[new_node->Name()] = new_node;
            return true;
        }

        if (Find(new_node->Name()))
        {
            // These is a node with such name
            return false;
        }

        if (parent_name == nullptr)
        {
            // Only root can be added without parent
            return false;
        }

        auto parent = Find(parent_name);

        if (parent == nullptr)
        {
            // There is no such parent
            return false;
        }

        nodes_[new_node->Name()] = new_node;
        parent->Add(new_node);
        return true;
    }

    // Add a sub tree, steal its nodes, sub_tree is empty after this operation
    bool AddTree(Tree<ValueType> &sub_tree, const char *parent_name = nullptr)
    {
        if (sub_tree.IsEmpty())
        {
            // Other tree is empty, so job done
            return true;
        }

        if (IsEmpty())
        {
            // Our tree is empty, steal everything from sub_tree
            root_ = sub_tree.root_;
            nodes_ = sub_tree.nodes_;
            // Clear sub tree
            sub_tree.nodes_.clear();
            sub_tree.root_ = nullptr;
            return true;
        }

        if (parent_name == nullptr)
        {
            // Only allowed if our tree is empty
            return false;
        }

        auto parent = Find(parent_name);

        if (parent == nullptr)
        {
            // No such parent exists
            return false;
        }

        if (Find(sub_tree.Root()->Name()) != nullptr)
        {
            // sub_tree's root has the same name as one of our nodes
            return false;
        }

        // Remove duplicate nodes from sub_tree
        if (RemoveConflicts(sub_tree) == false)
        {
            return false;
        }

        // Add sub tree to our tree
        parent->Add(sub_tree.Root());
        nodes_.insert(sub_tree.nodes_.cbegin(), sub_tree.nodes_.cend());

        // Clear sub tree
        sub_tree.nodes_.clear();
        sub_tree.root_ = nullptr;

        return true;
    }

    bool RemoveNode(const char *node_name)
    {
        if (node_name == nullptr)
        {
            return false;
        }

        if (IsEmpty())
        {
            // Our tree is empty
            return false;
        }

        if (IsRoot(node_name))
        {
            // Cannot remove root
            return false;
        }

        auto it = nodes_.find(node_name);

        if (it == nodes_.cend())
        {
            return false;
        }

        // Attach children of the removed node to its parent
        for (auto it2 = it->second->children_.begin(); it2 != it->second->children_.end(); ++it2)
        {
            it->second->Parent()->Add(it2->second);
        }

        // Remove this node from its parent children
        it->second->Parent()->Remove(node_name);

        delete it->second;
        nodes_.erase(it);

        return true;
    }

    bool RemoveTree(const char *sub_tree_root_name)
    {
        if (sub_tree_root_name == nullptr)
        {
            return false;
        }

        if (IsEmpty())
        {
            // Our tree is empty
            return false;
        }

        if (IsRoot(sub_tree_root_name))
        {
            // Remove root and all nodes below
            root_ = nullptr;
            FreeAllNodes();
            nodes_.clear();
            return true;
        }

        auto sub_tree_root_node = Find(sub_tree_root_name);

        if (sub_tree_root_node == nullptr)
        {
            return false;
        }

        // Detach sub tree
        sub_tree_root_node->Parent()->Remove(sub_tree_root_name);

        // Free it completely
        RemoveSubTreeHelper(sub_tree_root_node);

        return true;
    }

    int Depth()
    {
        if (IsEmpty())
        {
            return 0;
        }

        int depth = 1;

        DepthHelper(root_, depth);

        return depth;
    }

private:

    void FreeAllNodes()
    {
        for (auto it = nodes_.cbegin(); it != nodes_.cend(); ++it)
        {
            delete it->second;
        }
    }

    bool IsRoot(const char *node_name)
    {
        return strcmp(root_->Name(), node_name) == 0;
    }

    void RemoveSubTreeHelper(Node<ValueType> *current_node)
    {
        for (auto it = current_node->children_.begin(); it != current_node->children_.end(); ++it)
        {
            RemoveSubTreeHelper(it->second);
        }

        nodes_.erase(current_node->Name());
        delete current_node;
    }

    bool RemoveConflicts(Tree<ValueType> &other_tree)
    {
        if (other_tree.IsEmpty())
        {
            return false;
        }

        if (Find(other_tree.Root()->Name()) != nullptr)
        {
            // Other tree's root is in conflict with our root
            return false;
        }

        RemoveConflictsHelper(other_tree, other_tree.Root());
        return true;
    }

    void RemoveConflictsHelper(Tree<ValueType> &other_tree, Node<ValueType> *other_tree_node)
    {
        for (auto it = other_tree_node->children_.begin(); it != other_tree_node->children_.end(); ++it)
        {
            RemoveConflictsHelper(other_tree, it->second);
        }

        if (Find(other_tree_node->Name()) != nullptr)
        {
            other_tree.RemoveNode(other_tree_node->Name());
        }
    }

    void DepthHelper(Node<ValueType> *current_node, int &depth)
    {
        if (current_node->children_.size() == 0)
        {
            return;
        }

        ++depth;

        for (auto it = current_node->children_.cbegin(); it != current_node->children_.cend(); ++it)
        {
            DepthHelper(it->second, depth);
        }
    }

    std::map<std::string, Node<ValueType> *> nodes_;
    Node<ValueType> *root_;

}; // class Tree

#endif // TREE_H_
