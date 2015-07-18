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

#ifndef FF_EVAL_H
#define FF_EVAL_H

#include <ff_keyset.h>

/** @brief Evaluate a condition. */
extern int /** @return 0 or 1 on success, -1 on error */
eval_cond(
	const s_keyset *env,	/**< __borrow__ environment */
	const char *cond	/**< __borrow__ condition */
);

#endif /* ff_eval.h */
