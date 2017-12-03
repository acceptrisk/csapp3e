#pragma once
#include <iostream>

/// @class RefPtr<T> - reference counter based smart pointer
template<typename T>
class RefPtr
{
public:
    RefPtr() : ptr_(nullptr)
    {}

    RefPtr(T* p) : ptr_(p)
    {}

    ~RefPtr()
    {
        if (ptr_)
        {
            ptr_->Unref();
        }
    }

    RefPtr(const RefPtr& rhs) noexcept : ptr_(rhs.ptr_)
    {
        if (ptr_)
        {
            ptr_->Ref();
        }
    }

    RefPtr& operator=(RefPtr rhs) noexcept
    {
        //RefPtr temp(rhs);
        swap(*this, rhs);
        //T* p = rhs.ptr_;
        //if (p)
        //{
        //    p->Ref();
        //}
        //if (ptr_)
        //{
        //    ptr_->Unref();
        //}
        //ptr_ = p;
        return (*this);
    }

    RefPtr(RefPtr&& rhs) noexcept : ptr_(rhs.ptr_)
    {
        std::cout << "move constructor" << std::endl;
        rhs.ptr_ = nullptr;
    }

    //RefPtr& operator=(RefPtr&& rhs) noexcept
    //{
    //    std::cout << "move assignment operator" << std::endl;
    //    if (ptr_)
    //    {
    //        ptr_->Unref();
    //    }
    //    ptr_ = rhs.ptr_;
    //    rhs.ptr_ = nullptr;
    //    return (*this);
    //}

    const T* Get() const noexcept
    {
        return const_cast<const T*>(ptr_);
    }

    T* Get() noexcept
    {
        return ptr_;
    }

    T& operator*()
    {
        return (*ptr_);
    }

    T* operator->()
    {
        return ptr_;
    }

    void reset(T* p)
    {
        if (ptr_)
        {
            ptr_->Unref();
        }

        ptr_ = p;
    }

    T* release() noexcept
    {
        T* p = ptr_;
        ptr_ = nullptr;
        return p;
    }

    friend void swap(RefPtr& first, RefPtr & second) noexcept
    {
        //enable ADL
        using std::swap;

        // by swapping the members of two objects,
        // the two objects are effectively swapped
        swap(first.ptr_, second.ptr_);
    }

private:
    T* ptr_;
};
