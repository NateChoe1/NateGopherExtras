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

#define _GNU_SOURCE
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

struct Line **pageBegins;
int pagesAllocated;
int pageCount;

struct Line {
	char *lineContent;
	int allocatedLength;
	int length;
	unsigned char done : 1;
	struct Line *next;
};

struct Line *getNextLine(FILE *file) {
	struct Line *line = malloc(sizeof(struct Line));
	line->allocatedLength = 10;
	line->lineContent = malloc(line->allocatedLength);
	line->length = 0;
	line->done = 0;
	for (;;) {
		int c = fgetc(file);
		switch (c) {
			case EOF:
				line->done = 1;
			case '\n':
				goto endLoop;
		}//break just goes out of the switch statement, which isn't what I want.
		line->lineContent[line->length] = c;
		line->length++;
		if (line->length == line->allocatedLength - 1) {
			line->allocatedLength *= 2;
			char *newString = malloc(line->allocatedLength);
			for (int i = 0; i < line->length; i++)
				newString[i] = line->lineContent[i];
			free(line->lineContent);
			line->lineContent = newString;
		}
	}

endLoop:
	line->lineContent[line->length] = '\0';
	return line;
}

void getPagesFromFile(char *fileName) {
	FILE *file = fopen(fileName, "r");
	struct Line *head = getNextLine(file);
	struct Line *iter = head;

	pagesAllocated = 10;
	pageCount = 1;
	pageBegins = malloc(sizeof(struct Line) * pagesAllocated);
	pageBegins[0] = head;//the first line will always be a page begin, but will not be preceded by a dot.

	while (!iter->done) {
		iter->next = getNextLine(file);
		struct Line *next = iter->next;
		if (strcmp(iter->lineContent, ".") == 0) {
			pageBegins[pageCount] = next;
			iter->next = NULL;
			pageCount++;
			if (pageCount == pagesAllocated) {
				pagesAllocated *= 2;
				struct Line **newPageBegins;
				newPageBegins = malloc(sizeof(struct Line *) * pagesAllocated);
				for (int i = 0; i < pageCount; i++)
					newPageBegins[i] = pageBegins[i];
				free(pageBegins);
				pageBegins = newPageBegins;
			}
		}
		iter = next;
	}
}
