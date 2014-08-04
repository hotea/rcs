/* license server client
 * link with lclnt_funcs.o dgram.o
 */
#include <stdio.h>

void do_regular_work(void);
int main(int ac, char *av[]) 
{
     setup();
     if (get_ticket() != 0)
	  exit(0);
     
     do_regular_work();
     release_ticket();
     shut_down();
}

do_regular_work()
{
     printf("supersleep version running - License software\n");
     sleep(10);			/* out parented sleep algorithm */
}
