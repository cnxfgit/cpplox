//
// Created by hlx on 2023/10/4.
//

#ifndef CPPLOX_MEMORY_H
#define CPPLOX_MEMORY_H

#include <cstdlib>
#include "common.h"
#include "object.h"

namespace cpplox{


// 初始分配内存
#define ALLOCATE(type, count) reallocate<type>(nullptr, 0, count)

// 动态数组扩容 小于8则初始化为8 否则则容量乘2
#define GROW_CAPACITY(capacity) ((capacity) < 8 ? 8 : (capacity) * 2)

// 释放数组
#define FREE_ARRAY(type, pointer, oldCount) reallocate<type>(pointer, oldCount, 0)

// 释放对象
#define FREE(type, pointer) reallocate<type>(pointer, 1, 0)

// 动态数组容量调整 类型 void指针 旧长度 新长度
#define GROW_ARRAY(type, pointer, oldCount, newCount) \
    reallocate<type>(pointer, oldCount, newCount)

    void compute(size_t oldSize, size_t newSize);

    // 重新分配内存 扩容或者缩容 取决于新旧长度的大小
    template<typename T>
    T *reallocate(T *pointer, size_t oldSize, size_t newSize) {
        oldSize = oldSize * sizeof(T);
        newSize = newSize * sizeof(T);

        compute(oldSize, newSize);

        // 新长度为0是 释放该指针 返回null
        if (newSize == 0) {
            free(pointer);
            return nullptr;
        }

        // 新长度非0时 c底层会重分配
        T *result = (T *) realloc(pointer, newSize);
        if (result == nullptr) exit(1);    // 计算机内存不足时 退出抛出异常码1
        return result;
    }

    // 标记对象
    void markObject(Obj* object);

// 标记值
    void markValue(Value value);

// 执行一次垃圾回收
    void collectGarbage();

// 释放虚拟机根链的对象
    void freeObjects();
}

#endif //CPPLOX_MEMORY_H
