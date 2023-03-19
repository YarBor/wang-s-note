// cpp
#include <memory>
#include <iostream>
#include <cstring>

class MyClass {
public:
    // 构造函数，接受 char 数组和 string 的初始值
    MyClass(const char* c, const std::string& s) : data_(new std::string(s)), size_(std::strlen(c) + 1) {
        arr_ = new char[size_];
        std::strcpy(arr_, c);
    }

    // 移动构造函数
    MyClass(MyClass&& other) noexcept : arr_(other.arr_), size_(other.size_), data_(std::move(other.data_)) {
        other.arr_ = nullptr; // 将原来的指针置为nullptr，防止析构函数释放内存
    }

    // 复制构造函数
    MyClass(const MyClass& other) : size_(other.size_), data_(other.data_) {
        arr_ = new char[size_];
        std::memcpy(arr_, other.arr_, size_);
    }

    // 析构函数，释放成员变量指针所指向的内存
    ~MyClass() {
        delete[] arr_;
    }

    // 移动赋值运算符
    MyClass& operator=(MyClass&& other) noexcept {
        if (this != &other) {
            delete[] arr_;
            arr_ = other.arr_;
            size_ = other.size_;
            data_ = std::move(other.data_);
            other.arr_ = nullptr;
        }
        return *this;
    }

    // 拷贝赋值运算符
    MyClass& operator=(const MyClass& other) {
        if (this != &other) {
            delete[] arr_;
            size_ = other.size_;
            data_ = other.data_;
            arr_ = new char[size_];
            std::memcpy(arr_, other.arr_, size_);
        }
        return *this;
    }

private:
    char* arr_;                         // char 数组成员
    std::unique_ptr<std::string> data_; // 使用 shared_ptr 管理 string
    size_t size_;                       // char 数组长度
};