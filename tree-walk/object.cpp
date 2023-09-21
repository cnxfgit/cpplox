//
// Created by hlx on 2023/9/21.
//

#include "object.h"

namespace cpplox{
    Boolean::Boolean(bool value) {
        this->value = value;
    }

    std::string Boolean::toString() {
        return value ? "true" : "false";
    }

    Boolean::~Boolean() = default;

    Number::Number(double value) {
        this->value = value;
    }

    std::string Number::toString() {
        return std::to_string(value);
    }

    Number::~Number() = default;

    String::String(std::string value) {
        this->value = std::move(value);
    }

    std::string String::toString() {
        return value;
    }

    String::~String() = default;
}