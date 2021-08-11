#pragma once

#include <iterator>
#include <stdexcept>
#include <algorithm>
#include <initializer_list>

struct ReserveProxyObj
{
    explicit ReserveProxyObj(std::size_t size) : capacity(size){}
    std::size_t capacity;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve)
{
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector
{
public:

    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    SimpleVector(ReserveProxyObj obj)
    {
        Reserve(obj.capacity);
    }

    explicit SimpleVector(size_t size)
        :items_(new Type[size]{}), capacity_(size), size_(size) {}

    SimpleVector(size_t size, const Type& value)
        :items_(new Type[size]{}), capacity_(size), size_(size)
    {
        std::fill(items_, end(), value);
    }

    SimpleVector(std::initializer_list<Type> init)
        :items_(new Type[init.size()]{}), capacity_(init.size()), size_(init.size())
    {
        int i = 0;
        for (auto it = init.begin(); it != init.end(); ++it)
        {
            items_[i] = std::move(*it);
            ++i;
        }
    }

    SimpleVector(const SimpleVector& other)
        :items_(new Type[other.size_]{}), capacity_(other.size_), size_(other.size_)
    {
        int i = 0;
        for (auto it = other.begin(); it != other.end(); ++it)
        {
            items_[i] = *it;
            ++i;
        }
    }

    SimpleVector(SimpleVector&& other)
    {
        swap(other);
    }


    ~SimpleVector() noexcept
    {
        delete[] items_;
    }

    size_t GetSize() const noexcept
    {
        return size_;
    }

    size_t GetCapacity() const noexcept
    {
        return capacity_;
    }

    bool IsEmpty() const noexcept
    {
        return size_ == 0;
    }

    Type& operator[](size_t index) noexcept
    {
        return items_[index];
    }

    const Type& operator[](size_t index) const noexcept
    {
        return items_[index];
    }

    Type& At(size_t index)
    {
        if (index >= size_)
        {
            throw std::out_of_range("");
        }
        return items_[index];
    }

    const Type& At(size_t index) const
    {
        if (index >= size_)
        {
            throw std::out_of_range("");
        }
        return items_[index];
    }

    void Clear() noexcept
    {
        size_ = 0;
    }

    void Reserve(size_t new_capacity)
    {
        if (new_capacity > capacity_)
        {
            SimpleVector<Type> new_this(new_capacity);
            if (IsEmpty())
            {
                swap(new_this);
                Clear();
            }
            else
            {
                int old_size = size_;
                std::copy(begin(), end(), new_this.begin());
                swap(new_this);
                size_ = old_size;
            }
        }
    }

    void Resize(size_t new_size)
    {
        if (new_size <= size_)
        {
            size_ = new_size;
        }
        else if (new_size < capacity_)
        {
            std::fill(end(), items_ + new_size, 0);
            size_ = new_size;
        }
        else if (new_size > capacity_)
        {
            Type* items = new Type[new_size]{};
            std::copy(items_, end(), items);
            items_ = items;
            size_ = new_size;
            capacity_ = new_size;
        }
    }

    Iterator begin() noexcept
    {
        return items_;
    }

    Iterator end() noexcept
    {
        return items_ + size_;
    }

    ConstIterator begin() const noexcept
    {
        return items_;
    }

    ConstIterator end() const noexcept
    {
        return items_ + size_;
    }

    ConstIterator cbegin() const noexcept
    {
        return items_;
    }

    ConstIterator cend() const noexcept
    {
        return items_ + size_;
    }

    SimpleVector& operator=(const SimpleVector& rhs)
    {
        SimpleVector<Type> rhs_copy(rhs);
        swap(rhs_copy);
        return *this;
    }

    SimpleVector& operator=(SimpleVector&& rhs)
    {
        size_ = std::exchange(rhs.size_, size_);
        capacity_ = std::exchange(rhs.capacity_, capacity_);
        std::move(rhs.begin(), rhs.end(), items_);
        return *this;
    }

    void PushBack(const Type& item)
    {
        Insert(end(), item);
    }

    void PushBack(Type&& value)
    {
        Insert(end(), std::move(value));
    }

    Iterator Insert(ConstIterator pos, const Type& value)
    {
        return Inserter(pos, value);
    }

    Iterator Insert(ConstIterator pos, Type&& value)
    {
        return Inserter(pos, std::move(value));
    }

    Iterator Inserter(ConstIterator pos, Type&& value)
    {
        int dist = pos - begin();
        if (size_ == capacity_)
        {
            size_t new_capacity = std::max(1, (int)capacity_ * 2);
            Type* copy = new Type[new_capacity]{};
            std::move(begin(), end(), copy);
            Type* copy_copy = items_;
            items_ = copy;
            copy = copy_copy;
            capacity_ = new_capacity;
        }
        std::move_backward(begin() + dist, end(), end() + 1);
        *(begin() + dist) = std::move(value);
        ++size_;

        return begin() + dist;
    }

    

    void PopBack() noexcept
    {
        if (!IsEmpty())
        {
            --size_;
        }
    }

    Iterator Erase(ConstIterator pos)
    {
        int dist = pos - begin();
        std::move(begin() + dist + 1, end(), begin() + dist);
        --size_;
        return begin() + dist;
    }

    void swap(SimpleVector& other) noexcept
    {
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        std::swap(items_, other.items_);
    }

private:

    Type* items_{ nullptr };
    size_t capacity_ = 0;
    size_t size_ = 0;
};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs)
{
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs)
{
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs)
{
    return !(lhs > rhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs)
{
    return std::lexicographical_compare(rhs.begin(), rhs.end(), lhs.begin(), lhs.end());
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs)
{
    return !(lhs < rhs);
}