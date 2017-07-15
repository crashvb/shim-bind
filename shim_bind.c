/*
 * capture calls to a routine and replace with your code
 * http://unix.stackexchange.com/a/305336/119298
 * gcc -Wall -O2 -fpic -shared -ldl -o shim_bind.so shim_bind.c
 * LD_PRELOAD=/path/to/shim_bind.so ./test
 */

#define _GNU_SOURCE /* needed to get RTLD_NEXT defined in dlfcn.h */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	static int (*real_bind)(int sockfd, const struct sockaddr *addr, socklen_t addrlen) = NULL;
	int port = 7777;
	struct sockaddr_in theaddr;

	if (!real_bind)
	{
		real_bind = dlsym(RTLD_NEXT, "bind");
		char *error = dlerror();
		if (error != NULL)
		{
			fprintf(stderr, "%s\n", error);
			exit(1);
		}
	}

	fprintf(stderr, "binding: port %d\n", port);
	memcpy(&theaddr, addr, sizeof(theaddr));
	theaddr.sin_port = htons((unsigned short)port);
	return real_bind(sockfd, (struct sockaddr*)&theaddr, addrlen);
}
