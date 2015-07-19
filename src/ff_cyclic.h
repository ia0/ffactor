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

#ifndef FF_CYCLIC_H
#define FF_CYCLIC_H

#include <stddef.h> /* size_t */

#include <ff_chunk.h> /* s_chunk */

/** @brief Abstract cyclic buffer. */
struct cyclic;
typedef struct cyclic s_cyclic;

typedef int /**< @return 0 on success, -1 on error */
(*f_callback)(
	void *arg,		/**< __borrow__ argument */
	const char *data,	/**< __borrow__ source */
	size_t len		/**< source length */
);

/** @brief Create an empty cyclic buffer. */
extern int /** @return 0 on success, -1 on error */
cyclic_ctor(
	s_cyclic **cyclic,	/**< __return__ created buffer */
	size_t size		/**< initial size (power of 2) */
);

/** @brief Push data to a cyclic buffer. */
extern int /** @return 0 on success, -1 on error */
cyclic_push_cb(
	s_cyclic *cyclic,	/**< __borrow__ buffer */
	const char *data,	/**< __borrow__ source */
	size_t len,		/**< source length */
	f_callback callback,	/**< callback function */
	void *arg		/**< callback argument */
);

/** @brief Make a chunk from a cyclic buffer. */
extern int /** @return 0 on success, -1 on error */
cyclic_make_chunk(
	s_chunk **chunk,	/**< __return__ resulting chunk */
	const s_cyclic *cyclic	/**< __borrow__ source buffer */
);

/** @brief Delete a cyclic buffer. */
extern void
cyclic_dtor(
	s_cyclic *cyclic	/**< __take__ buffer */
);

#endif /* ff_cyclic.h */
