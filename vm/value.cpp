//
// Created by hlx on 2023/10/4.
//

#include <cstdio>

#include "object.h"
#include "memory.h"
#include "value.h"

namespace cpplox {

    void Value::print() {
#ifdef NAN_BOXING
        if (IS_BOOL((*this))) {
            printf(AS_BOOL((*this)) ? "true" : "false");
        } else if (IS_NIL((*this))) {
            printf("nil");
        } else if (IS_NUMBER((*this))) {
            printf("%g", AS_NUMBER((*this)));
        } else if (IS_OBJ((*this))) {
            printObject((*this));
        }
#else
        switch (this->type) {
            case VAL_BOOL:
                printf(AS_BOOL(*this) ? "true" : "false");
                break;
            case VAL_NIL:
                printf("nil");
                break;
            case VAL_NUMBER:
                printf("%g", AS_NUMBER(*this));
                break;
            case VAL_OBJ:
                printObject(*this);
                break;
        }
#endif
    }

    bool Value::operator==(const Value &other) const {
#ifdef NAN_BOXING
        if (IS_NUMBER((*this)) && IS_NUMBER(other)) {
            return AS_NUMBER((*this)) == AS_NUMBER(other);
        }
        return this->v == other.v;
#else
        if (this->type != other.type) return false;
        switch (this->type) {
            case VAL_BOOL:
                return AS_BOOL(*this) == AS_BOOL(other);
            case VAL_NIL:
                return true;
            case VAL_NUMBER:
                return AS_NUMBER(*this) == AS_NUMBER(other);
            case VAL_OBJ: {
                return AS_OBJ(*this) == AS_OBJ(other);
            }
            default:
                return false; // Unreachable.
        }
#endif

    }

    ValueArray::~ValueArray() {
        compute( sizeof(Value) * _data.capacity(), 0);
    }

    void ValueArray::write(Value value) {
        size_t oldSize = _data.capacity();
        _data.push_back(value);
        size_t newSize = _data.capacity();
        if (oldSize != newSize) {
            compute(oldSize * sizeof(Value), newSize * sizeof(Value));
        }
    }

    Value ValueArray::operator[](size_t index) {
        return _data[index];
    }

    size_t ValueArray::size() {
        return _data.size();
    }
}