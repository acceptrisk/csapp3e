#pragma once
#include <atomic>

template <typename T>
class RefObject : public T
{
public:
    RefObject():refCount_(1)
    {}

    virtual ~RefObject()
    {}

    template<typename T1, typename... Tn>
    RefObject(T1&& t1, Tn&&... tn) : T(t1, tn...), refCount_(1)
    {}

    RefObject(const RefObject& rhs):refCount_(1)
    {}

    RefObject& operator=(const RefObject& rhs)
    {
        static_cast<T&>(*this) = rhs;
        return (*this);
    }

    virtual void Destroy()
    {
        delete this;
    }

    void Ref()
    {
        ++refCount_;
    }

    void Unref()
    {
        if (--refCount_ == 0)
        {
            Destroy();
        }        
    }
    int Use_count() const
    {
        return (int)refCount_;
    }

private:        
    std::atomic<int> refCount_;
};

