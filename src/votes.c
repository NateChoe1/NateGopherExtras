#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int tabsVotes; //This is correct.
int spacesVotes;

void setupVoting() {
	tabsVotes = 0;
	spacesVotes = 0;
}

void handleVotePage(int fd, char *page) {
	char *votedMessage = "    Thank you for voting in this very meaningful poll.\r\n1Go back to the main page\t\tkoreanalive.com\t70\r\n.\r\n";
	if (strcmp(page, "/vote") == 0) {
		char *voteMessageTemplate = "    Tabs votes: %d\r\n    Spaces votes: %d\r\n\r\n";
		int voteMessageLength = snprintf(NULL, 0, voteMessageTemplate, tabsVotes, spacesVotes);
		char *voteMessage = malloc(voteMessageLength + 1);
		sprintf(voteMessage, voteMessageTemplate, tabsVotes, spacesVotes);
		char *header = "    This section is a poll on tabs vs spaces.\r\n    Current polling results:\r\n";
		char *footer = "    To vote, click on one of these links:\r\n1Vote tabs\t/vote/right\tkoreanalive.com\t70\r\n1Vote spaces\t/vote/wrong\tkoreanalive.com\t70\r\n.\r\n";
		send(fd, header, strlen(header), 0);
		send(fd, voteMessage, voteMessageLength, 0);
		send(fd, footer, strlen(footer), 0);

		//This is awful practice, but I don't really care.
		return;
	}
	else if (strcmp(page, "/vote/right") == 0) {
		tabsVotes++;
		send(fd, votedMessage, strlen(votedMessage), 0);
		return;
	}
	else if (strcmp(page, "/vote/wrong") == 0) {
		spacesVotes++;
		send(fd, votedMessage, strlen(votedMessage), 0);
		return;
	}
	else {
		char *noVote = "That page doesn't exist.\r\n1Go back to the main page\t\tkoreanalive.com\t70\r\n.\r\n";
		send(fd, noVote, strlen(noVote), 0);
	}
}
