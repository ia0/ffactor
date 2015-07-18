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

#ifndef FF_ENGINE_H
#define FF_ENGINE_H

#include <ff_keyset.h>

/** @brief Engine from one file to another. */
/** Expects a header to describe tokenization. */
extern int /** @return 0 on success, -1 on error */
engine_file(
	s_keyset *env,		/**< __borrow__ environment */
	const char *in,		/**< __borrow__ input path */
	const char *out		/**< __borrow__ output path */
);

/** @brief Engine from a configuration file. */
/** Like engine_file() but does not expect a header. The prefix is
 * empty and the suffix is a line feed.
 */
extern int /** @return 0 on success, -1 on error */
engine_config(
	s_keyset *env,		/**< __borrow__ environment */
	const char *path	/**< __borrow__ path */
);

/** @brief Engine from standard input to standard output. */
/** Like engine_file() but for standard input and output. */
/** On error, stdin and stdout are closed. */
extern int /** @return 0 on success, -1 on error */
engine_std(
	s_keyset *env	/**< __borrow__ environment */
);

#endif /* ff_engine.h */
