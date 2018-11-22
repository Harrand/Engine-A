//
// Created by Harry on 14/11/2018.
//

#ifndef TOPAZ_BINARY_TREE_HPP
#define TOPAZ_BINARY_TREE_HPP

#include <optional>

template<typename T>
class BinaryTree;

namespace tz::data::binary_tree
{
    enum class ChildType{LEFT_CHILD, RIGHT_CHILD};
}

template<typename T>
class BinaryTreeNode
{
public:
    BinaryTreeNode(BinaryTree<T>* tree, T payload);
    const BinaryTree<T>* get_tree() const;
    const T& get_payload() const;
    const BinaryTreeNode<T>* get_left_child() const;
    const BinaryTreeNode<T>* get_right_child() const;
    const BinaryTreeNode<T>* get_child(tz::data::binary_tree::ChildType type) const;
    void set_left_child(std::unique_ptr<BinaryTreeNode<T>> node);
    void set_right_child(std::unique_ptr<BinaryTreeNode<T>> node);
    void set_child(tz::data::binary_tree::ChildType type, std::unique_ptr<BinaryTreeNode<T>> node);
    template<typename... Args>
    BinaryTreeNode<T>& emplace_child(tz::data::binary_tree::ChildType type, Args&&... args);
private:
    BinaryTree<T>* tree;
    T payload;
    std::unique_ptr<BinaryTreeNode<T>> left_child;
    std::unique_ptr<BinaryTreeNode<T>> right_child;
};

template<typename T>
class BinaryTree
{
public:
    using node_type = BinaryTreeNode<T>;
    BinaryTree();
    BinaryTree(BinaryTreeNode<T> root);
    const BinaryTreeNode<T>* get_root() const;
    BinaryTreeNode<T>& emplace_node(T data, BinaryTreeNode<T>* parent, tz::data::binary_tree::ChildType location);
private:
    std::optional<BinaryTreeNode<T>> root;
};

#include "data/binary_tree.inl"
#endif //TOPAZ_BINARY_TREE_HPP
