/*

NateGopherExtras, EasyGopher with some extra toys.
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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void handleVotePage(int fd, char *page);
void setupVoting();

void setupSpecials() {
	setupVoting();
}

char stringStartsWith(char *comp, char *begin) {
	int len = strlen(begin);
	for (int i = 0; i < len; i++) {
		if (!comp[i] || begin[i] != comp[i])
			return 0;
	}
	return 1;
}

int handleSpecials(int fd, char *page) {
	if (stringStartsWith(page, "/vote")) {
		handleVotePage(fd, page);
		return 0;
	}
	return -1;
}
