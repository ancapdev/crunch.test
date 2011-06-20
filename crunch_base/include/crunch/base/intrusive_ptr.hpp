#ifndef CRUNCH_BASE_INTRUSIVE_PTR_HPP
#define CRUNCH_BASE_INTRUSIVE_PTR_HPP

namespace Crunch {

//
// Reference counted pointer
//
// User must provide AddRef(T*) and Release(T*) in either T's namespace or the Crunch namespace (if the compiler doesn't support ADL)
// Providing thread safety is left to he user
template<typename T>
class IntrusivePtr
{
public:
    IntrusivePtr()
        : mObject(nullptr)
    {}

    explicit IntrusivePtr(T* object, bool addReference = true)
        : mObject(object)
    {
        if (addReference)
            AddRef(mObject);
    }

    IntrusivePtr(IntrusivePtr<T>&& rhs)
        : mObject(rhs.mObject)
    {
        rhs.mObject = nullptr;
    }

    IntrusivePtr(IntrusivePtr<T> const& rhs)
        : mObject(rhs.mObject)
    {
        AddRef(mObject);
    }

    ~IntrusivePtr()
    {
        CheckedRelease();
    }

    IntrusivePtr& operator = (IntrusivePtr const& rhs)
    {
        Reset(rhs.mObject);
        return *this;
    }

    IntrusivePtr& operator = (IntrusivePtr&& rhs)
    {
        if (&rhs == this)
            return *this;

        CheckedRelease();
        mObject = rhs.mObject;
        rhs.mObject = nullptr;
        return *this;
    }

    IntrusivePtr& operator = (T* object)
    {
        Reset(object);
        return *this;
    }

    void Reset()
    {
        CheckedRelease();
        mObject = nullptr;
    }

    void Reset(T* object, bool addReference = true)
    {
        if (addReference)
            AddRef(object);

        CheckedRelease();
        mObject = object;
    }

    T* operator -> () const
    {
        return mObject;
    }

    T& operator * () const
    {
        return *mObject;
    }

    T* Get() const
    {
        return mObject;
    }

    bool operator ! () const
    {
        return !mObject;
    }

private:
    void CheckedRelease()
    {
        if (mObject)
            Release(mObject);
    }

    T* mObject;
};

template<typename T>
bool operator == (IntrusivePtr<T> const& lhs, IntrusivePtr<T> const& rhs)
{
    return lhs.Get() == rhs.Get();
}

template<typename T>
bool operator == (IntrusivePtr<T> const& lhs, T* rhs)
{
    return lhs.Get() == rhs;
}

template<typename T>
bool operator == (T* lhs, IntrusivePtr<T> const& rhs)
{
    return lhs == rhs.Get();
}

template<typename T>
bool operator != (IntrusivePtr<T> const& lhs, IntrusivePtr<T> const& rhs)
{
    return lhs.Get() != rhs.Get();
}

template<typename T>
bool operator != (IntrusivePtr<T> const& lhs, T* rhs)
{
    return lhs.Get() != rhs;
}

template<typename T>
bool operator != (T* lhs, IntrusivePtr<T> const& rhs)
{
    return lhs != rhs.Get();
}

template<typename T>
bool operator < (IntrusivePtr<T> const& lhs, IntrusivePtr<T> const& rhs)
{
    return lhs.Get() < rhs.Get();
}

template<typename T>
bool operator < (IntrusivePtr<T> const& lhs, T* rhs)
{
    return lhs.Get() < rhs;
}

template<typename T>
bool operator < (T* lhs, IntrusivePtr<T> const& rhs)
{
    return lhs < rhs.Get();
}

template<typename T>
bool operator <= (IntrusivePtr<T> const& lhs, IntrusivePtr<T> const& rhs)
{
    return lhs.Get() <= rhs.Get();
}

template<typename T>
bool operator <= (IntrusivePtr<T> const& lhs, T* rhs)
{
    return lhs.Get() <= rhs;
}

template<typename T>
bool operator <= (T* lhs, IntrusivePtr<T> const& rhs)
{
    return lhs <= rhs.Get();
}

template<typename T>
bool operator > (IntrusivePtr<T> const& lhs, IntrusivePtr<T> const& rhs)
{
    return lhs.Get() > rhs.Get();
}

template<typename T>
bool operator > (IntrusivePtr<T> const& lhs, T* rhs)
{
    return lhs.Get() > rhs;
}

template<typename T>
bool operator > (T* lhs, IntrusivePtr<T> const& rhs)
{
    return lhs > rhs.Get();
}

template<typename T>
bool operator >= (IntrusivePtr<T> const& lhs, IntrusivePtr<T> const& rhs)
{
    return lhs.Get() >= rhs.Get();
}

template<typename T>
bool operator >= (IntrusivePtr<T> const& lhs, T* rhs)
{
    return lhs.Get() >= rhs;
}

template<typename T>
bool operator >= (T* lhs, IntrusivePtr<T> const& rhs)
{
    return lhs >= rhs.Get();
}


}

#endif
