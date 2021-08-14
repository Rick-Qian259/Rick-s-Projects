#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "emulator.h"
#include "sr.h"

/* ******************************************************************
   Go Back N protocol.  Adapted from
   ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   Network properties:
   - one way network delay averages five time units (longer if there
   are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
   or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
   (although some can be lost).

   Modifications (6/6/2008 - CLP): 
   - removed bidirectional GBN code and other code not used by prac. 
   - fixed C style to adhere to current programming style
   (24/3/2013 - CLP)
   - added GBN implementation
**********************************************************************/

#define RTT 15.0      /* round trip time.  MUST BE SET TO 15.0 when submitting assignment */
#define WINDOWSIZE 6  /* Maximum number of buffered unacked packet */
#define SEQSPACE 12   /* min sequence space for SR must be at least double size of windowsize */
#define NOTINUSE (-1) /* used to fill header fields that are not being used */

/* generic procedure to compute the checksum of a packet.  Used by both sender and receiver  
   the simulator will overwrite part of your packet with 'z's.  It will not overwrite your 
   original checksum.  This procedure must generate a different checksum to the original if
   the packet is corrupted.
*/
int ComputeChecksum(struct pkt packet)
{
  int checksum = 0;
  int i;

  checksum = packet.seqnum;
  checksum += packet.acknum;
  for (i = 0; i < 20; i++)
    checksum += (int)(packet.payload[i]);

  return checksum;
}

bool IsCorrupted(struct pkt packet)
{
  if (packet.checksum == ComputeChecksum(packet))
    return (false);
  else
    return (true);
}

/********* Sender (A) variables and functions ************/

static struct pkt buffer[WINDOWSIZE]; /* array for storing packets waiting for ACK */
static int windowfirst, windowlast;   /* array indexes of the first/last packet awaiting ACK */
static int windowcount;               /* the number of packets currently awaiting an ACK */
static int A_nextseqnum;              /* the next sequence number to be used by the sender */
static int temp_A[WINDOWSIZE];        /* this array is used to be a buffer in A */
static int A_j;                       /* this is the index of temp_a* /

/* this array is used to store the package sequence number which will be used to judge whether the window
    on the sender B should be moved. This array will be initilised by -1 in each number.  */
static int temp_array[WINDOWSIZE];    /* this is the buffer in B */

/* 
    this function is used to judge whether the window on sender B should be slided.  
*/
bool IfMove(int temp_array[], int expect_seq_num)
{
  int i;

  for (i = 0; i < WINDOWSIZE; i++)
  {
    if (expect_seq_num == temp_array[i])
    {
      temp_array[i] = -1;
      return true;
    }
  }
  return false;
}

bool IsStored(int temp_array[], int seq)
{
  int i;
  for (i = 0; i < WINDOWSIZE; i++)
  {
    if (temp_array[i] == seq)
      return false;
  }
  return true;
}

/* called from layer 5 (application layer), passed the message to be sent to other side */
void A_output(struct msg message)
{
  struct pkt sendpkt;
  int i;

  /* if not blocked waiting on ACK */
  if (windowcount < WINDOWSIZE)
  {
    if (TRACE > 1)
      printf("----A: New message arrives, send window is not full, send new messge to layer3!\n");

    /* create packet */
    sendpkt.seqnum = A_nextseqnum;
    sendpkt.acknum = NOTINUSE;
    for (i = 0; i < 20; i++)
      sendpkt.payload[i] = message.data[i];
    sendpkt.checksum = ComputeChecksum(sendpkt);

    /* put packet in window buffer */
    /* windowlast will always be 0 for alternating bit; but not for GoBackN */
    windowlast = (windowlast + 1) % WINDOWSIZE;
    buffer[windowlast] = sendpkt;
    windowcount++;

    /* send out packet */
    if (TRACE > 0)
      printf("Sending packet %d to layer 3\n", sendpkt.seqnum);
    tolayer3(A, sendpkt);

    /* start timer if first packet in window */
    if (windowcount == 1)
      starttimer(A, RTT);

    /* get next sequence number, wrap back to 0 */
    A_nextseqnum = (A_nextseqnum + 1) % SEQSPACE;
  }
  /* if blocked,  window is full */
  else
  {
    if (TRACE > 0)
      printf("----A: New message arrives, send window is full\n");
    window_full++;
  }
}

/* 
   called from layer 3, when a packet arrives for layer 4 
   In this practical this will always be an ACK as B never sends data.
*/
void A_input(struct pkt packet)
{
  /* when window moves, restart the timer once. */
  bool ifRestartTimer = false;
  /* if received ACK is not corrupted */
  if (!IsCorrupted(packet))
  {
    if (TRACE > 0)
      printf("----A: uncorrupted ACK %d is received\n", packet.acknum);
    total_ACKs_received++;

    /* check if new ACK or duplicate */
    if (windowcount != 0)
    {
      int seqfirst = buffer[windowfirst].seqnum;
      int seqlast = buffer[windowlast].seqnum;

      /* check case when seqnum has and hasn't wrapped */
      if (((seqfirst <= seqlast) && (packet.acknum >= seqfirst && packet.acknum <= seqlast)) ||
          ((seqfirst > seqlast) && (packet.acknum >= seqfirst || packet.acknum <= seqlast)))
      {
        /* packet is a new ACK */
        if (TRACE > 0)
          printf("----A: ACK %d is not a duplicate\n", packet.acknum);
        new_ACKs++;

        /* buffer the received the correct packet when it is not the first packet in the window */
        temp_A[A_j] = packet.acknum;
        A_j = (A_j + 1) % WINDOWSIZE;

        /* judge whether move the window, when the seqfirst is buffered in temp_a 
           which means the oldest packet has recived the correct ACK, then move the windos */
        while (IfMove(temp_A, seqfirst))
        {
          seqfirst = (seqfirst + 1) % SEQSPACE;
          windowcount--;
          windowfirst = (windowfirst + 1) % WINDOWSIZE;
          ifRestartTimer = true;
        }
        /* restart the timer */
        if (ifRestartTimer)
        {
          stoptimer(A);
          if (windowcount > 0)
            starttimer(A, RTT);
        }
      }
      else if (TRACE > 0)
        printf("----A: duplicate ACK received, do nothing!\n");
    }
    else if (TRACE > 0)
      printf("----A: duplicate ACK received, do nothing!\n");
  }
  else if (TRACE > 0)
    printf("----A: corrupted ACK is received, do nothing!\n");
}

/* called when A's timer goes off */
void A_timerinterrupt(void)
{
  if (TRACE > 0)
    printf("----A: time out,resend packets!\n");

  if (TRACE > 0)
    printf("---A: resending packet %d\n", buffer[windowfirst].seqnum);
  tolayer3(A, buffer[windowfirst]);
  packets_resent++;
  starttimer(A, RTT);
}

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init(void)
{
  int i;
  /* initialise A's window, buffer and sequence number */
  A_nextseqnum = 0; /* A starts with seq num 0, do not change this */
  windowfirst = 0;
  windowlast = -1; /* windowlast is where the last packet sent is stored.  
		     new packets are placed in winlast + 1 
		     so initially this is set to -1
		   */
  windowcount = 0;
  A_j = 0;
  /* initialise the each node as -1 in buffer array, because the default number of array node is 0, 
      but the packet sequence number could be 0, but not -1 */
  for (i = 0; i < WINDOWSIZE; i++)
  {
    temp_A[i] = -1;
  }
}

/********* Receiver (B)  variables and procedures ************/

static int expectedseqnum; /* the sequence number expected next by the receiver */
static int B_nextseqnum;   /* the sequence number for the next packets sent by B */

static int B_expect_seq_num;

static int B_windowfirst, B_windowlast; /* this is the B's first/las window */
static int B_j;                         /* use for B_ */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet)
{
  struct pkt sendpkt;
  /* int k ; */
  int slide_account = 0;
  bool store_seq = false;
  int i;

  /* if not corrupted 
      ***TODO the receviced package's sequence number is on the range of window. */
  if (!IsCorrupted(packet))
  {
    /*if  ( (!IsCorrupted(packet))  && (packet.seqnum == expectedseqnum) ) {*/
    if (TRACE > 0)
      printf("----B: packet %d is correctly received, send ACK!\n", packet.seqnum);
    packets_received++;

    if (((B_windowfirst <= B_windowlast) && (B_windowfirst <= packet.seqnum && packet.seqnum <= B_windowlast)) ||
        ((B_windowfirst > B_windowlast) && (B_windowfirst <= packet.seqnum || B_windowlast >= packet.seqnum)))
    {
      if (IsStored(temp_array, packet.seqnum))
        store_seq = true;
    }

    /* if the package has never been stored, store it in array.
       And add the windowlast, because a new package has been received.*/
    if (store_seq)
    {
      temp_array[B_j] = packet.seqnum;
      B_j = (B_j + 1) % WINDOWSIZE;
    }

    while (IfMove(temp_array, B_expect_seq_num))
    {

      /* deliver to receiving application */
      tolayer5(B, packet.payload);
      B_expect_seq_num = (B_expect_seq_num + 1) % SEQSPACE;
      slide_account++;
    }

    B_windowfirst = (B_windowfirst + slide_account) % SEQSPACE;
    B_windowlast = (B_windowfirst + SEQSPACE - 1) % SEQSPACE;

    /* send an ACK for the received packet */
    sendpkt.acknum = packet.seqnum;

    /* update state variables */
    expectedseqnum = (expectedseqnum + 1) % SEQSPACE;

    /* create packet */
    sendpkt.seqnum = B_nextseqnum;
    B_nextseqnum = (B_nextseqnum + 1) % 2;

    /* we don't have any data to send.  fill payload with 0's */
    for (i = 0; i < 20; i++)
      sendpkt.payload[i] = '0';

    /* computer checksum */
    sendpkt.checksum = ComputeChecksum(sendpkt);

    /* send out packet */
    tolayer3(B, sendpkt);
  }
  else
  {
    /* packet is corrupted or out of order resend last ACK */
    /* In SR it doesn't need this one */

    /* if (TRACE > 0)
      printf("----B: packet corrupted or not expected sequence number, resend ACK!\n");
    if (expectedseqnum == 0)
      sendpkt.acknum = SEQSPACE - 1;
    else
      sendpkt.acknum = expectedseqnum - 1; */
  }
}

/* the following routine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init(void)
{
  int i;
  expectedseqnum = 0;
  B_nextseqnum = 1;
  /* the same as A_init */
  for (i = 0; i < WINDOWSIZE; i++)
  {
    temp_array[i] = -1;
  }
  B_j = 0;
  B_expect_seq_num = 0;
  B_windowfirst = 0;
  B_windowlast = 0;
}

/******************************************************************************
 * The following functions need be completed only for bi-directional messages *
 *****************************************************************************/

/* Note that with simplex transfer from a-to-B, there is no B_output() */
void B_output(struct msg message)
{
}

/* called when B's timer goes off */
void B_timerinterrupt(void)
{
}