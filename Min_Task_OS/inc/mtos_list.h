/*
 * Min_Task_OS - 轻量级任务操作系统
 * 链表模块头文件
 *
 * Copyright (c) 2024
 */

#ifndef __MTOS_LIST_H__
#define __MTOS_LIST_H__

#include <stddef.h>

/* 链表节点结构体 */
typedef struct mtos_list_node
{
    struct mtos_list_node *next;  /* 指向下一个节点 */
} mtos_list_node_t;

/* 链表结构体（用于简化链表操作） */
typedef struct mtos_list
{
    mtos_list_node_t *head;       /* 链表头节点 */
    mtos_list_node_t *tail;       /* 链表尾节点 */
    size_t size;                  /* 链表节点数量 */
} mtos_list_t;

/* 初始化链表节点 */
#define MTOS_LIST_NODE_INIT(node)  \
    do {                           \
        (node)->next = NULL;       \
    } while(0)

/* 初始化链表 */
#define MTOS_LIST_INIT(list)        \
    do {                           \
        (list)->head = NULL;       \
        (list)->tail = NULL;       \
        (list)->size = 0;          \
    } while(0)

/**
 * @brief 从链表节点获取包含该节点的结构体指针
 * @param ptr 节点指针
 * @param type 包含该节点的结构体类型
 * @param member 结构体中节点成员的名称
 */
#define MTOS_LIST_ENTRY(ptr, type, member) \
    ((type *)((uint8_t *)(ptr) - (uint8_t *)(&((type *)0)->member)))

/* 链表操作函数声明 */

/* 初始化链表节点 */
void mtos_list_node_init(mtos_list_node_t *node);

/* 初始化链表 */
void mtos_list_init(mtos_list_t *list);

/* 判断链表是否为空 */
int mtos_list_is_empty(const mtos_list_t *list);

/* 获取链表长度 */
size_t mtos_list_size(const mtos_list_t *list);

/* 在链表头部插入节点 */
void mtos_list_insert_head(mtos_list_t *list, mtos_list_node_t *node);

/* 在链表尾部插入节点 */
void mtos_list_insert_tail(mtos_list_t *list, mtos_list_node_t *node);

/* 在指定节点之后插入新节点 */
void mtos_list_insert_after(mtos_list_t *list, mtos_list_node_t *pos, mtos_list_node_t *node);

/* 从链表中删除指定节点 */
mtos_list_node_t *mtos_list_remove_node(mtos_list_t *list, mtos_list_node_t *node);

/* 删除并返回链表头节点 */
mtos_list_node_t *mtos_list_remove_head(mtos_list_t *list);

/* 删除并返回链表尾节点 */
mtos_list_node_t *mtos_list_remove_tail(mtos_list_t *list);

/**
 * @brief 遍历链表的宏定义（单向链表版本）
 * @param list 链表指针
 * @param node 遍历过程中使用的节点指针
 */
#define MTOS_LIST_FOR_EACH(list, node) \
    for (node = (list)->head; node != NULL; node = node->next)

/**
 * @brief 安全遍历链表的宏定义（可以在遍历过程中删除节点）
 * @param list 链表指针
 * @param node 遍历过程中使用的节点指针
 * @param temp 临时节点指针，用于保存下一个节点
 */
#define MTOS_LIST_FOR_EACH_SAFE(list, node, temp) \
    for (node = (list)->head, temp = (node ? node->next : NULL); \
         node != NULL; \
         node = temp, temp = (node ? node->next : NULL))

#endif /* __MTOS_LIST_H */
