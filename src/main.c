/*

SimpleGopher, a backend for gopher.
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

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "setup.h"

#define RECV_BUFFER_SIZE 100

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

int main() {
	fprintf(stderr, "EasyGopher  Copyright (C) 2021  Nathaniel Choe");
	fprintf(stderr, "This program comes with ABSOLUTELY NO WARRANTY; for details type `show w'.");
	fprintf(stderr, "This is free software, and you are welcome to redistribute it");
	fprintf(stderr, "under certain conditions; type `show c' for details.");

	struct sockaddr_in *addr = malloc(sizeof(struct sockaddr_in));
	int fd = openSocket(70, 5, addr);
	if (fd < 0) {
		fprintf(stderr, "An error occured while initializing the original file descriptor. Error code: %d\n", fd);
		return 1;
	}
	getPagesFromFile("pages.txt");
	for (;;) {
		int addr_size = sizeof(*addr);
		int newfd = accept(fd, (struct sockaddr *) addr, (socklen_t *) &addr_size);
		if (newfd < 0) {
			fprintf(stderr, "An error has occured! Error code: %d\n", errno);
			return 1;
		}
		char *recvBuffer = malloc(RECV_BUFFER_SIZE);
		int receivedData = recv(newfd, recvBuffer, RECV_BUFFER_SIZE, 0);
		recvBuffer[receivedData - 2] = '\0';
		//-2 for \r\n
		for (int i = 0; i < pageCount; i++) {
			if (strcmp(pageBegins[i]->lineContent, recvBuffer) == 0) {
				struct Line *iter = pageBegins[i]->next;
				//the next is because the first line is the directory.
				char shouldContinue = 1;
				while (iter != NULL) {
					send(newfd, iter->lineContent, iter->length, 0);
					send(newfd, "\r\n", 2, 0);
					iter = iter->next;
				}
				break;	
			}
		}
		shutdown(newfd, 2);
	}
	return 0;
}
