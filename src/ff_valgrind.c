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
#include <unistd.h> /* execvp, readlink */
#include <stdlib.h> /* free, malloc */
#include <string.h> /* memcpy, strcmp, strdup */
#include <err.h> /* warn */

#include <ff_valgrind.h>

/** @brief Read the content of `/proc/self/exe`. */
static int /** @return 0 on success, -1 on error */
get_self_path(
	char **path	/**< __return__ `/proc/self/exe` link */
)
{
	size_t size = 1024;

	while (1) {
		ssize_t wrote;

		*path = malloc(size);
		if (!*path) {
			warn("malloc()");
			return -1;
		}

		wrote = readlink("/proc/self/exe", *path, size);
		if (wrote < 0) {
			warn("readlink(/proc/self/exe)");
			free(*path);
			*path = NULL;
			return -1;
		}

		if ((size_t)wrote < size) {
			(*path)[wrote] = 0;
			break;
		}

		free(*path);
		size *= 2;
	}

	return 0;
}

extern int
valgrind_bootstrap(int *pargc, char ***pargv, const char *valopt)
{
	/* Already in valgrind? */
	if (*pargc > 1 && !strcmp((*pargv)[1], valopt)) {
		*pargc -= 2;
		*pargv += 2;
		return 0;
	}

	/* Boostrap with valgrind */
	size_t targc = (size_t)*pargc + 1U;
	char **argv, *path;

	argv = malloc((targc + 4U) * sizeof(*argv));
	if (!argv) {
		warn("malloc()");
		return -1;
	}

	if (get_self_path(&path) < 0)
		goto free_argv;

	argv[0] = (char[]){ "valgrind" };
	argv[1] = (char[]){ "-q" };
	argv[2] = path;
	argv[3] = strdup(valopt);
	if (!argv[3])
		goto free_path;
	memcpy(&argv[4], &(*pargv)[0], targc * sizeof(char *));
	assert(argv[targc + 3U] == NULL);

	execvp(argv[0], argv);
	warn("execvp()");
	free(argv[3]);

free_path:
	free(path);
free_argv:
	free(argv);
	return -1;
}
