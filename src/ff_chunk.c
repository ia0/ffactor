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
#include <stdlib.h> /* free, malloc, realloc */
#include <string.h> /* memcpy, strlen */
#include <err.h> /* warn, warnx */

#include <ff_chunk.h>

extern int
chunk_empty_ctor(s_chunk **chunk, size_t size)
{
	assert(0 < size && size <= MAX_SIZE);

	*chunk = malloc(sizeof(**chunk));
	if (!*chunk) {
		warn("malloc()");
		return -1;
	}

	(*chunk)->data = malloc(size);
	if (!(*chunk)->data) {
		warn("malloc()");
		free(*chunk);
		*chunk = NULL;
		return -1;
	}

	(*chunk)->size = size;
	(*chunk)->len = 0;

	return 0;
}

extern int
chunk_resize(s_chunk *chunk, size_t nsize)
{
	char *ndata;

	assert(0 < nsize && nsize <= MAX_SIZE);
	assert(chunk->len <= nsize);

	ndata = realloc(chunk->data, nsize);
	if (!ndata) {
		warn("realloc()");
		return -1;
	}

	chunk->data = ndata;
	chunk->size = nsize;

	return 0;
}

extern int
chunk_push(s_chunk *chunk, const char *data, size_t len)
{
	/* Available space is too small? */
	if (chunk->size - chunk->len < len) {
		size_t nsize = chunk->size;

		while (nsize - chunk->len < len) {
			nsize *= 2;
			if (nsize > MAX_SIZE) {
				warnx("Maximum chunk size reached");
				return -1;
			}
		}

		if (chunk_resize(chunk, nsize))
			return -1;
	}

	memcpy(&chunk->data[chunk->len], data, len);
	chunk->len += len;

	return 0;
}

extern int
chunk_string_ctor(s_chunk **chunk, const char *data)
{
	size_t len = strlen(data);

	if (chunk_empty_ctor(chunk, len ? len : 1))
		return -1;

	if (chunk_push(*chunk, data, len)) {
		chunk_dtor(*chunk);
		*chunk = NULL;
		return -1;
	}

	return 0;
}

extern int
chunk_copy_ctor(s_chunk **chunk, const s_chunk *data)
{
	size_t len = data->len;

	if (chunk_empty_ctor(chunk, len ? len : 1))
		return -1;

	if (chunk_push(*chunk, data->data, data->len)) {
		chunk_dtor(*chunk);
		*chunk = NULL;
		return -1;
	}

	return 0;
}

extern void
chunk_dtor(s_chunk *chunk)
{
	if (!chunk)
		return;

	free(chunk->data);
	free(chunk);
}
