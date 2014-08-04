/* compile cc bounce_aio.c set_ticker.c -lrt -l curses -o bounce_aio
 */
#include<stdio.h>
#include<curses.h>
#include<signal.h>
#include<aio.h>

#define MESSAGE "hello"
#define BLANK	"     "

int row = 10;	/*current row*/
int col = 0;	/*current column*/
int dir = 1;	/*where we are going*/
int delay = 200;/*how long to wait*/
int done = 0;

struct aiocb kbcbuf;	/*an aio control buf*/

main()
{
	void on_alarm();	/*handler for alarm*/
	void on_input();	/*handler for keybd*/
	void setup_aio_buffer();

	initscr();
	crmode();
	noecho();
	clear();

	signal(SIGIO,on_input);
	setup_aio_buffer();
	aio_read(&kbcbuf);

	signal(SIGALRM, on_alarm);
	set_ticker(delay);

	mvaddstr(row, col, MESSAGE);

	while (!done)
		pause();
	endwin();
}

void on_input()
{
	int c;
	char *cp = (char *)kbcbuf.aio_buf;	/*cast to char * */

	/*check for error */
	if (aio_error(&kbcbuf) != 0)
		perror("reading failed");
	else
		/*get number of chars read*/
		if (aio_return(&kbcbuf) == 1) {
			c = *cp;
			if (c == 'Q' || c == EOF)
				done = 1;
			else if (c == ' ')
				dir = -dir;
		}
	/*place a new request*/
	aio_read(&kbcbuf);
}

void on_alarm()
{
	signal(SIGALRM, on_alarm);
	mvaddstr(row, col, BLANK);
	col+= dir;
	mvaddstr(row, col, MESSAGE);
	refresh();

	if (dir == -1 && col <=0)
		dir=1;
	else if (dir == 1 && col + strlen(MESSAGE) >= COLS)
		dir = -1;
}

void setup_aio_buffer()
{
	static  char input[1];		/* 1 char of input */
	/*describe what to read */
	kbcbuf.aio_fildes = 0;
	kbcbuf.aio_buf	  = input;
	kbcbuf.aio_nbytes = 1;
	kbcbuf.aio_offset = 0;

	kbcbuf.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
	kbcbuf.aio_sigevent.sigev_signo = SIGIO;
}
