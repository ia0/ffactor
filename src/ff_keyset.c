/*
 * FFactor processes factored file into developed files.
 * Copyright (C) 2015  ia0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <assert.h> /* assert */
#include <stdlib.h> /* free, malloc */
#include <string.h> /* strchr, strncmp */
#include <err.h> /* warn */

#include <ff_keyset.h>

const char *
key_parse(const char *data)
{
	const char *c;

	for (c = data; *c; c++) {
		if ('A' <= *c && *c <= 'Z') continue;
		if ('a' <= *c && *c <= 'z') continue;
		if (c == data) break;
		if ('0' <= *c && *c <= '9') continue;
		if (strchr("/_-", *c)) continue;
		break;
	}

	return c;
}

typedef struct key s_key;

struct key {
	const char *value;
	const s_key *next;
};

struct keyset {
	const s_key *first;
};

extern int
keyset_empty_ctor(s_keyset **keyset)
{
	*keyset = malloc(sizeof(**keyset));
	if (!*keyset) {
		warn("malloc()");
		return -1;
	}

	(*keyset)->first = NULL;

	return 0;
}

extern int
keyset_add(s_keyset *keyset, const char *value)
{
	s_key *key;

	assert(value);

	key = malloc(sizeof(*key));
	if (!key) {
		warn("malloc()");
		return -1;
	}

	key->value = value;
	key->next = keyset->first;
	keyset->first = key;

	return 0;
}

extern int
keyset_member(const s_keyset *keyset, const char *value, size_t len)
{
	const s_key *cur;

	for (cur = keyset->first; cur; cur = cur->next)
		if (!strncmp(cur->value, value, len) && !cur->value[len])
			return 1;

	return 0;
}

extern void
keyset_dtor(s_keyset *keyset)
{
	const s_key *cur, *next;

	if (!keyset)
		return;

	for (cur = keyset->first; cur; cur = next) {
		next = cur->next;
		free((void *)cur->value);
		free((void *)cur);
	}
	free(keyset);
}
