//// Created by hlx on 2023/10/4.//#include "vm.h"#include <cstdarg>#include <cstdio>#include <cstring>#include <ctime>#include "common.h"#include "debug.h"#include "compiler.h"#include "object.h"#include "memory.h"namespace cpplox {    VM vm;    // 时钟原生函数    static Value clockNative(int argCount, Value *args) {        return NUMBER_VAL((double) clock() / CLOCKS_PER_SEC);    }// 重置虚拟机栈 top指针指向栈数组首位即可    static void resetStack() {        vm.stackTop = vm.stack;        vm.frameCount = 0;        vm.openUpvalues = nullptr;    }// 运行时异常    static void runtimeError(const char *format, ...) {        va_list args;        va_start(args, format);        vfprintf(stderr, format, args);        va_end(args);        fputs("\n", stderr);        for (int i = vm.frameCount - 1; i >= 0; i--) {            CallFrame *frame = &vm.frames[i];            ObjFunction *function = frame->closure->function;            size_t instruction = frame->ip - function->chunk->code.data() - 1;            fprintf(stderr, "[line %d] in ",                    function->chunk->lines[instruction]);            if (function->name == nullptr) {                fprintf(stderr, "script\n");            } else {                fprintf(stderr, "%s()\n", function->name->chars->c_str());            }        }        resetStack();    }// 定义一个原生函数    static void defineNative(std::string name, NativeFn function) {        push(OBJ_VAL(copyString(name)));        push(OBJ_VAL(newNative(function)));        vm.globals[AS_STRING(vm.stack[0])] = vm.stack[1];        pop();        pop();    }    void initVM() {        resetStack();        vm.objects = nullptr;        vm.bytesAllocated = 0;        vm.nextGC = 1024 * 1024;        vm.grayCount = 0;        vm.grayCapacity = 0;        vm.grayStack = nullptr;        vm.initString = nullptr;        vm.initString = copyString("init");        defineNative("clock", clockNative);    }    void freeVM() {        vm.globals.clear();        vm.strings.clear();        vm.initString = nullptr;        freeObjects();    }    void push(Value value) {        *vm.stackTop = value;        vm.stackTop++;    }    Value pop() {        vm.stackTop--;        return *vm.stackTop;    }// 查看栈中的值但是不弹出 peek(0)为栈顶    static Value peek(int distance) {        return vm.stackTop[-1 - distance];    }// 执行    static bool call(ObjClosure *closure, int argCount) {        if (argCount != closure->function->arity) {            runtimeError("Expected %d arguments but got %d.", closure->function->arity, argCount);            return false;        }        // 调用栈过长        if (vm.frameCount == FRAMES_MAX) {            runtimeError("Stack overflow.");            return false;        }        // 记录新函数栈帧        CallFrame *frame = &vm.frames[vm.frameCount++];        frame->closure = closure;        frame->ip = closure->function->chunk->code.data();        frame->slots = vm.stackTop - argCount - 1;        return true;    }// 调用 值类型  仅接受 函数 类 方法    static bool callValue(Value callee, int argCount) {        if (IS_OBJ(callee)) {            switch (OBJ_TYPE(callee)) {                case OBJ_BOUND_METHOD: {                    ObjBoundMethod *bound = AS_BOUND_METHOD(callee);                    vm.stackTop[-argCount - 1] = bound->receiver;                    return call(bound->method, argCount);                }                case OBJ_CLASS: {                    ObjClass *klass = AS_CLASS(callee);                    vm.stackTop[-argCount - 1] = OBJ_VAL(newInstance(klass));                    if (klass->methods->find(vm.initString) != klass->methods->end()) {                        return call(AS_CLOSURE((*klass->methods)[vm.initString]), argCount);                    } else if (argCount != 0) {                        runtimeError("Expected 0 arguments but got %d.", argCount);                        return false;                    }                    return true;                }                case OBJ_CLOSURE:                    return call(AS_CLOSURE(callee), argCount);                case OBJ_NATIVE: {                    NativeFn native = AS_NATIVE(callee);                    Value result = native(argCount, vm.stackTop - argCount);                    vm.stackTop -= argCount + 1;                    push(result);                    return true;                }                default:                    break; // Non-callable object type.            }        }        runtimeError("Can only call functions and classes.");        return false;    }    // 从类中执行方法    static bool invokeFromClass(ObjClass *klass, ObjString *name, int argCount) {        if (klass->methods->find(name) == klass->methods->end()) {            runtimeError("Undefined property '%s'.", name->chars->c_str());            return false;        }        return call(AS_CLOSURE((*klass->methods)[name]), argCount);    }// 执行方法    static bool invoke(ObjString *name, int argCount) {        Value receiver = peek(argCount);        if (!IS_INSTANCE(receiver)) {            runtimeError("Only instances have methods.");            return false;        }        ObjInstance *instance = AS_INSTANCE(receiver);        if (instance->fields->find(name) != instance->fields->end()) {            Value value = (*instance->fields)[name];            vm.stackTop[-argCount - 1] = value;            return callValue(value, argCount);        }        return invokeFromClass(instance->klass, name, argCount);    }// 绑定方法给实例    static bool bindMethod(ObjClass *klass, ObjString *name) {        if (klass->methods->find(name) == klass->methods->end()) {            runtimeError("Undefined property '%s'.", name->chars->c_str());            return false;        }        ObjBoundMethod *bound = newBoundMethod(peek(0),                                               AS_CLOSURE((*klass->methods)[name]));        pop();        push(OBJ_VAL(bound));        return true;    }// 捕获提升值    static ObjUpvalue *captureUpvalue(Value *local) {        ObjUpvalue *prevUpvalue = nullptr;        ObjUpvalue *upvalue = vm.openUpvalues;        while (upvalue != nullptr && upvalue->location > local) {            prevUpvalue = upvalue;            upvalue = upvalue->next;        }        if (upvalue != nullptr && upvalue->location == local) {            return upvalue;        }        ObjUpvalue *createdUpvalue = newUpvalue(local);        createdUpvalue->next = upvalue;        if (prevUpvalue == nullptr) {            vm.openUpvalues = createdUpvalue;        } else {            prevUpvalue->next = createdUpvalue;        }        return createdUpvalue;    }// 关闭提升值    static void closeUpvalues(Value *last) {        while (vm.openUpvalues != nullptr && vm.openUpvalues->location >= last) {            ObjUpvalue *upvalue = vm.openUpvalues;            upvalue->closed = *upvalue->location;            upvalue->location = &upvalue->closed;            vm.openUpvalues = upvalue->next;        }    }// 定义方法    static void defineMethod(ObjString *name) {        Value method = peek(0);        ObjClass *klass = AS_CLASS(peek(1));        (*klass->methods)[name] = method;        pop();    }// 是否为false 只要不为空或者布尔false都是true    static bool isFalsey(Value value) {        return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));    }// 连接字符串    static void concatenate() {        ObjString *b = AS_STRING(peek(0));        ObjString *a = AS_STRING(peek(1));        std::string chars = *a->chars + *b->chars;        compute(0, chars.capacity());        ObjString *result = takeString(std::move(chars));        pop();        pop();        push(OBJ_VAL(result));    }// 虚拟机运行时    static InterpretResult run() {        // 拿到vm中的栈帧        CallFrame *frame = &vm.frames[vm.frameCount - 1];// 读取字节码块单个字节#define READ_BYTE() (*frame->ip++)// 读取常量，在读取单个字节后再读取单个字节的值为常量数组的索引#define READ_SHORT() (frame->ip += 2, (uint16_t)((frame->ip[-2] << 8) | frame->ip[-1]))// 读取常量，在读取单个字节后再读取单个字节的值为常量数组的索引#define READ_CONSTANT() \    (frame->closure->function->chunk->constants[READ_BYTE()])// 读取常量后 转化为值字符串#define READ_STRING() AS_STRING(READ_CONSTANT())// 模拟二元运算#define BINARY_OP(valueType, op) \    do { \      if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \        runtimeError("Operands must be numbers."); \        return InterpretResult::RUNTIME_ERROR; \      } \      double b = AS_NUMBER(pop()); \      double a = AS_NUMBER(pop()); \      push(valueType(a op b)); \    } while (false)        for (;;) {// debug 轨迹 执行#ifdef DEBUG_TRACE_EXECUTION            // 打印虚拟机栈的内容        printf("          ");        for (Value *slot = vm.stack; slot < vm.stackTop; slot++) {            printf("[ ");            slot->print();            printf(" ]");        }        printf("\n");        // 反汇编        disassembleInstruction(frame->closure->function->chunk,        (int)(frame->ip - frame->closure->function->chunk->code.data()));#endif            uint8_t instruction = READ_BYTE();            switch (instruction) {                case OP_CONSTANT: {                    Value constant = READ_CONSTANT();                    push(constant);                    break;                }                case OP_NIL:                    push(NIL_VAL);                    break;                case OP_TRUE:                    push(BOOL_VAL(true));                    break;                case OP_FALSE:                    push(BOOL_VAL(false));                    break;                case OP_POP:                    pop();                    break;                case OP_GET_LOCAL: {                    uint8_t slot = READ_BYTE();                    push(frame->slots[slot]);                    break;                }                case OP_SET_LOCAL: {                    uint8_t slot = READ_BYTE();                    frame->slots[slot] = peek(0);                    break;                }                case OP_GET_GLOBAL: {                    ObjString *name = READ_STRING();                    printf("name: %s\n", name->chars->c_str());                    if (vm.globals.find(name) == vm.globals.end()) {                        runtimeError("Undefined variable '%s'.", name->chars->c_str());                        return InterpretResult::RUNTIME_ERROR;                    }                    push(vm.globals[name]);                    break;                }                case OP_DEFINE_GLOBAL: {                    ObjString *name = READ_STRING();                    vm.globals[name] = peek(0);                    pop();                    break;                }                case OP_SET_GLOBAL: {                    ObjString *name = READ_STRING();                    if (vm.globals.find(name) == vm.globals.end()) {                        runtimeError("Undefined variable '%s'.", name->chars->c_str());                        return InterpretResult::RUNTIME_ERROR;                    }                    vm.globals[name] = peek(0);                    break;                }                case OP_GET_UPVALUE: {                    uint8_t slot = READ_BYTE();                    push(*frame->closure->upvalues[slot]->location);                    break;                }                case OP_SET_UPVALUE: {                    uint8_t slot = READ_BYTE();                    *frame->closure->upvalues[slot]->location = peek(0);                    break;                }                case OP_GET_PROPERTY: {                    if (!IS_INSTANCE(peek(0))) {                        runtimeError("Only instances have properties.");                        return InterpretResult::RUNTIME_ERROR;                    }                    ObjInstance *instance = AS_INSTANCE(peek(0));                    ObjString *name = READ_STRING();                    if (instance->fields->find(name) != instance->fields->end()) {                        pop(); // Instance.                        push((*instance->fields)[name]);                        break;                    }                    if (!bindMethod(instance->klass, name)) {                        return InterpretResult::RUNTIME_ERROR;                    }                    break;                }                case OP_SET_PROPERTY: {                    if (!IS_INSTANCE(peek(1))) {                        runtimeError("Only instances have fields.");                        return InterpretResult::RUNTIME_ERROR;                    }                    ObjInstance *instance = AS_INSTANCE(peek(1));                    (*instance->fields)[READ_STRING()] = peek(0);                    Value value = pop();                    pop();                    push(value);                    break;                }                case OP_GET_SUPER: {                    ObjString *name = READ_STRING();                    ObjClass *superclass = AS_CLASS(pop());                    if (!bindMethod(superclass, name)) {                        return InterpretResult::RUNTIME_ERROR;                    }                    break;                }                case OP_EQUAL: {                    Value b = pop();                    Value a = pop();                    push(BOOL_VAL((a == b)));                    break;                }                case OP_GREATER:                    BINARY_OP(BOOL_VAL, >);                    break;                case OP_LESS:                    BINARY_OP(BOOL_VAL, <);                    break;                case OP_ADD: {                    if (IS_STRING(peek(0)) && IS_STRING(peek(1))) {                        concatenate();                    } else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1))) {                        double b = AS_NUMBER(pop());                        double a = AS_NUMBER(pop());                        push(NUMBER_VAL(a + b));                    } else {                        runtimeError(                                "Operands must be two numbers or two strings.");                        return InterpretResult::RUNTIME_ERROR;                    }                    break;                }                case OP_SUBTRACT:                    BINARY_OP(NUMBER_VAL, -);                    break;                case OP_MULTIPLY:                    BINARY_OP(NUMBER_VAL, *);                    break;                case OP_DIVIDE:                    BINARY_OP(NUMBER_VAL, /);                    break;                case OP_NOT:                    push(BOOL_VAL(isFalsey(pop())));                    break;                case OP_NEGATE:                    if (!IS_NUMBER(peek(0))) {                        runtimeError("Operand must be a number.");                        return InterpretResult::RUNTIME_ERROR;                    }                    push(NUMBER_VAL(-AS_NUMBER(pop())));                    break;                case OP_PRINT: {                    pop().print();                    printf("\n");                    break;                }                case OP_JUMP: {                    uint16_t offset = READ_SHORT();                    frame->ip += offset;                    break;                }                case OP_JUMP_IF_FALSE: {                    uint16_t offset = READ_SHORT();                    if (isFalsey(peek(0))) frame->ip += offset;                    break;                }                case OP_LOOP: {                    uint16_t offset = READ_SHORT();                    frame->ip -= offset;                    break;                }                case OP_CALL: {                    int argCount = READ_BYTE();                    if (!callValue(peek(argCount), argCount)) {                        return InterpretResult::RUNTIME_ERROR;                    }                    // 调用成功后将栈帧还回去                    frame = &vm.frames[vm.frameCount - 1];                    break;                }                case OP_INVOKE: {                    ObjString *method = READ_STRING();                    int argCount = READ_BYTE();                    if (!invoke(method, argCount)) {                        return InterpretResult::RUNTIME_ERROR;                    }                    frame = &vm.frames[vm.frameCount - 1];                    break;                }                case OP_SUPER_INVOKE: {                    ObjString *method = READ_STRING();                    int argCount = READ_BYTE();                    ObjClass *superclass = AS_CLASS(pop());                    if (!invokeFromClass(superclass, method, argCount)) {                        return InterpretResult::RUNTIME_ERROR;                    }                    frame = &vm.frames[vm.frameCount - 1];                    break;                }                case OP_CLOSURE: {                    ObjFunction *function = AS_FUNCTION(READ_CONSTANT());                    ObjClosure *closure = newClosure(function);                    push(OBJ_VAL(closure));                    for (int i = 0; i < closure->upvalueCount; i++) {                        uint8_t isLocal = READ_BYTE();                        uint8_t index = READ_BYTE();                        if (isLocal) {                            closure->upvalues[i] = captureUpvalue(frame->slots + index);                        } else {                            closure->upvalues[i] = frame->closure->upvalues[index];                        }                    }                    break;                }                case OP_CLOSE_UPVALUE:                    closeUpvalues(vm.stackTop - 1);                    pop();                    break;                case OP_RETURN: {                    Value result = pop();                    closeUpvalues(frame->slots);                    vm.frameCount--;                    if (vm.frameCount == 0) {                        pop();                        return InterpretResult::OK;                    }                    vm.stackTop = frame->slots;                    push(result);                    frame = &vm.frames[vm.frameCount - 1];                    break;                }                case OP_CLASS:                    push(OBJ_VAL(newClass(READ_STRING())));                    break;                case OP_INHERIT: {                    Value superclass = peek(1);                    if (!IS_CLASS(superclass)) {                        runtimeError("Superclass must be a class.");                        return InterpretResult::RUNTIME_ERROR;                    }                    ObjClass *subclass = AS_CLASS(peek(0));                    Table *from = AS_CLASS(superclass)->methods;                    subclass->methods->insert(from->begin(), from->end());                    pop(); // Subclass.                    break;                }                case OP_METHOD:                    defineMethod(READ_STRING());                    break;            }        }#undef READ_BYTE#undef READ_SHORT#undef READ_CONSTANT#undef READ_STRING#undef BINARY_OP    }    InterpretResult VM::interpret(const char *source) {        // 解释时编译        ObjFunction *function = compile(source);        if (function == nullptr) return InterpretResult::COMPILE_ERROR;        push(OBJ_VAL(function));        ObjClosure *closure = newClosure(function);        pop();        push(OBJ_VAL(closure));        call(closure, 0);        return run();    }}