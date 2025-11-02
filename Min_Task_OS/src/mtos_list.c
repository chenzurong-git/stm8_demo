/*
 * Min_Task_OS - 轻量级任务操作系统
 * 链表模块实现文件
 *
 * Copyright (c) 2024
 */

#include "mtos_list.h"

/**
 * @brief 初始化链表节点
 * @param node 待初始化的节点指针
 */
void mtos_list_node_init(mtos_list_node_t *node)
{
    if (node != NULL)
    {
        node->next = NULL;
    }
}

/**
 * @brief 初始化链表
 * @param list 待初始化的链表指针
 */
void mtos_list_init(mtos_list_t *list)
{
    if (list != NULL)
    {
        list->head = NULL;
        list->tail = NULL;
        list->size = 0;
    }
}

/**
 * @brief 判断链表是否为空
 * @param list 链表指针
 * @return 1表示链表为空，0表示链表非空
 */
int mtos_list_is_empty(const mtos_list_t *list)
{
    return (list != NULL && list->head == NULL) ? 1 : 0;
}

/**
 * @brief 获取链表长度
 * @param list 链表指针
 * @return 链表节点数量
 */
size_t mtos_list_size(const mtos_list_t *list)
{
    return (list != NULL) ? list->size : 0;
}

/**
 * @brief 在链表头部插入节点
 * @param list 链表指针
 * @param node 待插入的节点指针
 */
void mtos_list_insert_head(mtos_list_t *list, mtos_list_node_t *node)
{
    if (list == NULL || node == NULL)
    {
        return;
    }

    // 确保节点未被其他链表引用
    node->next = NULL;

    if (list->head == NULL)
    {
        // 链表为空，新节点是第一个节点
        list->head = node;
        list->tail = node;
        node->next = NULL;
    }
    else
    {
        // 在头部插入节点
        node->next = list->head;
        list->head = node;
    }

    list->size++;
}

/**
 * @brief 在链表尾部插入节点
 * @param list 链表指针
 * @param node 待插入的节点指针
 */
void mtos_list_insert_tail(mtos_list_t *list, mtos_list_node_t *node)
{
    if (list == NULL || node == NULL)
    {
        return;
    }

    // 确保节点未被其他链表引用
    node->next = NULL;

    if (list->tail == NULL)
    {
        // 链表为空，新节点是第一个节点
        list->head = node;
        list->tail = node;
        node->next = NULL;
    }
    else
    {
        // 在尾部插入节点
        list->tail->next = node;
        list->tail = node;
    }

    list->size++;
}

/* 删除前向插入函数 - 单向链表不再支持在任意位置前插入 */
/* 如需在任意位置前插入，需要从头开始遍历查找前驱节点 */

/**
 * @brief 在指定节点之后插入新节点
 * @param list 链表指针
 * @param pos 插入位置的节点指针
 * @param node 待插入的节点指针
 */
void mtos_list_insert_after(mtos_list_t *list, mtos_list_node_t *pos, mtos_list_node_t *node)
{
    if (list == NULL || pos == NULL || node == NULL)
    {
        return;
    }

    // 确保节点未被其他链表引用
    node->next = NULL;

    if (pos == list->tail)
    {
        // 在尾部插入
        mtos_list_insert_tail(list, node);
    }
    else
    {
        // 在中间位置插入
        node->next = pos->next;
        pos->next = node;
        list->size++;
    }
}

/**
 * @brief 从链表中删除指定节点
 * @param list 链表指针
 * @param node 待删除的节点指针
 * @return 被删除的节点指针，如果删除失败返回NULL
 */
mtos_list_node_t *mtos_list_remove_node(mtos_list_t *list, mtos_list_node_t *node)
{
    if (list == NULL || node == NULL || list->head == NULL)
    {
        return NULL;
    }

    // 对于单向链表，需要找到待删除节点的前驱节点
    if (node == list->head)
    {
        // 删除头节点
        list->head = node->next;

        // 如果是最后一个节点，同时更新tail
        if (node == list->tail)
        {
            list->tail = NULL;
        }
    }
    else
    {
        // 查找前驱节点
        mtos_list_node_t *prev_node = list->head;
        while (prev_node != NULL && prev_node->next != node)
        {
            prev_node = prev_node->next;
        }

        if (prev_node == NULL)
        {
            // 节点不在链表中
            return NULL;
        }

        // 更新前驱节点的next指针
        prev_node->next = node->next;

        // 如果删除的是尾节点，更新tail
        if (node == list->tail)
        {
            list->tail = prev_node;
        }
    }

    // 清空节点的链接
    node->next = NULL;

    list->size--;
    return node;
}

/**
 * @brief 删除链表的头节点
 * @param list 链表指针
 * @return 被删除的头节点指针，如果链表为空返回NULL
 */
mtos_list_node_t *mtos_list_remove_head(mtos_list_t *list)
{
    if (list == NULL || list->head == NULL)
    {
        return NULL;
    }

    mtos_list_node_t *node = list->head;

    if (list->head == list->tail)
    {
        // 链表只有一个节点
        list->head = NULL;
        list->tail = NULL;
    }
    else
    {
        list->head = node->next;
    }

    // 清空节点的链接
    node->next = NULL;

    list->size--;
    return node;
}

/**
 * @brief 删除链表的尾节点
 * @param list 链表指针
 * @return 被删除的尾节点指针，如果链表为空返回NULL
 */
mtos_list_node_t *mtos_list_remove_tail(mtos_list_t *list)
{
    if (list == NULL || list->tail == NULL)
    {
        return NULL;
    }

    mtos_list_node_t *node = list->tail;

    if (list->head == list->tail)
    {
        // 链表只有一个节点
        list->head = NULL;
        list->tail = NULL;
    }
    else
    {
        // 对于单向链表，需要找到尾节点的前驱节点
        mtos_list_node_t *prev_node = list->head;
        while (prev_node->next != list->tail)
        {
            prev_node = prev_node->next;
        }

        list->tail = prev_node;
        list->tail->next = NULL;
    }

    // 清空节点的链接
    node->next = NULL;

    list->size--;
    return node;
}
