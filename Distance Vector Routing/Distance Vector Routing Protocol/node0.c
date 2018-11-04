#include <stdio.h>
#include "project3.h"

extern int TraceLevel;
extern float clocktime;

struct distance_table {
  int costs[MAX_NODES][MAX_NODES];
};
struct distance_table dt0;
struct NeighborCosts   *neighbor0;

/* students to write the following two routines, and maybe some others */


void printdt0( int MyNodeNumber, struct NeighborCosts *neighbor, struct distance_table *dtptr );    //function prototype
#define MIN(x, y) (((x) < (y)) ? (x) : (y))     //a simple function to decide the minimum of 2 numbers
int nodesCantSend0[MAX_NODES];                  //an array of nodes that cannot be contacted from this node

void rtinit0() {
    printf("rtinit0 called at %.3f \n", clocktime);

    //Loop counters
    int i,c,r;

    //first get neighbor link costs
    neighbor0 = getNeighborCosts(0);

    /*
     * populate distance_table with infinities and check neighbor
     * link costs to see if there is a link that cannot be reached
    */
    for(r = 0; r < MAX_NODES; ++r){
        nodesCantSend0[r] = NO;
        if(neighbor0->NodeCosts[r] == INFINITY)
            nodesCantSend0[r] = YES;


        for(c = 0; c < MAX_NODES; ++c){
            dt0.costs[r][c] = INFINITY;
        }
    }

    /*
     * Update the distance_table to get link costs from this node to
     * node i via node i (diagonal in the distance table)
    */
    for(i = 0; i < MAX_NODES; ++i){
        dt0.costs[i][i] = neighbor0->NodeCosts[i];
    }


    //define the packet to send the node cost and add source id.
    struct RoutePacket pkt;
    pkt.sourceid = 0;

    /*
     * populate packets minimum cost array with infinities and then
     * update it to hold the minimum distance to node r between the current
     * infinity and the shortest cost in column r of the distance table
     */
    for (r = 0; r < MAX_NODES; ++r) {
        pkt.mincost[r] = INFINITY;
        for (c = 0; c < MAX_NODES; ++c) {
            pkt.mincost[r] = MIN(pkt.mincost[r], dt0.costs[r][c]);
        }
    }

    /*
     * send a packet to all nodes that are not the current node and are
     * direct neighbors only
     */

    //int isPrintedInit0 = NO;
    for (i = 0; i < MAX_NODES; ++i) {
        if(i != 0 && nodesCantSend0[i] != YES){
            pkt.destid = i;
            //if (isPrintedInit0 == NO){
                printf("NODE 0 to NODE %d at %.3f Initial Costs: %d %d %d %d\n",i, clocktime, pkt.mincost[0], pkt.mincost[1], pkt.mincost[2], pkt.mincost[3]);
            //    isPrintedInit0 = YES;
            // }
            toLayer2(pkt);
        }
    }
    printdt0(0, neighbor0, &dt0);
}


void rtupdate0( struct RoutePacket *rcvdpkt ) {
    printf("rtupdate0 called at %.3f \n", clocktime);


    int i,c,r;                      //Loop counters
    int isUpdate = NO;              //variable to send updated table if the minimum cost to some node has changed
    int src = rcvdpkt->sourceid;    //get node number that packet received from

    /*
     * check if the currect cost to node i is longer than by going through
     * src node to node i and if so, update the shortest path and send that
     * information to neighbor nodes
     */
    for (i = 0; i < MAX_NODES; ++i) {
        int currentCost = dt0.costs[i][src];
        int altCost = dt0.costs[src][src] + rcvdpkt->mincost[i];

        if(currentCost > altCost){
            dt0.costs[i][src] = altCost;
            isUpdate = YES;
        }
    }

    /*
     * If a new shortest path is found send a packet to all neighbors with
     * new shortest path information
     */
    if(isUpdate){
        struct RoutePacket pkt;
        pkt.sourceid = 0;

        /*
        * populate packets minimum cost array with infinities and then
        * update it to hold the minimum distance to node r between the current
        * infinity and the shortest cost in column r of the distance table
        */
        for (r = 0; r < MAX_NODES; ++r) {
            pkt.mincost[r] = INFINITY;
            for (c = 0; c < MAX_NODES; ++c) {
                pkt.mincost[r] = MIN(pkt.mincost[r], dt0.costs[r][c]);
            }
        }
        //int isPrintedUpdt0 = NO;

        /*
        * send a packet to all nodes that are not the current node and are
        * direct neighbors only
        */
        for (i = 0; i < MAX_NODES; ++i) {
            if(i != 0 && nodesCantSend0[i] != YES){
                pkt.destid = i;
                //if (isPrintedUpdt0 == NO){
                printf("NODE 0 to NODE %d at %.3f Updated Costs: %d %d %d %d\n",i, clocktime, pkt.mincost[0], pkt.mincost[1], pkt.mincost[2], pkt.mincost[3]);
                //    isPrintedUpdt0 = YES;
                //}
                toLayer2(pkt);
            }
        }
        printdt0(0, neighbor0, &dt0);
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
void printdt0( int MyNodeNumber, struct NeighborCosts *neighbor, 
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
}    // End of printdt0

