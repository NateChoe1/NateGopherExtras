/*

This file is part of SimpleGopher, a backend for gopher.
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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct HashMap pages;

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
		if (c == '\n') {
			c = fgetc(file);
			fseek(file, -1, SEEK_CUR);
			if (c == EOF)
				line->done = 1;
			line->lineContent[line->length] = '\0';
			return line;
		}
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
}

void getPagesFromFile(char *fileName) {
	FILE *file = fopen(fileName, "r");
	struct Line *head = getNextLine(file);
	struct Line *iter = head;
	pages = initializeHashMap();
	pages = addToHashMap(pages, head->lineContent, head, strlen(head->lineContent), sizeof(head));

	while (!iter->done) {
		iter->next = getNextLine(file);
		struct Line *next = iter->next;
		if (!iter->done && strcmp(iter->lineContent, ".") == 0) {
			pages = addToHashMap(pages, next->lineContent, next, strlen(next->lineContent), sizeof(next));
			iter->next = NULL;
		}
		iter = next;
	}
}
