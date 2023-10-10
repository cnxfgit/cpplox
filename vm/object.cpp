//
// Created by hlx on 2023/10/4.
//

#include <cstdio>
#include <utility>

#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"


namespace cpplox {

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
        printf("%p allocate %zu for %d\n", (void *) object, sizeof(T), type);
#endif
        return object;
    }

    ObjBoundMethod *newBoundMethod(Value receiver, ObjClosure *method) {
        auto *bound = ALLOCATE_OBJ(ObjBoundMethod, OBJ_BOUND_METHOD);
        bound->receiver = receiver;
        bound->method = method;
        return bound;
    }

    ObjClass *newClass(ObjString *name) {
        auto *klass = ALLOCATE_OBJ(ObjClass, OBJ_CLASS);
        klass->name = name;
        klass->methods = new Table();
        return klass;
    }

    ObjClosure *newClosure(ObjFunction *function) {
        // 统计提升值
        auto **upvalues = ALLOCATE(ObjUpvalue*, function->upvalueCount);
        for (int i = 0; i < function->upvalueCount; i++) {
            upvalues[i] = nullptr;
        }

        auto *closure = ALLOCATE_OBJ(ObjClosure, OBJ_CLOSURE);
        closure->function = function;
        closure->upvalues = upvalues;
        closure->upvalueCount = function->upvalueCount;
        return closure;
    }

    ObjFunction *newFunction() {
        auto *function = ALLOCATE_OBJ(ObjFunction, OBJ_FUNCTION);
        function->arity = 0;
        function->upvalueCount = 0;
        function->name = nullptr;
        function->chunk = new Chunk();
        return function;
    }

    ObjInstance *newInstance(ObjClass *klass) {
        auto *instance = ALLOCATE_OBJ(ObjInstance, OBJ_INSTANCE);
        instance->klass = klass;
        instance->fields = new Table();
        return instance;
    }

    ObjNative *newNative(NativeFn function) {
        auto *native = ALLOCATE_OBJ(ObjNative, OBJ_NATIVE);
        native->function = function;
        return native;
    }

    // 分配字符串
    static ObjString *allocateString(std::string chars) {
        auto *string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
        string->chars = new std::string(std::move(chars));

        push(OBJ_VAL(string));
        vm.strings[string] = NIL_VAL;
        pop();
        return string;
    }

    ObjString *takeString(std::string chars) {
        ObjString key{};
        std::string str(std::move(chars));
        key.chars = &str;
        // 如果在全局字符串中匹配到了   则释放这个用全局的字符串
        auto iter = vm.strings.find(&key);
        if (iter != vm.strings.end()) {
            // 析构char时计算
            compute(str.capacity(), 0);
            return iter->first;
        }

        return allocateString(std::move(str));
    }

    ObjString *copyString(const std::string& chars) {
        ObjString key{};
        std::string str(chars);
        key.chars = &str;
        // 全局存在则直接用全局的
        auto iter = vm.strings.find(&key);
        if (iter != vm.strings.end()) {
            return iter->first;
        }

        std::string string = chars;
        compute(0, string.capacity());
        return allocateString(std::move(string));
    }

    void markTable(Table *table) {
        for (auto &item: *table) {
            markObject(item.first);
            markValue(item.second);
        }
    }

    void tableRemoveWhite(Table *table) {
        std::vector<ObjString *> toRemove;
        for (const auto &item: *table) {
            if (!item.first->isMarked) {
                toRemove.push_back(item.first);
            }
        }

        for (const auto &item: toRemove) {
            table->erase(item);
        }
    }

    ObjUpvalue *newUpvalue(Value *slot) {
        auto *upvalue = ALLOCATE_OBJ(ObjUpvalue, OBJ_UPVALUE);
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
        printf("<fn %s>", function->name->chars->c_str());
    }

    void printObject(Value value) {
        switch (OBJ_TYPE(value)) {
            case OBJ_BOUND_METHOD:
                printFunction(AS_BOUND_METHOD(value)->method->function);
                break;
            case OBJ_CLASS:
                printf("%s", AS_CLASS(value)->name->chars->c_str());
                break;
            case OBJ_CLOSURE:
                printFunction(AS_CLOSURE(value)->function);
                break;
            case OBJ_FUNCTION:
                printFunction(AS_FUNCTION(value));
                break;
            case OBJ_INSTANCE:
                printf("%s instance",
                       AS_INSTANCE(value)->klass->name->chars->c_str());
                break;
            case OBJ_NATIVE:
                printf("<native fn>");
                break;
            case OBJ_STRING:
                printf("%s", AS_CSTRING(value)->c_str());
                break;
            case OBJ_UPVALUE:
                printf("upvalue");
                break;
        }
    }

}
