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

#ifndef FF_KEYSET_H
#define FF_KEYSET_H

#include <stddef.h>

/** @brief Parse a key. */
extern const char * /** @return pointer to end of key */
key_parse(
	const char *data	/**< __borrow__ */
);

/** @brief Abstract set of keys. */
struct keyset;
typedef struct keyset s_keyset;

/** @brief Create an empty set of keys. */
extern int /** @return 0 on success, -1 on error */
keyset_empty_ctor(
	s_keyset **keyset	/**< __return__ created set */
);

/** @brief Add a key to a set. */
/** Does not check for duplicates. */
extern int /** @return 0 on success, -1 on error */
keyset_add(
	s_keyset *keyset,	/**< __borrow__ keyset */
	/**< - unchanged on error */
	const char *key		/**< __take__ key to add */
);

/** @brief Test key membership. */
extern int /** @return 1 if member, 0 otherwise */
keyset_member(
	const s_keyset *keyset,	/**< __borrow__ keyset */
	const char *key,	/**< __borrow__ key to test */
	/**< - may not be null-terminated */
	size_t len		/**< key length */
);

/** @brief Delete a set of keys. */
extern void
keyset_dtor(
	s_keyset *keyset	/**< __take__ keyset */
);

#endif /* ff_keyset.h */
