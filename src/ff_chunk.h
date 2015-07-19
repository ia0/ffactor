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

#ifndef FF_CHUNK_H
#define FF_CHUNK_H

#include <stddef.h> /* size_t */

/**
 * @file
 * @brief Tail-extensible binary data.
 *
 * A chunk may grow from its tail and may contain arbitrary binary
 * data including null bytes.
 */

/** @brief Maximum chunk capacity. */
#define MAX_SIZE ((size_t)-1 / 2)

/**
 * @brief Chunk of data.
 *
 * Invariants:
 * - `0 < size <= MAX_SIZE`
 * - `0 <= len <= size`
 * - for all `0 <= i < len`, `data[i]` is initialized
 */
struct chunk {
	char *data;	/**< content */
	size_t size;	/**< capacity */
	size_t len;	/**< length */
};
typedef struct chunk s_chunk;

/** @brief Create an empty chunk. */
extern int /** @return 0 on success, -1 on error */
chunk_empty_ctor(
	s_chunk **chunk,	/**< __return__ created chunk */
	size_t size		/**< initial size */
	/**< - must satisfy `0 < size <= MAX_SIZE` */
);

/** @brief Create a chunk from a string. */
/** Does not copy the null-terminator. */
extern int /** @return 0 on success, -1 on error */
chunk_string_ctor(
	s_chunk **chunk,	/**< __return__ created chunk */
	const char *data	/**< __borrow__ source string */
	/**< - length must not exceed #MAX_SIZE */
);

/** @brief Create a chunk from a chunk. */
extern int /** @return 0 on success, -1 on error */
chunk_copy_ctor(
	s_chunk **chunk,	/**< __return__ created chunk */
	const s_chunk *data	/**< __borrow__ source chunk */
);

/** @brief Resize a chunk. */
extern int /** @return 0 on success, -1 on error */
chunk_resize(
	s_chunk *chunk,		/**< __borrow__ chunk to resize */
	/**< - unchanged on error */
	size_t size		/**< new size */
	/**< - must satisfy `0 < size <= MAX_SIZE` */
	/**< - must satisfy `chunk->len <= size` */
);

/** @brief Push data to a chunk. */
extern int /** @return 0 on success, -1 on error */
chunk_push(
	s_chunk *chunk,		/**< __borrow__ destination */
	/**< - unchanged on error */
	const char *data,	/**< __borrow__ source */
	size_t len		/**< source length */
);

/** @brief Delete a chunk. */
extern void
chunk_dtor(
	s_chunk *chunk	/**< __take__ chunk */
);

#endif /* ff_chunk.h */
