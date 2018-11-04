#include <stdio.h>
#include "project3.h"

extern int TraceLevel;
extern float clocktime;

struct distance_table {
  int costs[MAX_NODES][MAX_NODES];
};
struct distance_table dt1;
struct NeighborCosts   *neighbor1;

/* students to write the following two routines, and maybe some others */
void printdt1( int MyNodeNumber, struct NeighborCosts *neighbor,
               struct distance_table *dtptr );
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
int nodesCantSend1[MAX_NODES];


void rtinit1() {
    printf("rtinit1 called at %.3f \n", clocktime);

    int i,c,r;
    neighbor1 = getNeighborCosts(1);

    //printf("Neighbor costs for 1 are %d %d %d %d \n", neighbor1->NodeCosts[0], neighbor1->NodeCosts[1], neighbor1->NodeCosts[2], neighbor1->NodeCosts[3]);

    for(r = 0; r < MAX_NODES; ++r){
        nodesCantSend1[r] = NO;
        if(neighbor1->NodeCosts[r] == INFINITY)
            nodesCantSend1[r] = YES;

        for(c = 0; c < MAX_NODES; ++c){
            dt1.costs[r][c] = INFINITY;
        }
    }

    for(i = 0; i < MAX_NODES; ++i){
        dt1.costs[i][i] = neighbor1->NodeCosts[i];
    }



    struct RoutePacket pkt;
    pkt.sourceid = 1;

    for (r = 0; r < MAX_NODES; ++r) {
        pkt.mincost[r] = INFINITY;
        for (c = 0; c < MAX_NODES; ++c) {
            pkt.mincost[r] = MIN(pkt.mincost[r], dt1.costs[r][c]);
        }
    }

    //int isPrintedInit1 = NO;
    for (i = 0; i < MAX_NODES; ++i) {
        //UPDATE HERE
        if(i != 1 && nodesCantSend1[i] != YES){
            pkt.destid = i;
            //if (isPrintedInit1 == NO){
            printf("NODE 1 to NODE %d at %.3f Initial Costs: %d %d %d %d\n",i, clocktime, pkt.mincost[0], pkt.mincost[1], pkt.mincost[2], pkt.mincost[3]);
            //    isPrintedInit1 = YES;
            //}
            toLayer2(pkt);
        }
    }
    printdt1(1, neighbor1, &dt1);
}


void rtupdate1( struct RoutePacket *rcvdpkt ) {
    printf("rtupdate1 called at %.3f \n", clocktime);

    int i,c,r;
    int isUpdate = NO;
    int src = rcvdpkt->sourceid;

    for (i = 0; i < MAX_NODES; ++i) {
        int currentCost = dt1.costs[i][src];
        int altCost = dt1.costs[src][src] + rcvdpkt->mincost[i];

        if(currentCost > altCost){
            dt1.costs[i][src] = altCost;
            isUpdate = YES;
        }
    }

    if(isUpdate){
        struct RoutePacket pkt;
        pkt.sourceid = 1;

        for (r = 0; r < MAX_NODES; ++r) {
            pkt.mincost[r] = INFINITY;
            for (c = 0; c < MAX_NODES; ++c) {
                pkt.mincost[r] = MIN(pkt.mincost[r], dt1.costs[r][c]);
            }
        }

        //int isPrintedUpdt1 = NO;
        for (i = 0; i < MAX_NODES; ++i) {
            if(i != 1 && nodesCantSend1[i] != YES){
                pkt.destid = i;
                //if (isPrintedUpdt1 == NO){
                printf("NODE 1 to NODE %d at %.3f Updated Costs: %d %d %d %d\n",i, clocktime, pkt.mincost[0], pkt.mincost[1], pkt.mincost[2], pkt.mincost[3]);
                //    isPrintedUpdt1 = YES;
                //}
                toLayer2(pkt);
            }
        }
        printdt1(1, neighbor1, &dt1);

    }
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
void printdt1( int MyNodeNumber, struct NeighborCosts *neighbor, 
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
}    // End of printdt1

