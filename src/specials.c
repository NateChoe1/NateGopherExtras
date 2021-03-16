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

#define voteMessageTemplate "    Tabs: %d\r\n    Spaces: %d\r\n\r\n"

int tabsVotes;//This is the correct option.
int spacesVotes;

void setupSpecials() {
	tabsVotes = 0;
	spacesVotes = 0;
}

int handleSpecials(int fd, char *page) {
	if (strcmp(page, "/vote") == 0) {
		int voteMessageLength = snprintf(NULL, 0, voteMessageTemplate, tabsVotes, spacesVotes);
		char *voteMessage = malloc(voteMessageLength + 1);
		sprintf(voteMessage, voteMessageTemplate, tabsVotes, spacesVotes);
		char *header = "    This section is a poll on tabs vs spaces.\r\n    Current polling results:\r\n";
		char *footer = "    To vote, click on one of these links:\r\n1Vote tabs\t/tabs vote\tnates-debian-vm\t70\r\n1Vote spaces\t/spaces vote\tnates-debian-vm\t70\r\n.\r\n";
		send(fd, header, strlen(header), 0);
		send(fd, voteMessage, voteMessageLength, 0);
		send(fd, footer, strlen(footer), 0);

		//This is awful practice, but I don't really care.
		return 0;
	}
	char *votedMessage = "    Thank you for voting in this very meaningful poll.\r\n1Go back to the main page\t\tnates-debian-vm\t70\r\n.\r\n";
	if (strcmp(page, "/tabs vote") == 0) {
		tabsVotes++;
		send(fd, votedMessage, strlen(votedMessage), 0);
		return 0;
	}
	if (strcmp(page, "/spaces vote") == 0) {
		spacesVotes++;
		send(fd, votedMessage, strlen(votedMessage), 0);
		return 0;
	}
	//If I could use a switch statement I would, but switch statements don't work here.
	return -1;
}
