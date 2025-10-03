#pragma once
#ifdef _DEBUG
#define NEW new(_NORMAL_BLOCK, __FILE__, __LINE__ )
#endif // _DEBUG

#include <memory>
#include <iostream>
#ifdef _DEBUG
#include <crtdbg.h>
#endif

// DebugDeleter 定義
template<typename T>
struct DebugDeleter {
    const char* file;
    int line;
    DebugDeleter(const char* f, int l) : file(f), line(l) {}

    void operator()(T* ptr) const {
        std::cout << "Deleting " << ptr << " from " << file << ":" << line << "\n";
        delete ptr;
    }
};

// MakeUniqueDebug 関数
template<typename T, typename... Args>
std::unique_ptr<T,
#ifdef _DEBUG
    DebugDeleter<T>
#else
    std::default_delete<T>
#endif
>
MakeUniqueDebug(const char* file, int line, Args&&... args) {
#ifdef _DEBUG
    return std::unique_ptr<T, DebugDeleter<T>>(
        new(_NORMAL_BLOCK, file, line) T(std::forward<Args>(args)...),
        DebugDeleter<T>(file, line));
#else
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
#endif
}

// マクロ
#define MAKE_UNIQUE_DEBUG(T, ...) MakeUniqueDebug<T>(__FILE__, __LINE__, ##__VA_ARGS__)

// ポインタ型マクロ
#ifdef _DEBUG
#define UNIQUE_PTR_DEBUG(T) std::unique_ptr<T, DebugDeleter<T>>
#else
#define UNIQUE_PTR_DEBUG(T) std::unique_ptr<T>
#endif