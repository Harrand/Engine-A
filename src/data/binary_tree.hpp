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
    std::optional<BinaryTreeNode<T>> get_left_child() const;
    std::optional<BinaryTreeNode<T>> get_right_child() const;
    void set_left_child(BinaryTreeNode<T> node);
    void set_right_child(BinaryTreeNode<T> node);
    void set_child(tz::data::binary_tree::ChildType type, BinaryTreeNode<T> node);
private:
    BinaryTree<T>* tree;
    T payload;
    std::optional<BinaryTreeNode<T>> left_child;
    std::optional<BinaryTreeNode<T>> right_child;
};

template<typename T>
class BinaryTree
{
public:
    BinaryTree();
    BinaryTree(BinaryTreeNode<T> root);
    BinaryTreeNode<T>& emplace_node(T data, BinaryTreeNode<T>* parent, tz::data::binary_tree::ChildType location);
private:
    std::optional<BinaryTreeNode<T>> root;
};

#include "data/binary_tree.inl"
#endif //TOPAZ_BINARY_TREE_HPP
