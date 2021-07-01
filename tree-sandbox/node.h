#ifndef NODE_H_
#define NODE_H_

#include <cstring>
#include <map>
#include <ostream>
#include <string>
#include <utility>

template<typename ValueType>
class Tree;

template<typename ValueType>
class Node
{
public:

    typedef std::vector<Node *> ChildVec;
    typedef std::map<std::pair<ValueType, std::string>, Node *> ChildMap;

    explicit Node() :
    name_(kCommonEmptyName),
    value_(),
    parent_(nullptr),
    children_()
    {}

    explicit Node(const char *name, const ValueType &value) :
    name_(name),
    value_(value),
    parent_(nullptr),
    children_()
    {}

    ~Node()
    {}

    const char *Name() const
    {
        return name_;
    }

    const ValueType &Value() const
    {
        return value_;
    }

    Node *Parent()
    {
        return parent_;
    }

    void SetParent(Node *parent)
    {
        parent_ = parent;
    }

    const ChildMap &Children() const
    {
        return children_;
    }

    ChildVec ChildrenToVector()
    {
        std::vector<Node *> res;
        res.reserve(children_.size());

        for (auto it = children_.cbegin(); it != children_.cend(); ++it)
        {
            res.push_back(it->second);
        }

        return res;
    }

    Node *Child(const char *child_name)
    {
        if (child_name == nullptr)
        {
            return nullptr;
        }

        auto it = Find(child_name);

        if (it == children_.cend())
        {
            return nullptr;
        }

        return it->second;
    }

    bool Add(Node *new_child)
    {
        if (new_child == nullptr)
        {
            return false;
        }

        new_child->parent_ = this;

        return children_.insert(std::make_pair(std::make_pair(
            new_child->Value(), new_child->Name()), new_child)).second;
    }

    bool Remove(const char *child_name)
    {
        if (child_name == nullptr)
        {
            return false;
        }

        auto it = Find(child_name);

        if (it == children_.cend())
        {
            return false;
        }

        it->second->SetParent(nullptr);
        children_.erase(it);
        return true;
    }

    bool operator < (const Node &other) const
    {
        if (this->value_ == other.value_)
        {
            return strcmp(this->name_, other.name_) < 0;
        }

        return (this->value_ < other.value_);
    }

private:

    typename ChildMap::const_iterator Find(const char *child_name) const
    {
        return std::find_if(children_.cbegin(), children_.cend(),
            [&child_name](const std::pair<std::pair<ValueType, std::string>, Node *> & t)
            {
                return (t.first.second == child_name);
            });
    }

    friend class Tree<ValueType>;

    static constexpr const char *kCommonEmptyName = "";
    const char* name_;
    ValueType value_;
    Node *parent_;
    ChildMap children_;

}; // class N

template<typename ValueType>
std::ostream &operator << (std::ostream& os, const Node<ValueType>& node)
{
    return os << '(' << node.Name() << ',' << node.Value() << ')';
}

template<typename ValueType>
std::ostream &operator << (std::ostream& os, Node<ValueType> *node)
{
    return os << (*node);
}

#endif // NODE_H_
