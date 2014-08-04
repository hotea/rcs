/* functions for the client of the license server */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

/* important variables used throughout */
static int pid = -1;		/* Our PID */
static int sd  = -1;		/* Our communications socket */
static struct sockaddr serv_addr; /* Server address */
static socklen_t serv_alen;	  /* length of address */
static char ticket_buf[128];	  /* buffer to hold our ticket */
static have_ticket = 0;		  /* set when we have a ticket */

#define MSGLEN 128		/* size of our datagrams */
#define SERVER_PORTNUM 2020	/* Our server's port number */
#define HOSTLEN 512
#define oops(p) {perror(p); exit(1); }

char *do_transaction();

/* setup: get pid, socket, and address of license server
 * IN no args
 * RET nothing, dies on error
 * notes: assumes server is on same host as client
 */
setup()
{
     char hostname[BUFSIZ];

     pid = getpid();		/* for ticks and msgs */
     sd = make_dgram_client_socket(); /* to talk to server */
     if (sd == -1)
	  oops("Cannot create socket");
     gethostname(hostname, HOSTLEN); /* server on same host */
     make_internet_address(hostname, SERVER_PORTNUM, &serv_addr);
     serv_alen = sizeof(serv_addr);
}

shut_down()
{
     close(sd);
}

/* get a ticket from the license server */
int get_ticket()
{
     char *response;
     char buf[MSGLEN];

     if (have_ticket)		/* don't be greedy */
	  return(0);

     sprintf(buf, "HELO%d", pid); /* compose request */

     if ((response = do_transaction(buf)) == NULL)
	  return(-1);

     /* parse the response and see if we got a ticket.
      * on success, the message is: TICK ticket-string
      * on failure, the massage is: FAIL failure-MSGLEN
      */
     if (strncmp(response, "TICK" ,4) == 0){
	  strcpy(ticket_buf, response + 5); /* grab ticket-id */
	  have_ticket = 1;		    /* set this flag */
	  narrate("got ticket", ticket_buf);
	  return(0);
     }

     if (strncmp(response, "FAIL", 4) == 0)
	  narrate("Could not get ticket", response);
     else
	  narrate("Unknownnn message:",response);

     return(-1);
}

/* give a ticket back to the server */
int release_ticket()
{
     char buf[MSGLEN];
     char *response;

     if (!have_ticket)		/* don't have a ticket */
	  return(0);		/* nothing to release */

     sprintf(buf, "GBYE%s", ticket_buf); /* compose message */
     if ((response = do_transaction(buf)) == NULL)
	  return (-1);

     /* examine response */
     if (strncmp(response, "THNX", 4) == 0) {
	  narrate("released ticket OK", "");
	  return 0;
     }
     if (strncmp(response, "FAIL", 4) == 0)
	  narrate("release failed", response+5);
     else
	  narrate("Unknown message:",response);
     return -1;
}

/* send a request to the server and get a response back
 * IN mag_p    message to send
 * results: pointer to message string, or NULL for error
 *                 NOTE:pointer returned is to static storage
 *                 overwritten by each successive
 * note: for extra security, compare retaddr to serv_addr
 */
char *do_transaction(char *msg)
{
     static char buf[MSGLEN];
     struct sockaddr retaddr;
     socklen_t addrlen=sizeof(retaddr);
     int ret;
     
     ret = sendto(sd, msg, strlen(msg), 0, &serv_addr, serv_alen);
     if (ret == -1) {
	     oops("sendto");
	     return(NULL);
     }
     ret = recvfrom(sd, buf, MSGLEN, 0, &retaddr, &addrlen);
     if (ret == -1) {
	  oops("recvfrom");
	  return(NULL);
     }

     /* now return the message itself */
     return(buf);
}

/* narrate: print messages to stderr for debugging and demo purposes
 * IN msg1, msg2: strings to print along with pid and title
 * RET nothing, dies on error
 */
narrate(char *msg1)
{
     char buf[MSGLEN];
     sprintf(buf, "CLIENT[%d]: %s", pid, msg1);
     perror(buf);
}
