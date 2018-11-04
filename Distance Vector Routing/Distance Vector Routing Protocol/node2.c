#include <stdio.h>
#include "project3.h"

extern int TraceLevel;
extern float clocktime;

struct distance_table {
  int costs[MAX_NODES][MAX_NODES];
};
struct distance_table dt2;
struct NeighborCosts   *neighbor2;

/* students to write the following two routines, and maybe some others */
void printdt2( int MyNodeNumber, struct NeighborCosts *neighbor,
               struct distance_table *dtptr );
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
int nodesCantSend2[MAX_NODES];


void rtinit2() {
    printf("rtinit2 called at %.3f \n", clocktime);
    int i,c,r;

    neighbor2 = getNeighborCosts(2);

    for(r = 0; r < MAX_NODES; ++r){
        nodesCantSend2[r] = NO;
        if(neighbor2->NodeCosts[r] == INFINITY)
            nodesCantSend2[r] = YES;


        for(c = 0; c < MAX_NODES; ++c){
            dt2.costs[r][c] = INFINITY;
        }
    }

    for(i = 0; i < MAX_NODES; ++i){
        dt2.costs[i][i] = neighbor2->NodeCosts[i];
    }


    struct RoutePacket pkt;
    pkt.sourceid = 2;

    for (r = 0; r < MAX_NODES; ++r) {
        pkt.mincost[r] = INFINITY;
        for (c = 0; c < MAX_NODES; ++c) {
            pkt.mincost[r] = MIN(pkt.mincost[r], dt2.costs[r][c]);
        }
    }

    //int isPrintedInit2 = NO;
    for (i = 0; i < MAX_NODES; ++i) {
        if(i != 2 && nodesCantSend2[i] != YES){
            pkt.destid = i;
            //if (isPrintedInit2 == NO){
            printf("NODE 2 to NODE %d at %.3f Initial Costs: %d %d %d %d\n",i, clocktime, pkt.mincost[0], pkt.mincost[1], pkt.mincost[2], pkt.mincost[3]);
            //    isPrintedInit2 = YES;
            //}
            toLayer2(pkt);
        }
    }
    printdt2(2, neighbor2, &dt2);
}


void rtupdate2( struct RoutePacket *rcvdpkt ) {
    printf("rtupdate2 called at %.3f \n", clocktime);
    int i,c,r;

    int isUpdate = NO;
    int src = rcvdpkt->sourceid;

    for (i = 0; i < MAX_NODES; ++i) {
        int currentCost = dt2.costs[i][src];
        int altCost = dt2.costs[src][src] + rcvdpkt->mincost[i];

        if(currentCost > altCost){
            dt2.costs[i][src] = altCost;
            isUpdate = YES;
        }
    }

    if(isUpdate){
        struct RoutePacket pkt;
        pkt.sourceid = 2;
        for (r = 0; r < MAX_NODES; ++r) {
            pkt.mincost[r] = INFINITY;
            for (c = 0; c < MAX_NODES; ++c) {
                pkt.mincost[r] = MIN(pkt.mincost[r], dt2.costs[r][c]);
            }
        }

        //int isPrintedUpdt2 = NO;
        for (i = 0; i < MAX_NODES; ++i) {
            if(i != 2 && nodesCantSend2[i] != YES){
                pkt.destid = i;
                //if (isPrintedUpdt2 == NO){
                printf("NODE 2 to NODE %d at %.3f Updated Costs: %d %d %d %d\n",i, clocktime, pkt.mincost[0], pkt.mincost[1], pkt.mincost[2], pkt.mincost[3]);
                    //isPrintedUpdt2 = YES;
                //}
                toLayer2(pkt);
            }
        }
    }
    printdt2(2, neighbor2, &dt2);
}


/////////////////////////////////////////////////////////////////////
//  printdt
//  This routine is being supplied to you.  It is the same code in
//  each node and is tailored based on the input arguments.
//  Required arguments:
//  MyNodeNumber:  This routine assumes that you know your node
//                 number and supply it when making this call.
//  struct NeighborCosts *neighbor:  A pointer to the structure 
//                 that's supplied via a call to getNeighborCosts().
//                 It tells this print routine the configuration
//                 of nodes surrounding the node we're working on.
//  struct distance_table *dtptr: This is the running record of the
//                 current costs as seen by this node.  It is 
//                 constantly updated as the node gets new
//                 messages from other nodes.
/////////////////////////////////////////////////////////////////////
void printdt2( int MyNodeNumber, struct NeighborCosts *neighbor, 
		struct distance_table *dtptr ) {
    int       i, j;
    int       TotalNodes = neighbor->NodesInNetwork;     // Total nodes in network
    int       NumberOfNeighbors = 0;                     // How many neighbors
    int       Neighbors[MAX_NODES];                      // Who are the neighbors

    // Determine our neighbors 
    for ( i = 0; i < TotalNodes; i++ )  {
        if (( neighbor->NodeCosts[i] != INFINITY ) && i != MyNodeNumber )  {
            Neighbors[NumberOfNeighbors] = i;
            NumberOfNeighbors++;
        }
    }
    // Print the header
    printf("                via     \n");
    printf("   D%d |", MyNodeNumber );
    for ( i = 0; i < NumberOfNeighbors; i++ )
        printf("     %d", Neighbors[i]);
    printf("\n");
    printf("  ----|-------------------------------\n");

    // For each node, print the cost by travelling thru each of our neighbors
    for ( i = 0; i < TotalNodes; i++ )   {
        if ( i != MyNodeNumber )  {
            printf("dest %d|", i );
            for ( j = 0; j < NumberOfNeighbors; j++ )  {
                    printf( "  %4d", dtptr->costs[i][Neighbors[j]] );
            }
            printf("\n");
        }
    }
    printf("\n");
}    // End of printdt2

