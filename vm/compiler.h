//
// Created by hlx on 2023/10/4.
//

#ifndef CPPLOX_COMPILER_H
#define CPPLOX_COMPILER_H

#include "vm.h"

namespace cpplox {

    // 编译
    ObjFunction *compile(const char *source);

    // 标记编译根对象
    void markCompilerRoots();
}

#endif //CPPLOX_COMPILER_H
