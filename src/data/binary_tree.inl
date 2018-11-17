template<typename T>
BinaryTreeNode<T>::BinaryTreeNode(BinaryTree<T>* tree, T payload): tree(tree), payload(payload), left_child(nullptr), right_child(nullptr) {}

template<typename T>
const BinaryTree<T>* BinaryTreeNode<T>::get_tree() const
{
    return this->tree;
}

template<typename T>
const BinaryTreeNode<T>* BinaryTreeNode<T>::get_left_child() const
{
    return this->left_child.get();
}

template<typename T>
const BinaryTreeNode<T>* BinaryTreeNode<T>::get_right_child() const
{
    return this->right_child.get();
}

template<typename T>
const BinaryTreeNode<T>* BinaryTreeNode<T>::get_child(tz::data::binary_tree::ChildType type) const
{
    switch(type)
    {
        case tz::data::binary_tree::ChildType::LEFT_CHILD:
            return this->get_left_child();
        case tz::data::binary_tree::ChildType::RIGHT_CHILD:
            return this->get_right_child();
    }
}

template<typename T>
void BinaryTreeNode<T>::set_left_child(std::unique_ptr<BinaryTreeNode<T>> node)
{
    this->left_child = std::move(node);
}

template<typename T>
void BinaryTreeNode<T>::set_right_child(std::unique_ptr<BinaryTreeNode<T>> node)
{
    this->right_child = std::move(node);
}

template<typename T>
void BinaryTreeNode<T>::set_child(tz::data::binary_tree::ChildType type, std::unique_ptr<BinaryTreeNode<T>> node)
{
    switch(type)
    {
        case tz::data::binary_tree::ChildType::LEFT_CHILD:
            this->set_left_child(std::move(node));
            break;
        case tz::data::binary_tree::ChildType::RIGHT_CHILD:
            this->set_right_child(std::move(node));
            break;
    }
}

template<typename T>
template<typename... Args>
BinaryTreeNode<T>& BinaryTreeNode<T>::emplace_child(tz::data::binary_tree::ChildType type, Args&&... args)
{
    std::unique_ptr<BinaryTreeNode<T>> child = std::make_unique<BinaryTreeNode<T>>(std::forward<Args>(args)...);
    switch(type)
    {
        case tz::data::binary_tree::ChildType::LEFT_CHILD:
        default:
            this->set_left_child(std::move(child));
            return *this->left_child;
        case tz::data::binary_tree::ChildType::RIGHT_CHILD:
            this->set_right_child(std::move(child));
            return *this->right_child;
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
    {
        return parent->emplace_child(location, this, data);
    }
    else
    {
        this->root = {this, data};
        return root.value();
    }
}
