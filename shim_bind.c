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
#include <errno.h>

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	const char * shim_bind_port = getenv("SHIM_BIND_PORT");
	int port = shim_bind_port ? atoi(shim_bind_port) : 7000;

	const char * shim_bind_end_port = getenv("SHIM_BIND_END_PORT");
	int end_port = shim_bind_end_port ? atoi(shim_bind_end_port) : 7020;

	const char * shim_bind_use_ipaddr_any = getenv("SHIM_BIND_USE_IPADDR_ANY");

	static int (*real_bind)(int sockfd, const struct sockaddr *addr, socklen_t addrlen) = NULL;
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

	memcpy(&theaddr, addr, sizeof(theaddr));

	if(shim_bind_use_ipaddr_any != NULL)
	{
		printf("shim_bind: overriding address");
		theaddr.sin_addr.s_addr = INADDR_ANY;
	}

	if(ntohs(theaddr.sin_port) != 0)
	{
		port = ntohs(theaddr.sin_port);
		end_port = ntohs(theaddr.sin_port);
	}

	int result;
	do
	{
		theaddr.sin_port = htons((unsigned short)port);
		printf("shim_bind: trying port %d\n", port);
		result = real_bind(sockfd, (struct sockaddr*)&theaddr, addrlen);
		port++;
	}while(result != 0 && errno == EADDRINUSE && port <= end_port);

	if(result != 0) errno = EFAULT;
	return result;
}
