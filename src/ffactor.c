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

#include <string.h> /* strcmp */
#include <stdio.h> /* printf */
#include <err.h> /* warn, warnx */

#ifdef FF_VALGRIND
# include <ff_valgrind.h>
#endif
#include <ff_keyset.h>
#include <ff_engine.h>

int
main(int argc, char *argv[])
{
	s_keyset *env;

#ifdef FF_VALGRIND
	if (valgrind_bootstrap(&argc, &argv, "--no-valgrind")) {
		warn("Could not bootstrap under valgrind");
		return 1;
	}
#endif

	if (argc == 2 && !strcmp("--help", argv[1])) {
		printf("Usage: %s [<env>..] [-- <src> <dst>]\n", argv[0]);
		return 0;
	}

	if (keyset_ctor(&env))
		return 1;

	for (int i = 1; i < argc; i++) {
		if (!strcmp("--", argv[i])) {
			if (i + 3 != argc) {
				warnx("Bad command line");
				return 1;
			}
			if (engine_file(env, argv[i + 1], argv[i + 2]))
				return 1;
			return 0;
		}
		if (engine_env(env, argv[i]))
			return 1;
	}

	if (engine_std(env))
		return 1;

	return 0;
}
