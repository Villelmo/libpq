#ifdef WIN32 
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include "libpq-fe.h"

/* for ntohl/htonl*/
#include <netinet/in.h>
#include <arpa/inet.h>

static void exit_nicely(PGconn *conn){
	PQfinish(conn);
	exit(1);
}

/* this function prints a query result that is a binary-format fetch from
 * a table defined as in the comment above. We split it out because the
 * main() function uses it twice*/
static void show_binary_results(PGresult *res){
	int i,
	    j;
	int i_fnum,
	    t_fnum,
	    b_num;
	/* Use PQfnumber to avoid assumptions about field order in result */
	i_fnum = PQfnumber(res,"i");
	t_fnum = PQfnumber(res,"t");
	b_fnum = PQfnumber(res,"b");

	for(i = 0; i < PQntuples(res); i++){
		char *iptr;
		char *tptr;
		char *bptr;
		int blen;
		int ival;

		/* Get the field values (we ignore possibility they are null!)*/
		iptr = PQgetvalue(res,i,i_fnum);
		tptr = PQgetvalue(res,i,t_fnum);
		bptr = PQgetvalue(res,i,b_fnum);

		/* the binary representation of int4 is in network byte order, which we'd better coerce to the local byte order*/
		ival = ntohl(*((uint32_t *) iptr));

		 /** The binary representation of TEXT is, well, text, and since libpq
		  * was nice enough to append a zero byte to it, it'll work just fine
		  * as a C string.
		  * The binary representation of BYTEA is a bunch of bytes, which could
		  * include embedded nulls so we have to pay attention to field length.
         */
		blen = PQgetlength(res,i,b_fnum);

		printf("tuple %d: got\n",i);
		printf("i = (%d bytes) %d\n", PQgetlength(res,i,i_fnum),ival);
		printf("t = (%d bytes) '%s'\n", PQgetlength(res,i,t_fnum),tptr);
		printf("b = (%d bytes)", blen);
		for(j = 0; j < blen; j++)
			printf("\\%03o",bptr[j]);
		printf("\n\n");
	}
}

int main(int argc, char **argv){
	const char *conninfo;
	PGconn *conn;
	PGresult *res;
	const char *paramValues[1];
	int paramLengths[1];
	int paramFormats[1];
	uint32_t binaryIntVal;
}
