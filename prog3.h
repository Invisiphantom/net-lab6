
#define INF 999

// 用相邻结点 计算到目的的最小距离
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MIN4(via) MIN(MIN(MIN(via[0], via[1]), via[2]), via[3])

struct rtpkt {
    int sourceid;   // 源路由器ID
    int destid;     // 目的路由器ID
    int mincost[4]; // 从源到各路由器的最小距离
};

// [目的][经过]距离表
struct distance_table {
    int costs[4][4]; // [dest][via]
};

void rtinit0();
void rtinit1();
void rtinit2();
void rtinit3();

void linkhandler0(int linkid, int newcost);
void linkhandler1(int linkid, int newcost);

void printdt0();
void printdt1();
void printdt2();
void printdt3();

void rtupdate0(struct rtpkt* rcvdpkt);
void rtupdate1(struct rtpkt* rcvdpkt);
void rtupdate2(struct rtpkt* rcvdpkt);
void rtupdate3(struct rtpkt* rcvdpkt);

void printmincost0();
void printmincost1();
void printmincost2();
void printmincost3();

void sendpkt(int src, int mincost[]);
void tolayer2(struct rtpkt packet);
