#include <stdio.h>
#include <string.h>
#include "prog3.h"

extern int TRACE;
extern float clocktime;

// 结点3 -> 结点{0,1,2,3}
const int connectcosts3[4] = { 7, 999, 2, 0 };

static struct distance_table dt3; // 结点3的距离表
static int min_cost3[4];          // 结点3到达各点的最小距离

// 初始化结点
void rtinit3()
{
    // 初始化距离表为无穷大
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            dt3.costs[i][j] = INF;

    // 设置直接距离
    for (int i = 0; i < 4; i++)
        dt3.costs[i][i] = connectcosts3[i];

    // 更新最小距离
    for (int i = 0; i < 4; i++)
        min_cost3[i] = MIN4(dt3.costs[i]);

    // 打印距离表
    printdt3();

    // 发送最小距离数据包
    sendpkt(3, min_cost3);
}

// 更新结点
void rtupdate3(struct rtpkt* pkt)
{
    int src = pkt->sourceid;
    int mincost[4];
    memcpy(mincost, pkt->mincost, 4 * sizeof(int));
    printf("> rtupdate3: %.3f [%d->3] (%d,%d,%d,%d)\n", clocktime, src, mincost[0], mincost[1], mincost[2], mincost[3]);

    // 更新经过src到每个目的地的最小距离
    for (int i = 0; i < 4; i++) {
        int cost = min_cost3[src] + mincost[i];
        if (cost < dt3.costs[i][src])
            dt3.costs[i][src] = cost;
    }

    // 经过所有点 更新最小距离
    int hasChange = 0;
    for (int i = 0; i < 4; i++) {
        int old_cost = min_cost3[i];
        min_cost3[i] = MIN4(dt3.costs[i]);
        if (min_cost3[i] < old_cost)
            hasChange = 1;
    }

    if (hasChange == 1) {
        printf("> rtupdate3: 执行更新\n");
        printdt3();
        sendpkt(3, min_cost3);
    } else
        printf("> rtupdate3: 未更新\n");
}

void printdt3()
{
    printf("\n");
    printf("==============================\n");
    printf("    D3|         via           \n");
    printf(" %3.3f|    0     1    2    min\n", clocktime);
    printf("------|-----------------------\n");
    printf("     0|  %3d   %3d  %3d  ->%3d\n", dt3.costs[0][0], dt3.costs[0][1], dt3.costs[0][2], min_cost3[0]);
    printf("dest 1|  %3d   %3d  %3d  ->%3d\n", dt3.costs[1][0], dt3.costs[1][1], dt3.costs[1][2], min_cost3[1]);
    printf("     2|  %3d   %3d  %3d  ->%3d\n", dt3.costs[2][0], dt3.costs[2][1], dt3.costs[2][2], min_cost3[2]);
    printf("==============================\n");
    printf("\n");
}
