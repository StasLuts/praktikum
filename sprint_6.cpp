#include <algorithm>
#include <iterator>
#include <iostream>
#include <cassert>
#include <cstddef>
#include <utility>
#include <string>
#include <vector>

using namespace std;

template <typename Type>
class SingleLinkedList
{

    struct Node
    {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val), next_node(next)
        {
        }
        Type value{};
        Node* next_node = nullptr;
    };

    template <typename ValueType>
    class BasicIterator
    {

        friend class SingleLinkedList;

        explicit BasicIterator(Node* node)
            :node_(node)
        {
        }

    public:

        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;

        BasicIterator() = default;

        BasicIterator(const BasicIterator<Type>& other) noexcept
            :node_(other.node_)
        {
        }

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept
        {
            return node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept
        {
            return !(*this == rhs);
        }

        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept
        {
            return node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept
        {
            return !(*this == rhs);
        }

        BasicIterator& operator++() noexcept
        {
            node_ = node_->next_node;
            return *this;
        }

        BasicIterator operator++(int) noexcept
        {
            auto old_value = *this;
            ++* this;
            return old_value;
        }

        [[nodiscard]] reference operator*() const noexcept
        {
            return node_->value;
        }

        [[nodiscard]] pointer operator->() const noexcept
        {
            return &node_->value;
        }

    private:

        Node* node_ = nullptr;
    };

public:

    using value_type = Type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;

    [[nodiscard]] Iterator begin() noexcept
    {
        return Iterator{ head_.next_node };
    }

    [[nodiscard]] Iterator before_begin() noexcept
    {
        return Iterator{ &head_ };
    }

    [[nodiscard]] Iterator end() noexcept
    {
        return Iterator{ nullptr };
    }

    [[nodiscard]] ConstIterator begin() const noexcept
    {
        return ConstIterator{ head_.next_node };
    }

    [[nodiscard]] ConstIterator cbefore_begin() const noexcept
    {
        return ConstIterator{ const_cast<Node*>(&head_) };
    }

    [[nodiscard]] ConstIterator before_begin() const noexcept
    {
        return ConstIterator{ &head_ };
    }

    [[nodiscard]] ConstIterator end() const noexcept
    {
        return ConstIterator{ nullptr };
    }

    [[nodiscard]] ConstIterator cbegin() const noexcept
    {
        return ConstIterator{ head_.next_node };
    }

    [[nodiscard]] ConstIterator cend() const noexcept
    {
        return ConstIterator{ nullptr };
    }

    SingleLinkedList() = default;

    SingleLinkedList(std::initializer_list<Type> values)
    {
        for (auto it = std::rbegin(values); it != std::rend(values); ++it)
        {
            PushFront(*it);
        }
    }

    SingleLinkedList(const SingleLinkedList& other)
    {
        SingleLinkedList tmp;
        vector<Type> tmp_vector;
        for (const auto& value : other) {
            tmp_vector.push_back(value);
        }
        for (auto it = std::rbegin(tmp_vector); it != std::rend(tmp_vector); ++it) {
            tmp.PushFront(*it);
        }
        swap(tmp);
    }

    ~SingleLinkedList()
    {
        Clear();
    }

    void Clear() noexcept
    {
        while (head_.next_node)
        {
            Node* deleteMe = head_.next_node;
            head_.next_node = head_.next_node->next_node;
            delete deleteMe;
        }
        size_ = 0;
    }

    Iterator InsertAfter(ConstIterator pos, const Type& value)
    {
        pos.node_->next_node = new Node(value, pos.node_->next_node);
        ++size_;
        return Iterator{ pos.node_->next_node };
    }

    void PopFront() noexcept
    {
        Node* second = head_.next_node->next_node;
        delete head_.next_node;
        head_.next_node = second;
        --size_;
    }

    Iterator EraseAfter(ConstIterator pos) noexcept
    {
        Node* second = pos.node_->next_node->next_node;
        delete pos.node_->next_node;
        pos.node_->next_node = second;
        --size_;
        return Iterator{ pos.node_->next_node };
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs)
    {
        if (head_.next_node == rhs.head_.next_node)
        {
            return *this;
        }
        SingleLinkedList buffer(rhs);
        swap(buffer);
        return *this;
    }

    void swap(SingleLinkedList& other) noexcept
    {
        std::swap(size_, other.size_);
        std::swap(head_.next_node, other.head_.next_node);
    }

    void PushFront(const Type& value)
    {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }

    [[nodiscard]] size_t GetSize() const noexcept
    {
        return size_;
    }

    [[nodiscard]] bool IsEmpty() const noexcept
    {
        return GetSize() == 0;
    }

private:

    Node head_{};
    size_t size_ = 0;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept
{
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
    return equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
    return lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
    return !(lhs > rhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
    return lexicographical_compare(rhs.begin(), rhs.end(), lhs.begin(), lhs.end());
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs)
{
    return !(lhs < rhs);
}