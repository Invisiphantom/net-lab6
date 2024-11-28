#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "prog3.h"

int TRACE = 1;

struct event {
    float evtime;           // 事件时间
    int evtype;             // 事件类型
    int eventity;           // 事件实体
    struct rtpkt* rtpktptr; // 关联的数据包指针
    struct event* prev;
    struct event* next;
};

// 事件队列
struct event* evlist = NULL;

// 可能的事件类型
#define FROM_LAYER2 2 // 数据包到达链路层

// 当前时间
float clocktime = 0.000;

// 返回随机浮点数 [0,1]
float jimsrand() { return (float)rand() / RAND_MAX; }

// 插入事件到事件队列
void insertevent(struct event* ev)
{
    struct event* q = evlist;

    // 如果队列为空
    if (q == NULL) {
        evlist = ev;
        ev->next = NULL;
        ev->prev = NULL;
    }

    else {
        // 找到首个更迟的事件q
        struct event* q_tmp = q;
        while (q != NULL && ev->evtime > q->evtime) {
            q_tmp = q;
            q = q->next;
        }

        // 插入到队列尾部
        if (q == NULL) {
            q_tmp->next = ev;
            ev->prev = q_tmp;
            ev->next = NULL;
        }

        // 插入到队列头部
        else if (q == evlist) {
            ev->next = evlist;
            ev->prev = NULL;
            ev->next->prev = ev;
            evlist = ev;
        }

        // 插入到队列中间
        // q_tmp -> ev -> q
        else {
            ev->next = q;
            ev->prev = q->prev;
            q->prev->next = ev;
            q->prev = ev;
        }
    }
}

// 调试打印事件队列
void printevlist()
{
    printf("--------------\n");
    for (struct event* q = evlist; q != NULL; q = q->next)
        printf("Event time=%f type=%d entity=%d\n", q->evtime, q->evtype, q->eventity);
    printf("--------------\n");
}

const int connectcosts[4][4] = {
    { 0, 1, 3, 7 },
    { 1, 0, 1, 999 },
    { 3, 1, 0, 2 },
    { 7, 999, 2, 0 },
};

// 用于每个结点发送数据包
void sendpkt(int src, int mincost[])
{
    struct rtpkt pkt;
    pkt.sourceid = src;
    memcpy(pkt.mincost, mincost, 4 * sizeof(int));

    for (int i = 0; i < 4; i++) {
        // 发送到相邻可达节点
        int cost = connectcosts[src][i];
        if (cost != 0 && cost != 999) {
            pkt.destid = i;
            tolayer2(pkt);
            printf("sendpkt: %.3f [%d->%d] (%d,%d,%d,%d)\n", clocktime, src, i, mincost[0], mincost[1], mincost[2],
                mincost[3]);
        }
    }
}

// 添加数据包的发送事件
void tolayer2(struct rtpkt pkt)
{
    // 确保数据包合法
    assert(pkt.sourceid >= 0 && pkt.sourceid < 4);
    assert(pkt.destid >= 0 && pkt.destid < 4);
    assert(pkt.sourceid != pkt.destid);
    assert(connectcosts[pkt.sourceid][pkt.destid] != 999);

    // 拷贝数据包
    struct rtpkt* mypkt = (struct rtpkt*)malloc(sizeof(struct rtpkt));
    mypkt->sourceid = pkt.sourceid;
    mypkt->destid = pkt.destid;
    memcpy(mypkt->mincost, pkt.mincost, 4 * sizeof(int));

    // 构造事件
    struct event* ev = (struct event*)malloc(sizeof(struct event));
    ev->evtype = FROM_LAYER2;  // 来自链路层
    ev->eventity = pkt.destid; // 目的节点
    ev->rtpktptr = mypkt;      // 数据包

    // 设置随机到达时间
    // 确保相同目的地的包有序到达
    float lastime = clocktime;
    for (struct event* ei = evlist; ei != NULL; ei = ei->next)
        if ((ei->evtype == FROM_LAYER2 && ei->eventity == ev->eventity))
            lastime = ei->evtime;
    ev->evtime = lastime + 2.0 * jimsrand();

    // 插入到事件队列
    insertevent(ev);
}

void main()
{
    float sum, avg;
    struct event* ev;

    printf("Enter TRACE:");
    scanf("%d", &TRACE);

    // 初始化随机数
    srand(9999);
    sum = 0.0;
    for (int i = 0; i < 1000; i++)
        sum = sum + jimsrand();
    avg = sum / 1000.0;
    if (avg < 0.25 || avg > 0.75) {
        printf("jimsrand: 生成的数不够随机\n");
        exit(0);
    }

    // 初始化当前时间
    clocktime = 0.0;

    // 初始化各结点
    rtinit0();
    rtinit1();
    rtinit2();
    rtinit3();

    while (1) {
        // 取出队列首的事件
        struct event* eventptr = evlist;

        // 如果已经没有事件, 则退出
        if (eventptr == NULL)
            goto terminate;

        // 从队列中移除事件
        evlist = evlist->next;

        // 如果队列不为空, 则更新队列头
        if (evlist != NULL)
            evlist->prev = NULL;

        // 打印事件信息
        if (TRACE > 1) {
            printf("MAIN: rcv event, t=%.3f, at %d", eventptr->evtime, eventptr->eventity);
            if (eventptr->evtype == FROM_LAYER2) {
                printf(" src:%2d,", eventptr->rtpktptr->sourceid);
                printf(" dest:%2d,", eventptr->rtpktptr->destid);
                printf(" contents: %3d %3d %3d %3d\n", eventptr->rtpktptr->mincost[0], eventptr->rtpktptr->mincost[1],
                    eventptr->rtpktptr->mincost[2], eventptr->rtpktptr->mincost[3]);
            }
        }

        // 更新当前时间
        clocktime = eventptr->evtime;

        // 处理事件
        if (eventptr->evtype == FROM_LAYER2) {
            if (eventptr->eventity == 0)
                rtupdate0(eventptr->rtpktptr);
            else if (eventptr->eventity == 1)
                rtupdate1(eventptr->rtpktptr);
            else if (eventptr->eventity == 2)
                rtupdate2(eventptr->rtpktptr);
            else if (eventptr->eventity == 3)
                rtupdate3(eventptr->rtpktptr);
            else {
                printf("Panic: unknown event entity\n");
                exit(0);
            }
        } else {
            printf("Panic: unknown event type\n");
            exit(0);
        }

        // 释放数据包内存
        if (eventptr->evtype == FROM_LAYER2)
            free(eventptr->rtpktptr);

        // 释放事件内存
        free(eventptr);
    }

terminate:
    printf("\n======模拟结束======\n");
    printf("当前时间: %.3f\n", clocktime);
    printdt0();
    printdt1();
    printdt2();
    printdt3();
}