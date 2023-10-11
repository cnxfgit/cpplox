//
// Created by hlx on 2023/10/4.
//

#ifndef CPPLOX_DEBUG_H
#define CPPLOX_DEBUG_H

#include "chunk.h"

namespace cpplox{

    // 反汇编字节码块
    void disassembleChunk(Chunk* chunk, const char* name);

    // 反汇编说明
    int disassembleInstruction(Chunk* chunk, int offset);

}

#endif //CPPLOX_DEBUG_H
