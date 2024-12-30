#include <iostream>
#include <utility> // для std::move

// Реалізація UniquePtr
template <typename T>
class UniquePtr {
private:
    T* ptr;

public:
    // Конструктори
    explicit UniquePtr(T* p = nullptr) : ptr(p) {}
    ~UniquePtr() { delete ptr; }

    // Заборона копіювання
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // Переміщення
    UniquePtr(UniquePtr&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    // Операції доступу
    T* operator->() const { return ptr; }
    T& operator*() const { return *ptr; }
    T* get() const { return ptr; }

    // Реліз
    T* release() {
        T* temp = ptr;
        ptr = nullptr;
        return temp;
    }

    void reset(T* p = nullptr) {
        delete ptr;
        ptr = p;
    }
};

// Реалізація SharedPtr
template <typename T>
class SharedPtr {
private:
    T* ptr;
    int* refCount;

    void release() {
        if (refCount) {
            (*refCount)--;
            if (*refCount == 0) {
                delete ptr;
                delete refCount;
            }
        }
    }

public:
    // Конструктори
    explicit SharedPtr(T* p = nullptr) : ptr(p), refCount(p ? new int(1) : nullptr) {}

    ~SharedPtr() { release(); }

    // Копіювання
    SharedPtr(const SharedPtr& other) : ptr(other.ptr), refCount(other.refCount) {
        if (refCount) {
            (*refCount)++;
        }
    }

    SharedPtr& operator=(const SharedPtr& other) {
        if (this != &other) {
            release();
            ptr = other.ptr;
            refCount = other.refCount;
            if (refCount) {
                (*refCount)++;
            }
        }
        return *this;
    }

    // Переміщення
    SharedPtr(SharedPtr&& other) noexcept : ptr(other.ptr), refCount(other.refCount) {
        other.ptr = nullptr;
        other.refCount = nullptr;
    }

    SharedPtr& operator=(SharedPtr&& other) noexcept {
        if (this != &other) {
            release();
            ptr = other.ptr;
            refCount = other.refCount;
            other.ptr = nullptr;
            other.refCount = nullptr;
        }
        return *this;
    }

    // Операції доступу
    T* operator->() const { return ptr; }
    T& operator*() const { return *ptr; }
    T* get() const { return ptr; }
    int use_count() const { return refCount ? *refCount : 0; }
};

// Тестування
struct Test {
    int value;
    Test(int v) : value(v) {
        std::cout << "Test object created with value " << value << "\n";
    }
    ~Test() {
        std::cout << "Test object destroyed\n";
    }
};

int main() {
    std::cout << "Testing UniquePtr:\n";
    {
        UniquePtr<Test> up1(new Test(10));
        std::cout << "Value: " << up1->value << "\n";

        UniquePtr<Test> up2 = std::move(up1);
        if (!up1.get()) {
            std::cout << "up1 is null after move\n";
        }
        std::cout << "Value from up2: " << up2->value << "\n";
    }

    std::cout << "\nTesting SharedPtr:\n";
    {
        SharedPtr<Test> sp1(new Test(20));
        std::cout << "Use count: " << sp1.use_count() << "\n";

        SharedPtr<Test> sp2 = sp1;
        std::cout << "Use count after copy: " << sp1.use_count() << "\n";

        SharedPtr<Test> sp3 = std::move(sp1);
        std::cout << "Use count after move: " << sp2.use_count() << "\n";
        if (!sp1.get()) {
            std::cout << "sp1 is null after move\n";
        }
    }

    return 0;
}
