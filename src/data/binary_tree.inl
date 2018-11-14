template<typename T>
BinaryTreeNode<T>::BinaryTreeNode(BinaryTree<T>* tree, T payload): tree(tree), payload(payload), left_child(std::nullopt), right_child(std::nullopt) {}

template<typename T>
const BinaryTree<T>* BinaryTreeNode<T>::get_tree() const
{
    return this->tree;
}

template<typename T>
std::optional<BinaryTreeNode<T>> BinaryTreeNode<T>::get_left_child() const
{
    return this->left_child;
}

template<typename T>
std::optional<BinaryTreeNode<T>> BinaryTreeNode<T>::get_right_child() const
{
    return this->right_child;
}

template<typename T>
void BinaryTreeNode<T>::set_left_child(BinaryTreeNode<T> node)
{
    this->left_child = node;
}

template<typename T>
void BinaryTreeNode<T>::set_right_child(BinaryTreeNode<T> node)
{
    this->right_child = node;
}

template<typename T>
void BinaryTreeNode<T>::set_child(tz::data::binary_tree::ChildType type, BinaryTreeNode<T> node)
{
    switch(type)
    {
        case tz::data::binary_tree::ChildType::LEFT_CHILD:
            this->set_left_child(node);
            break;
        case tz::data::binary_tree::ChildType::RIGHT_CHILD:
            this->set_right_child(node);
            break;
    }
}

template<typename T>
BinaryTree<T>::BinaryTree(): root(std::nullopt){}

template<typename T>
BinaryTree<T>::BinaryTree(BinaryTreeNode<T> root): root(root){}

template<typename T>
BinaryTreeNode<T>& BinaryTree<T>::emplace_node(T data, BinaryTreeNode<T>* parent, tz::data::binary_tree::ChildType location)
{
    if(parent != nullptr)
        parent->set_child(location, {this, data});
    else
        this->root = {this, data};
}