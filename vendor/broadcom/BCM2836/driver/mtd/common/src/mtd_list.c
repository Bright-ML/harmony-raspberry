#include "stdio.h"
#include "stdlib.h"
#include "string.h"

typedef struct Node {
    char *type;           /* flash type */
    int status;           /* flash status */
    struct MtdDev *mtd; /* mtd_info struct */
    struct Node *next;    /* next mtd_info struct */
} Lnode, *linklist;

linklist head;
#if 0 
struct MtdDev *GetMtd(const char *type)
{
    linklist p;
    if ((!type) || (!head)) {
        return NULL;
    }
    p = head->next;
    while (p) {
        if (strcmp(type, p->type) == 0) {
            p->status++;
            return p->mtd;
        }
        p = p->next;
    }
    return NULL;
}
#endif
int get_mtd_info(const char *type)
{
    linklist p;
    if ((!type) || (!head)) {
        return -1;
    }
    p = head->next;
    while (p) {
        if (strcmp(type, p->type) == 0) {
            return 0;
        }
        p = p->next;
    }
    return -1;
}
int FreeMtd(struct MtdDev *mtd)
{
    linklist p;
    if ((!mtd) || (!head)) {
        return -1;
    }
    p = head->next;
    while (p) {
        if (p->mtd == mtd) {
            p->status--;
            return 0;
        }
        p = p->next;
    }
    return -1;
}

int del_mtd_list(struct MtdDev *mtd)
{
    linklist p, q;
    if ((!mtd) || (!head)) {
        return -1;
    }
    p = head->next;
    q = head;
    while (p) {
        if (p->mtd == mtd) {
            if (!p->status) {
                q->next = p->next;
                free(p);
                return 0;
            } else {
                return -1;
            }
        }
        q = p;
        p = p->next;
    }
    return -1;
}
void add_mtd_list(char *type, struct MtdDev *mtd)
{
    linklist p, q;
    if ((!mtd) || (!type) || (!head)) {
        return;
    }
    p = head->next;
    q = (linklist)zalloc(sizeof(Lnode));
    if (!q) {
        return;
    }
    q->type = type;
    q->mtd = mtd;
    q->status = 0;
    head->next = q;
    q->next = p;
}
int mtd_init_list(void)
{
    head = (linklist)zalloc(sizeof(Lnode));
    if (!head) {
        return -1;
    }
    head->next = NULL;
    head->mtd = NULL;
    head->type = NULL;
    head->status = 0;
    return 0;
}
