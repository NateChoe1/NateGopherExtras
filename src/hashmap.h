/*

This file is part of EasyGopher, a backend for gopher.
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

#define ALLOCATED_HASHMAP_LENGTH 16384

struct HashMap {
	struct HashItem *hashItems[ALLOCATED_HASHMAP_LENGTH];
	int itemCount;
};

struct HashItem {
	void *id;
	void *value;
	int idLength;
	int valueLength;
	struct HashItem *next;
	//If there's a duplicate hash id, the id field will differentiate and to store all the HashItems, we use a linked list.
};

char compareIds(void *id1, void *id2, int length1, int length2) {
	if (length1 != length2) {
		return 0;
	}
	for (int i = 0; i < length1; i++) {
		if (*((char *) (id1 + i)) != *((char *) (id2 + i))) {
			return 0;
		}
	}

	return 1;
}

struct HashMap initializeHashMap() {
	struct HashMap newHashMap;
	newHashMap.itemCount = 0;
	for (int i = 0; i < ALLOCATED_HASHMAP_LENGTH; i++)
		newHashMap.hashItems[i] = NULL;
	return newHashMap;
}

int hash(void *id, int idLength) {
	unsigned short hashValue = 0;
	for (int i = 0; i < idLength; i++) {
		hashValue = (hashValue << 1) | (hashValue >> (sizeof(unsigned short) - 1));
		//Left circular shift
		hashValue ^= *((unsigned char *) (id + i));
	}
	return hashValue;
	//You can change this hash algorithm, just make sure the output is never larger than ALLOCATED_HASHMAP_LENGTH
	//In this case, unsigned shorts are always less that 16384 so we're fine.
}

struct HashMap addToHashMap(struct HashMap hashMap, void *id, void *value, int idLength, int valueLength) {
	int hashValue = hash(id, idLength);
	struct HashItem *hashItem = malloc(sizeof(struct HashItem));
	hashItem->id = id;
	hashItem->value = value;
	hashItem->idLength = idLength;
	hashItem->valueLength = valueLength;
	hashItem->next = hashMap.hashItems[hashValue];
	hashMap.hashItems[hashValue] = hashItem;
	//This does not account for the same element being added to a hashset twice.
	return hashMap;
}

struct HashMap removeFromHashMap(struct HashMap hashMap, void *id, int idLength) {
	if (id == NULL)
		return hashMap;
	int hashValue = hash(id, idLength);
	struct HashItem *iter = hashMap.hashItems[hashValue];
	if (compareIds(id, iter->id, idLength, iter->idLength)) {
		hashMap.hashItems[hashValue] = iter->next;
		free(iter);
		return hashMap;
	}
	while (iter->next != NULL) {
		if (compareIds(id, iter->next->id, idLength, iter->next->idLength)) {
			free(iter->next);
			iter->next = iter->next->next;
			return hashMap;
		}
	}

	return hashMap;
}

void *getValue(struct HashMap hashMap, void *id, int idLength) {
	if (id == NULL) {
		return NULL;
	}
	int hashValue = hash(id, idLength);
	struct HashItem *iter = hashMap.hashItems[hashValue];
	while (iter != NULL) {
		if (compareIds(id, iter->id, idLength, iter->idLength)) {
			return iter->value;
		}
	}
	return NULL;
}
