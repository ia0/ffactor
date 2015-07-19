/*
 * ffactor expands factored files according to an environment.
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
#include <ff_cyclic.h>

/**
 * @brief Cyclic data.
 *
 * Invariants:
 * - `size` is a power of 2
 * - current position is `len & (size - 1)`
 * - Buffer did not cycle yet:
 *   - `(len & size) == 0`
 *   - for all `0 <= i < len`, `data[i]` is initialized
 * - Buffer cycled:
 *   - `(len & size) != 0`
 *   - for all `0 <= i < size`, `data[i]` is initialized
 */
struct cyclic {
	char *data;	/**< content */
	size_t size;	/**< capacity (power of 2) */
	size_t len;	/**< length */
};

extern int
cyclic_ctor(s_cyclic **cyclic, size_t size)
{
	assert(size && !(size & (size - 1)));

	*cyclic = malloc(sizeof(**cyclic));
	if (!*cyclic) {
		warn("malloc()");
		return -1;
	}

	(*cyclic)->data = malloc(size);
	if (!(*cyclic)->data) {
		warn("malloc()");
		free(*cyclic);
		*cyclic = NULL;
		return -1;
	}

	(*cyclic)->size = size;
	(*cyclic)->len = 0;

	return 0;
}

extern int
cyclic_push_cb(
	s_cyclic *cyclic, const char *data, size_t len,
	f_callback callback, void *arg
)
{
	const size_t size = cyclic->size;
	size_t cur = cyclic->len & (size - 1);
	size_t cycled = cyclic->len & size;
	size_t next = size - cur; /**< available space before cycle */

	while (len > 0) {

		/* Do not write too much. */
		if (len < next)
			next = len;

		if (cycled && callback(arg, &cyclic->data[cur], next))
			return -1;

		memcpy(&cyclic->data[cur], data, next);

		assert(cur + next <= size);
		cyclic->len = cycled | (cur + next);
		cycled = cyclic->len & size;

		data += next;
		len -= next;

		cur = 0;
		next = size;
	}

	return 0;
}

extern int
cyclic_make_chunk(s_chunk **chunk, const s_cyclic *cyclic)
{
	const size_t size = cyclic->size;
	const char *data = cyclic->data;
	size_t len = cyclic->len;

	if (chunk_empty_ctor(chunk, size))
		return -1;

	/* Did the buffer cycle? */
	if (len & size) {
		len &= size - 1;

		if (chunk_push(*chunk, &data[len], size - len))
			return -1;
	}

	if (chunk_push(*chunk, data, len))
		return -1;

	return 0;
}

extern void
cyclic_dtor(s_cyclic *cyclic)
{
	if (!cyclic)
		return;

	free(cyclic->data);
	free(cyclic);
}
