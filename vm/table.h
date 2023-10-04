//
// Created by hlx on 2023/10/4.
//

#ifndef CPPLOX_TABLE_H
#define CPPLOX_TABLE_H

#include "common.h"
#include "value.h"

namespace cpplox{

    // 哈希节点
    typedef struct {
        ObjString *key; // 字符串对象做完key值
        Value value;    // 值对象可以是任意元素
    } Entry;

    // 哈希表
    typedef struct {
        int count;      // 当前元素数
        int capacity;   // 最大元素数
        Entry *entries; // 哈希节点数组
    } Table;

// 初始化表
    void initTable(Table *table);

// 释放表
    void freeTable(Table *table);

// 获取key对应值
    bool tableGet(Table *table, ObjString *key, Value *value);

// 插入哈希表
    bool tableSet(Table *table, ObjString *key, Value value);

// 移除键值对
    bool tableDelete(Table *table, ObjString *key);

    // 复制表
    void tableAddAll(Table *from, Table *to);

    // 在表中寻找字符串节点
    ObjString *tableFindString(Table *table, const char *chars, int length, uint32_t hash);

    // 及时移除不能访问的字符串
    void tableRemoveWhite(Table* table);

    // 标记表
    void markTable(Table* table);
}

#endif //CPPLOX_TABLE_H
