#include <stdio.h>
#include <string.h>
#include "prog3.h"

extern int TRACE;
extern float clocktime;

// 结点2 -> 结点{0,1,2,3}
const int connectcosts2[4] = { 3, 1, 0, 2 };

static struct distance_table dt2; // 结点2的距离表
static int min_cost2[4];          // 结点2到达各点的最小距离

// 初始化结点
void rtinit2()
{
    // 初始化距离表为无穷大
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            dt2.costs[i][j] = INF;

    // 设置直接距离
    for (int i = 0; i < 4; i++)
        dt2.costs[i][i] = connectcosts2[i];

    // 更新最小距离
    for (int i = 0; i < 4; i++)
        min_cost2[i] = MIN4(dt2.costs[i]);

    // 打印距离表
    printdt2();

    // 发送最小距离数据包
    sendpkt(2, min_cost2);
}

// 更新结点
void rtupdate2(struct rtpkt* pkt)
{
    int src = pkt->sourceid;
    int mincost[4];
    memcpy(mincost, pkt->mincost, 4 * sizeof(int));
    printf("> rtupdate2: %.3f [%d->2] (%d,%d,%d,%d)\n", clocktime, src, mincost[0], mincost[1], mincost[2], mincost[3]);

    // 更新经过src到每个目的地的最小距离
    for (int i = 0; i < 4; i++) {
        int cost = min_cost2[src] + mincost[i];
        if (cost < dt2.costs[i][src])
            dt2.costs[i][src] = cost;
    }

    printdt2();

    // 经过所有点 更新最小距离
    int hasChange = 0;
    for (int i = 0; i < 4; i++) {
        int old_cost = min_cost2[i];
        min_cost2[i] = MIN4(dt2.costs[i]);
        if (min_cost2[i] < old_cost)
            hasChange = 1;
    }

    if (hasChange == 1) {
        printf("> rtupdate2: 执行更新\n");
        printdt2();
        sendpkt(2, min_cost2);
    } else
        printf("> rtupdate2: 未更新\n");
}

void printdt2()
{
    printf("\n");
    printf("==============================\n");
    printf("    D2|         via           \n");
    printf(" %3.3f|    0     1    3    min\n", clocktime);
    printf("------|-----------------------\n");
    printf("     0|  %3d   %3d  %3d  ->%3d\n", dt2.costs[0][0], dt2.costs[0][1], dt2.costs[0][3], min_cost2[0]);
    printf("dest 1|  %3d   %3d  %3d  ->%3d\n", dt2.costs[1][0], dt2.costs[1][1], dt2.costs[1][3], min_cost2[1]);
    printf("     3|  %3d   %3d  %3d  ->%3d\n", dt2.costs[3][0], dt2.costs[3][1], dt2.costs[3][3], min_cost2[3]);
    printf("==============================\n");
    printf("\n");
}
