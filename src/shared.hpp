#ifndef SHARED_PTR
#define SHARED_PTR

template <typename T>
class ControlBlock {
   public:
    T* ptr;             // 被管理的对象指针
    size_t strong_cnt;  // 强引用计数
    size_t weak_cnt;    // 弱引用计数

    explicit ControlBlock(T* p)
        : ptr(p), strong_cnt(1), weak_cnt(0) {}

    ~ControlBlock() {
        delete ptr;  // 销毁被管理的对象
    }
};

template <typename T>
class WeakPtr;

template <typename T>
class SharedPtr {
   public:
    SharedPtr();
    explicit SharedPtr(T* pointer);
    ~SharedPtr();

    SharedPtr(const SharedPtr& other) noexcept;

    SharedPtr(const WeakPtr<T>& other) noexcept;

    SharedPtr& operator=(const SharedPtr& other) noexcept;
    operator bool() const;
    size_t use_count() const;
    T* get() const;
    T& operator*() const;
    T* operator->() const;
    void reset();
    void reset(T* pointer);

   private:
    T* ptr;
    size_t* shared_cnt;  // 使用size_t* 原因 由于共享，所以当改变cnt值的时候所有share的实例共享
    size_t* weak_cnt;
    friend class WeakPtr<T>;
    void clear();
};

template <typename T>
SharedPtr<T>::SharedPtr()
    : ptr(nullptr), shared_cnt(nullptr), weak_cnt(nullptr) {}

template <typename T>
SharedPtr<T>::SharedPtr(T* pointer)
    : ptr(pointer), shared_cnt(nullptr), weak_cnt(nullptr) {
    if (ptr != nullptr) {
        shared_cnt = new size_t(1);
        weak_cnt = new size_t(0);
    }
}

template <typename T>
SharedPtr<T>::~SharedPtr() {
    clear();
}

template <typename T>
SharedPtr<T>::SharedPtr(const SharedPtr& other) noexcept
    : ptr(other.ptr), shared_cnt(other.shared_cnt), weak_cnt(other.weak_cnt) {
    if (shared_cnt) {
        (*shared_cnt)++;
    }
}

template <typename T>
SharedPtr<T>::SharedPtr(const WeakPtr<T>& other) noexcept
    : ptr(other.ptr), shared_cnt(other.shared_cnt), weak_cnt(other.weak_cnt) {
    if (shared_cnt) {
        (*shared_cnt)++;
    }
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr& other) noexcept {
    if (&other == this) {
        return *this;
    }
    clear();
    ptr = other.ptr;
    shared_cnt = other.shared_cnt;
    weak_cnt = other.weak_cnt;
    if (shared_cnt) {
        (*shared_cnt)++;
    }
    return *this;
}

template <typename T>
SharedPtr<T>::operator bool() const {
    return (ptr != nullptr);
}

template <typename T>
size_t SharedPtr<T>::use_count() const {
    return shared_cnt ? *shared_cnt : 0;
}

template <typename T>
T* SharedPtr<T>::get() const {
    return ptr;
}

template <typename T>
T& SharedPtr<T>::operator*() const {
    return *ptr;
}

template <typename T>
T* SharedPtr<T>::operator->() const {
    return ptr;
}

template <typename T>
void SharedPtr<T>::reset() {
    clear();
    ptr = nullptr;
    shared_cnt = nullptr;
    weak_cnt = nullptr;
}

template <typename T>
void SharedPtr<T>::reset(T* pointer) {
    clear();
    ptr = pointer;
    if (ptr != nullptr) {
        shared_cnt = new size_t(1);
        weak_cnt = new size_t(0);
    } else {
        shared_cnt = nullptr;
        weak_cnt = nullptr;
    }
}

template <typename T>
void SharedPtr<T>::clear() {
    if (shared_cnt) {
        if ((*shared_cnt) > 0) {
            (*shared_cnt)--;
            if ((*shared_cnt) == 0) {
                delete ptr;
                if (weak_cnt && (*weak_cnt) == 0) {
                    delete shared_cnt;
                    delete weak_cnt;
                }
            }
        }
    }
    ptr = nullptr;
    shared_cnt = nullptr;
    weak_cnt = nullptr;
}

template <typename T, typename... Args>
SharedPtr<T> make_shared(Args&&... args) {
    // Args&&通用引用是一个模板语法特性，而非具体的引用类型,可以传入左值或者右值
    return SharedPtr<T>(new T(std::forward<Args>(args)...));
    // return SharedPtr<T>();
}

#endif  // SHARED_PTR