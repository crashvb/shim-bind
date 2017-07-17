#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	printf("Opening socket ...\n");	
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("ERROR opening socket");
		exit(1);
	}

	int port = 0;
	
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	//addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(port);

	printf("Binding to port: %d\n", port);	
	if (bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) != 0)
	{
		perror("ERROR on binding");
		exit(1);
	}

	socklen_t len = sizeof(addr);
	if (getsockname(sockfd, (struct sockaddr *) &addr, &len) != 0)
		perror("ERROR on getsockname");
	else
		printf("Bound to port: %d\n", ntohs(addr.sin_port));

	printf("Listening ...\n");
	listen(sockfd,5);

	printf("Sleeping ...\n");
	usleep(5 * 1000 * 1000);

	return 0;
}
