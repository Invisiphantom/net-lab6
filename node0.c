#include <stdio.h>
#include <string.h>
#include "prog3.h"

extern int TRACE;
extern float clocktime;

// 结点0 -> 结点{0,1,2,3}
const int connectcosts0[4] = { 0, 1, 3, 7 };

static struct distance_table dt0; // 结点0的距离表
static int min_cost0[4];          // 结点0到达各点的最小距离

// TODO: 初始化结点
void rtinit0()
{
    // 初始化距离表为无穷大
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            dt0.costs[i][j] = INF;

    // 设置直接距离
    for (int i = 0; i < 4; i++)
        dt0.costs[i][i] = connectcosts0[i];

    // 更新最小距离
    for (int i = 0; i < 4; i++)
        min_cost0[i] = MIN4(dt0.costs[i]);

    // 打印距离表
    printdt0();

    // 发送最小距离数据包
    sendpkt(0, min_cost0);
}

// TODO: 更新结点
void rtupdate0(struct rtpkt* pkt)
{
    int src = pkt->sourceid;
    int mincost[4];
    memcpy(mincost, pkt->mincost, 4 * sizeof(int));
    printf("> rtupdate0: %.3f [%d->0] (%d,%d,%d,%d)\n", clocktime, src, mincost[0], mincost[1], mincost[2], mincost[3]);

    // 更新经过src到每个目的地的最小距离
    for (int i = 0; i < 4; i++) {
        int cost = min_cost0[src] + mincost[i];
        if (cost < dt0.costs[i][src])
            dt0.costs[i][src] = cost;
    }

    printdt0();

    // 经过所有点 更新最小距离
    int hasChange = 0;
    for (int i = 0; i < 4; i++) {
        int old_cost = min_cost0[i];
        min_cost0[i] = MIN4(dt0.costs[i]);
        if (min_cost0[i] < old_cost)
            hasChange = 1;
    }

    if (hasChange == 1) {
        printf("> rtupdate0: 执行更新\n");
        printdt0();
        sendpkt(0, min_cost0);
    } else
        printf("> rtupdate0: 未更新\n");
}

void printdt0()
{
    printf("\n");
    printf("==============================\n");
    printf("    D0|         via           \n");
    printf(" %3.3f|    1     2    3    min\n", clocktime);
    printf("------|-----------------------\n");
    printf("     1|  %3d   %3d  %3d  ->%3d\n", dt0.costs[1][1], dt0.costs[1][2], dt0.costs[1][3], min_cost0[1]);
    printf("dest 2|  %3d   %3d  %3d  ->%3d\n", dt0.costs[2][1], dt0.costs[2][2], dt0.costs[2][3], min_cost0[2]);
    printf("     3|  %3d   %3d  %3d  ->%3d\n", dt0.costs[3][1], dt0.costs[3][2], dt0.costs[3][3], min_cost0[3]);
    printf("==============================\n");
    printf("\n");
}