/*

EasyGopher, a backend for gopher.
Copyright (C) 2021  Nathaniel Choe

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

//This is a modified version of EasyGopher.

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "hashmap.h"
#include "setup.h"

#define RECV_BUFFER_SIZE 100

void setupSpecials();
int handleSpecials(int fd, char *page);

int openSocket(unsigned short port, int queueSize, struct sockaddr_in *addr) {
	int fd = socket(PF_INET, SOCK_STREAM, 0);

	if (fd <= 0)
		return -1;

	addr->sin_family      = PF_INET;
	addr->sin_addr.s_addr = INADDR_ANY;
	addr->sin_port        = htons(port);

	int addrlen = sizeof(*addr);

	if (bind(fd, (struct sockaddr *) addr, addrlen) < 0)
		return -2;
	if (listen(fd, queueSize) < 0)
		return -3;
	return fd;
}

int main(int argc, char **argv) {
	fprintf(stderr, "EasyGopher  Copyright (C) 2021  Nathaniel Choe\n");
	fprintf(stderr, "This program comes with ABSOLUTELY NO WARRANTY; for details, see the GPL.\n");
	fprintf(stderr, "This is free software, and you are welcome to redistribute it\n");
	fprintf(stderr, "under certain conditions; see the GPL for more details.\n");

	char *pageName = "pages.txt";
	int port = 70;
	int bufferSize = 5;
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--port") == 0 || strcmp(argv[i], "-p") == 0) {
			i++;
			port = atoi(argv[i]);
			continue;
		}
		if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--bufferSize") == 0) {
			i++;
			bufferSize = atoi(argv[i]);
			continue;
		}
		if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
			printf("Usage: ./EasyGopher [options] [page file]\n");
			printf("The page file refers to the file containing the information about what the pages show.\n");
			printf("The format of the page file goes like this:\n\n");

			printf("[Page name]\n");
			printf("[Page content line 1]\n");
			printf("[Page content line 2]\n");
			printf(".\n");
			printf("[Page 2 name]\n");
			printf("[Page 2 content line 1]\n");
			printf("[Page 2 content line 2]\n");
			printf(".\n\n");

			printf("Options include:\n");
			printf("--help        -h      Display this help message\n");
			printf("--port        -p      Select the gopher port to use\n");
			printf("--bufferSize  -b      Change the buffer size of the socket\n");
			return 0;
		}
		pageName = argv[i];
	}

	getPagesFromFile(pageName);
	setupSpecials();

	struct sockaddr_in *addr = malloc(sizeof(struct sockaddr_in));
	int fd = openSocket(port, bufferSize, addr);
	if (fd < 0) {
		fprintf(stderr, "An error occured while initializing the original file descriptor. Error code: %d\n", errno);
		return 1;
	}
	for (;;) {
		int addr_size = sizeof(*addr);
		int newfd = accept(fd, (struct sockaddr *) addr, (socklen_t *) &addr_size);
		if (newfd < 0) {
			fprintf(stderr, "An error has occured! Error code: %d\n", errno);
			return 1;
		}
		char *recvBuffer = malloc(RECV_BUFFER_SIZE);
		int receivedData = recv(newfd, recvBuffer, RECV_BUFFER_SIZE, 0);
		if (receivedData >= 2) {
			recvBuffer[receivedData - 2] = '\0';
			//-2 for \r\n

			if (handleSpecials(newfd, recvBuffer) == -1) {
				struct Line *iter = getValue(pages, recvBuffer, strlen(recvBuffer));
				//the next is because the first line is the directory.
				if (iter != NULL) {
					iter = iter->next;
					while (iter != NULL) {
						send(newfd, iter->lineContent, iter->length, 0);
						send(newfd, "\r\n", 2, 0);
						iter = iter->next;
					}
				}
			}
		}

		shutdown(newfd, 2);
	}
	return 0;
}
