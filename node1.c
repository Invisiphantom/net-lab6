#include <stdio.h>
#include <string.h>
#include "prog3.h"

extern int TRACE;
extern float clocktime;

// 结点1 -> 结点{0,1,2,3}
const int connectcosts1[4] = { 1, 0, 1, 999 };

static struct distance_table dt1; // 结点1的距离表
static int min_cost1[4];          // 结点1到达各点的最小距离

// 初始化结点
void rtinit1()
{
    // 初始化距离表为无穷大
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            dt1.costs[i][j] = INF;

    // 设置直接距离
    for (int i = 0; i < 4; i++)
        dt1.costs[i][i] = connectcosts1[i];

    // 更新最小距离
    for (int i = 0; i < 4; i++)
        min_cost1[i] = MIN4(dt1.costs[i]);

    // 打印距离表
    printdt1();

    // 发送最小距离数据包
    sendpkt(1, min_cost1);
}

// 更新结点
void rtupdate1(struct rtpkt* pkt)
{
    int src = pkt->sourceid;
    int mincost[4];
    memcpy(mincost, pkt->mincost, 4 * sizeof(int));
    printf("> rtupdate1: %.3f [%d->1] (%d,%d,%d,%d)\n", clocktime, src, mincost[0], mincost[1], mincost[2], mincost[3]);

    // 更新经过src到每个目的地的最小距离
    for (int i = 0; i < 4; i++) {
        int cost = min_cost1[src] + mincost[i];
        if (cost < dt1.costs[i][src])
            dt1.costs[i][src] = cost;
    }

    printdt1();

    // 经过所有点 更新最小距离
    int hasChange = 0;
    for (int i = 0; i < 4; i++) {
        int old_cost = min_cost1[i];
        min_cost1[i] = MIN4(dt1.costs[i]);
        if (min_cost1[i] < old_cost)
            hasChange = 1;
    }

    if (hasChange == 1) {
        printf("> rtupdate1: 执行更新\n");
        printdt1();
        sendpkt(1, min_cost1);
    } else
        printf("> rtupdate1: 未更新\n");
}

void printdt1()
{
    printf("\n");
    printf("==============================\n");
    printf("    D1|         via           \n");
    printf(" %3.3f|    0     2    3    min\n", clocktime);
    printf("------|-----------------------\n");
    printf("     0|  %3d   %3d  %3d  ->%3d\n", dt1.costs[0][0], dt1.costs[0][2], dt1.costs[0][3], min_cost1[0]);
    printf("dest 2|  %3d   %3d  %3d  ->%3d\n", dt1.costs[2][0], dt1.costs[2][2], dt1.costs[2][3], min_cost1[2]);
    printf("     3|  %3d   %3d  %3d  ->%3d\n", dt1.costs[3][0], dt1.costs[3][2], dt1.costs[3][3], min_cost1[3]);
    printf("==============================\n");
    printf("\n");
}
