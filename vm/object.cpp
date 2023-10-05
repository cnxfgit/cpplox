//
// Created by hlx on 2023/10/4.
//

#include <cstdio>
#include <cstring>

#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"
#include "table.h"


namespace cpplox{

// 分配对象
#define ALLOCATE_OBJ(type, objectType) (type*)allocateObject<type>(objectType)

    // 分配对象
    template<typename T>
    static Obj *allocateObject(ObjType type) {
        Obj *object = reallocate<T>(nullptr, 0, 1);
        object->type = type;
        object->isMarked = false;

        // 串进虚拟机根链表中
        object->next = vm.objects;
        vm.objects = object;

#ifdef DEBUG_LOG_GC
        printf("%p allocate %zu for %d\n", (void*)object, sizeof(T), type);
#endif
        return object;
    }

    ObjBoundMethod *newBoundMethod(Value receiver, ObjClosure *method) {
        ObjBoundMethod *bound = ALLOCATE_OBJ(ObjBoundMethod, OBJ_BOUND_METHOD);
        bound->receiver = receiver;
        bound->method = method;
        return bound;
    }

    ObjClass *newClass(ObjString *name) {
        ObjClass *klass = ALLOCATE_OBJ(ObjClass, OBJ_CLASS);
        klass->name = name;
        initTable(&klass->methods);
        return klass;
    }

    ObjClosure *newClosure(ObjFunction *function) {
        // 统计提升值
        ObjUpvalue **upvalues = ALLOCATE(ObjUpvalue*, function->upvalueCount);
        for (int i = 0; i < function->upvalueCount; i++) {
            upvalues[i] = nullptr;
        }

        ObjClosure *closure = ALLOCATE_OBJ(ObjClosure, OBJ_CLOSURE);
        closure->function = function;
        closure->upvalues = upvalues;
        closure->upvalueCount = function->upvalueCount;
        return closure;
    }

    ObjFunction *newFunction() {
        ObjFunction *function = ALLOCATE_OBJ(ObjFunction, OBJ_FUNCTION);
        function->arity = 0;
        function->upvalueCount = 0;
        function->name = nullptr;
        function->chunk = new Chunk();
        return function;
    }

    ObjInstance *newInstance(ObjClass *klass) {
        ObjInstance *instance = ALLOCATE_OBJ(ObjInstance, OBJ_INSTANCE);
        instance->klass = klass;
        initTable(&instance->fields);
        return instance;
    }

    ObjNative *newNative(NativeFn function) {
        ObjNative *native = ALLOCATE_OBJ(ObjNative, OBJ_NATIVE);
        native->function = function;
        return native;
    }

// 分配字符串
    static ObjString *allocateString(char *chars, uint32_t hash) {
        ObjString *string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
        string->chars = new std::string(chars);
        string->hash = hash;

        push(OBJ_VAL(string));
        tableSet(&vm.strings, string, NIL_VAL);
        pop();
        return string;
    }

// 计算哈希值
    static uint32_t hashString(const char *key, int length) {
        uint32_t hash = 2166136261u;
        for (int i = 0; i < length; i++) {
            hash ^= (uint8_t) key[i];
            hash *= 16777619;
        }
        return hash;
    }

    ObjString *takeString(char *chars, int length) {
        uint32_t hash = hashString(chars, length);
        // 如果在全局字符串中匹配到了   则释放这个用全局的字符串
        ObjString *interned = tableFindString(&vm.strings, chars, length, hash);
        if (interned != nullptr) {
            FREE_ARRAY(char, chars, length + 1);
            return interned;
        }

        return allocateString(chars, hash);
    }

    ObjString *copyString(const char *chars, int length) {
        // 全局存在则直接用全局的
        uint32_t hash = hashString(chars, length);
        ObjString *interned = tableFindString(&vm.strings, chars, length, hash);
        if (interned != nullptr) return interned;

        char *heapChars = ALLOCATE(char, length + 1);
        memcpy(heapChars, chars, length);
        heapChars[length] = '\0';
        return allocateString(heapChars, hash);
    }

    ObjUpvalue *newUpvalue(Value *slot) {
        ObjUpvalue *upvalue = ALLOCATE_OBJ(ObjUpvalue, OBJ_UPVALUE);
        upvalue->closed = NIL_VAL;
        upvalue->location = slot;
        upvalue->next = nullptr;
        return upvalue;
    }

// 输出函数信息
    static void printFunction(ObjFunction *function) {
        if (function->name == nullptr) {
            printf("<script>");
            return;
        }
        printf("<fn %s>", function->name->chars);
    }

    void printObject(Value value) {
        switch (OBJ_TYPE(value)) {
            case OBJ_BOUND_METHOD:
                printFunction(AS_BOUND_METHOD(value)->method->function);
                break;
            case OBJ_CLASS:
                printf("%s", AS_CLASS(value)->name->chars);
                break;
            case OBJ_CLOSURE:
                printFunction(AS_CLOSURE(value)->function);
                break;
            case OBJ_FUNCTION:
                printFunction(AS_FUNCTION(value));
                break;
            case OBJ_INSTANCE:
                printf("%s instance",
                       AS_INSTANCE(value)->klass->name->chars);
                break;
            case OBJ_NATIVE:
                printf("<native fn>");
                break;
            case OBJ_STRING:
                printf("%s", AS_CSTRING(value));
                break;
            case OBJ_UPVALUE:
                printf("upvalue");
                break;
        }
    }

}
