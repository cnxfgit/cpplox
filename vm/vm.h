//
// Created by hlx on 2023/10/4.
//

#ifndef CPPLOX_VM_H
#define CPPLOX_VM_H

#include <cstdio>

#include "object.h"
#include "value.h"


namespace cpplox {

    // 栈数组最大值
    const size_t FRAMES_MAX = 64;
    const size_t STACK_MAX = (FRAMES_MAX * UINT8_COUNT);

    enum class InterpretResult {
        OK,               // 解释执行成功
        COMPILE_ERROR,    // 编译期异常
        RUNTIME_ERROR     // 运行时异常
    };

// 调用帧
    struct CallFrame {
        ObjClosure *closure;        // 调用的函数闭包
        uint8_t *ip;                // 指向字节码数组的指针 指函数执行到哪了
        Value *slots;               // 指向vm栈中该函数使用的第一个局部变量
    };

// 虚拟机
    class VM {
    public:
        CallFrame frames[FRAMES_MAX];   // 栈帧数组 所有函数调用的执行点
        int frameCount;                 // 当前调用栈数

        Value stack[STACK_MAX];         // 虚拟机栈
        Value *stackTop;                // 栈顶指针 总是指向栈顶
        Table globals;                  // 全局变量表
        Table strings;                  // 全局字符串表
        ObjString *initString;          // 构造器名称
        ObjUpvalue *openUpvalues;       // 全局提升值

        size_t bytesAllocated;          // 已经分配的内存
        size_t nextGC;                  // 出发下一次gc的阈值

        Obj *objects;                   // 对象根链表
        int grayCount;                  // 灰色对象数量
        int grayCapacity;               // 灰色对象容量
        Obj **grayStack;                // 灰色对象栈

        // 解释字节码块
        InterpretResult interpret(const char *source);

        void push(Value value);

        Value pop();

        void resetStack();

        void defineNative(const std::string& name, NativeFn function);

    private:
        void runtimeError(const char *format, ...);

        Value peek(int distance);

        bool call(ObjClosure *closure, int argCount);

        bool callValue(Value callee, int argCount);

        bool invokeFromClass(ObjClass *klass, ObjString *name, int argCount);

        bool invoke(ObjString *name, int argCount);

        bool bindMethod(ObjClass *klass, ObjString *name);

        ObjUpvalue *captureUpvalue(Value *local);

        void closeUpvalues(Value *last);

        void defineMethod(ObjString *name);

        void concatenate();

        InterpretResult run();
    };

    extern VM vm;

    // 初始化虚拟机啊
    void initVM();

    // 释放虚拟机
    void freeVM();

}

#endif //CPPLOX_VM_H
