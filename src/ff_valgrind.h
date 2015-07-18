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

#ifndef FF_VALGRIND_H
#define FF_VALGRIND_H

/**
 * @brief Execute a program inside valgrind.
 *
 * The `valopt` sentinel is used to check whether the program is
 * already running inside valgrind. As such it must not be a possible
 * first argument of the program.
 *
 * The program `argc` and `argv` are modified as if no bootstrap
 * occurred. The bootstrap process is thus transparent according to
 * these values.
 */
extern int /** @return 0 on success, -1 on error */
valgrind_bootstrap(
	int *pargc,		/**< __borrow__ program `argc` */
	char ***pargv,		/**< __borrow__ program `argv` */
	const char *valopt	/**< __borrow__ sentinel argument */
);

#endif /* ff_valgrind.h */
