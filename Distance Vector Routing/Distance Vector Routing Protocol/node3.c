#include <stdio.h>
#include "project3.h"

extern int TraceLevel;
extern float clocktime;

struct distance_table {
  int costs[MAX_NODES][MAX_NODES];
};
struct distance_table dt3;
struct NeighborCosts   *neighbor3;

/* students to write the following two routines, and maybe some others */
void printdt3( int MyNodeNumber, struct NeighborCosts *neighbor,
               struct distance_table *dtptr );
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
int nodesCantSend3[MAX_NODES];


void rtinit3() {
    printf("rtinit3 called at %.3f \n", clocktime);

    int i,c,r;

    neighbor3 = getNeighborCosts(3);

    //printf("Neighbor costs for 3 are %d %d %d %d \n", neighbor3->NodeCosts[0], neighbor3->NodeCosts[1], neighbor3->NodeCosts[2], neighbor3->NodeCosts[3]);


    for(r = 0; r < MAX_NODES; ++r){
        nodesCantSend3[r] = NO;
        if(neighbor3->NodeCosts[r] == INFINITY)
            nodesCantSend3[r] = YES;


        for(c = 0; c < MAX_NODES; ++c){
            dt3.costs[r][c] = INFINITY;
        }
    }

    for(i = 0; i < MAX_NODES; ++i){
        dt3.costs[i][i] = neighbor3->NodeCosts[i];
    }


    struct RoutePacket pkt;
    pkt.sourceid = 3;

    for (r = 0; r < MAX_NODES; ++r) {
        pkt.mincost[r] = INFINITY;
        for (c = 0; c < MAX_NODES; ++c) {
            pkt.mincost[r] = MIN(pkt.mincost[r], dt3.costs[r][c]);
        }
    }

    //int isPrintedInit3 = NO;
    for (i = 0; i < MAX_NODES; ++i) {
        if(i != 3 && nodesCantSend3[i] != YES){
            pkt.destid = i;
            //if (isPrintedInit3 == NO){
            printf("NODE 3 to NODE %d at %.3f Initial Costs: %d %d %d %d\n",i, clocktime, pkt.mincost[0], pkt.mincost[1], pkt.mincost[2], pkt.mincost[3]);
            //    isPrintedInit3 = YES;
            //}
            toLayer2(pkt);
        }
    }
    printdt3(3, neighbor3, &dt3);

}


void rtupdate3( struct RoutePacket *rcvdpkt ) {
    printf("rtupdate3 called at %.3f \n", clocktime);

    int i,c,r;
    int isUpdate = NO;
    int src = rcvdpkt->sourceid;

    for (i = 0; i < MAX_NODES; ++i) {
        int currentCost = dt3.costs[i][src];
        int altCost = dt3.costs[src][src] + rcvdpkt->mincost[i];

        if(currentCost > altCost){
            dt3.costs[i][src] = altCost;
            isUpdate = YES;
        }
    }

    if(isUpdate){
        struct RoutePacket pkt;
        pkt.sourceid = 3;
        for (r = 0; r < MAX_NODES; ++r) {
            pkt.mincost[r] = INFINITY;
            for (c = 0; c < MAX_NODES; ++c) {
                pkt.mincost[r] = MIN(pkt.mincost[r], dt3.costs[r][c]);
            }
        }
        //int isPrintedUpdt3 = NO;

        for (i = 0; i < MAX_NODES; ++i) {
            if(i != 3 && nodesCantSend3[i] != YES){
                pkt.destid = i;
                //if (isPrintedUpdt3 == NO){
                printf("NODE 3 to NODE %d at %.3f Updated Costs: %d %d %d %d\n",i, clocktime, pkt.mincost[0], pkt.mincost[1], pkt.mincost[2], pkt.mincost[3]);
                   // isPrintedUpdt3 = YES;
                //}
                toLayer2(pkt);
            }
        }
    }
    printdt3(3, neighbor3, &dt3);
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
void printdt3( int MyNodeNumber, struct NeighborCosts *neighbor, 
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
}    // End of printdt3

