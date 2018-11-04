#include <stdio.h>
#include <stdlib.h>
#include "project2.h"

/* ***************************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for unidirectional or bidirectional
   data transfer protocols from A to B and B to A.
   Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets may be delivered out of order.

   Compile as gcc -g project2.c student2.c -o p2
**********************************************************************/



/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/
/* 
 * The routines you will write are detailed below. As noted above, 
 * such procedures in real-life would be part of the operating system, 
 * and would be called by other procedures in the operating system.  
 * All these routines are in layer 4.
 */

int getChecksum(struct pkt aPkt);   //function prototype

const int WAITTIME = 50; //timeout set when a packet is sent and waiting for an ack.


int ready;          //boolean to determine whether packet can be sent to layer 3
int ackNum;             //abp sends packets with seq number of 0 or 1
int seqNum;         //holds the sequence number
struct msg lastMessage; //struct that holds the last message sent in case of retransmission
struct msg msgQueue[150];//holds messages that can't go out
int queIndex;       //holds the index of the next packet to be sent out
int queMax;         //used to check the upper bound of the queue and to check if all queued packets sent
int checkCalc[20];    //random numbers that are used to calculate checksum



/*
 * the method for getting the checksum is completely trivial
 * it takes the ack and seq number and adds 2 to it and that
 * number is multiplied by the first and the last number in the
 * multiplication array that ranges from 2-22. That array multiplies
 * every char in the payload. If characters are missing from the payload
 * the packet's value is reset to 0
 */
int getChecksum(struct pkt aPkt) {
    int num = 0;
    int i;
    int allCharsThere = 1;
    if (aPkt.payload[0])
        for (i = 0; i < MESSAGE_LENGTH; i++) {
            num += aPkt.payload[i] * checkCalc[i];
            if(!aPkt.payload[i])
                allCharsThere = 0;
        }
        if(!allCharsThere)
            num = 0;
    num += ((aPkt.seqnum + 10) * checkCalc[0]);
    num += ((aPkt.acknum + 15) * checkCalc[19]);
    return num;
}


/*
 * A_output(message), where message is a structure of type msg, containing 
 * data to be sent to the B-side. This routine will be called whenever the 
 * upper layer at the sending side (A) has a message to send. It is the job 
 * of your protocol to insure that the data in such a message is delivered 
 * in-order, and correctly, to the receiving side upper layer.
 */
void A_output(struct msg message) {
    if (!ready) {
        msgQueue[queMax] = message; //Store the message in the queue if more recent ack not received
        queMax = (queMax + 1) % 150;     //Increase the queue size
        return;                     //return because there is nothing to send.
    }
    if (ready) {
        int i;
        struct pkt newPkt;                          //create new packet
        newPkt.acknum = 0;                          //acknumber not used by sender
        newPkt.seqnum = seqNum % 2;                 //set packet sequence number to 0 or 1
        for (i = 0; i < MESSAGE_LENGTH; i++) {  //copy the message to packet and var holding sent message in
            newPkt.payload[i] = message.data[i];
            lastMessage.data[i] = message.data[i];  //saved to this variable in case of corruption
        }
        newPkt.checksum = getChecksum(newPkt);  //calculate checksum
        tolayer3(AEntity, newPkt);              //send the packet to network layer
        startTimer(0, WAITTIME);                //start the timer and wait for response
        ready = 0;                              //don't send when called again because need ack res
    }
}

/*
 * Just like A_output, but residing on the B side.  USED only when the 
 * implementation is bi-directional.
 */
void B_output(struct msg message) {

}

/* 
 * A_input(packet), where packet is a structure of type pkt. This routine 
 * will be called whenever a packet sent from the B-side (i.e., as a result 
 * of a tolayer3() being done by a B-side procedure) arrives at the A-side. 
 * packet is the (possibly corrupted) packet sent from the B-side.
 */
void A_input(struct pkt packet) {
    /*
     * This if statement checks ack and if there is something in the queue
     * If nothing is in the queue we prep a_output to send the next message.
    */
    if (packet.acknum == (seqNum % 2) && packet.checksum == getChecksum(packet) && queIndex == queMax) {
        //printf("%d == %d", packet.checksum, getChecksum(packet));
        stopTimer(AEntity);
        ready = 1;
        seqNum++;
        return;
    }
        /*
         * This if statement checks if ack and seq numbers are correct. Queue is implied
         * Checks if the index number for the queue has wrapped around as well which will happen
         * when the queue gets used > 150 times.
        */
    else if (packet.acknum == (seqNum % 2) && packet.checksum == getChecksum(packet) &&
             (queMax > 0 || (queMax == 0 && queIndex != 0))) {
        //printf("%d == %d/n", packet.checksum, getChecksum(packet));
        stopTimer(AEntity);
        ready = 1;
        A_output(msgQueue[queIndex]);
        queIndex = (queIndex + 1) % 150;
        seqNum++;
    }
        //This statement implies there the ack received is corrupt so we will let the timer expire
    else
        return;
}

/*
 * A_timerinterrupt()  This routine will be called when A's timer expires 
 * (thus generating a timer interrupt). You'll probably want to use this 
 * routine to control the retransmission of packets. See starttimer() 
 * and stoptimer() in the writeup for how the timer is started and stopped.
 */
void A_timerinterrupt() {
    ready = 1;              //allow output to resend the last message
    A_output(lastMessage);  //resend the last called with A_output
}

/* The following routine will be called once (only) before any other    */
/* entity A routines are called. You can use it to do any initialization */
void A_init() {
    ready = 1;
    seqNum = 0;
    queIndex = 0;
    queMax = 0;
    int i;
    for (i = 0; i < MESSAGE_LENGTH; i++) {
        //checkCalc[i] = (rand() % 8)+2;
        checkCalc[i] = (i+2)*2;
    }
}


/* 
 * Note that with simplex transfer from A-to-B, there is no routine  B_output() 
 */

/*
 * B_input(packet),where packet is a structure of type pkt. This routine 
 * will be called whenever a packet sent from the A-side (i.e., as a result 
 * of a tolayer3() being done by a A-side procedure) arrives at the B-side. 
 * packet is the (possibly corrupted) packet sent from the A-side.
 */
void B_input(struct pkt packet) {
    /*
     * this if statement checks to see if the packet received is not corrupt and sends
     * the data up to layer 5, creates an ACK packet that has no payload and sends it over
     * to A side
     */
    if (packet.seqnum == (ackNum % 2) && packet.checksum == getChecksum(packet)) {
        struct msg recMsg;
        int i;
        for (i = 0; i < MESSAGE_LENGTH; i++) {
            recMsg.data[i] = packet.payload[i];
        }
        tolayer5(BEntity, recMsg);

        struct pkt ackPkt;
        ackPkt.acknum = (ackNum % 2);
        ackPkt.seqnum = 0;
        ackPkt.checksum = getChecksum(ackPkt);

        tolayer3(BEntity, ackPkt);
        ackNum++;
        return;
    }
    /*
     * If the packet is corrupt, send back the opposite of the sequence number
     * so: 0->1 and 1->0
     */
    else {
        struct pkt ackPkt;
        ackPkt.seqnum = 0;
        ackPkt.acknum = ((ackNum + 1) % 2);
        ackPkt.checksum = getChecksum(ackPkt);
        tolayer3(BEntity, ackPkt);
    }
}

/*
 * B_timerinterrupt()  This routine will be called when B's timer expires 
 * (thus generating a timer interrupt). You'll probably want to use this 
 * routine to control the retransmission of packets. See starttimer() 
 * and stoptimer() in the writeup for how the timer is started and stopped.
 */
void B_timerinterrupt() {
}

/* 
 * The following routine will be called once (only) before any other   
 * entity B routines are called. You can use it to do any initialization 
 */
void B_init() {
    ackNum = 0;             //abp sends packets with seq number of 0 or 1
}
