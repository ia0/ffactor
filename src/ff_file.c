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
#include <stdio.h> /* fread, fwrite, feof, ferror, fclose */
#include <stdlib.h> /* malloc, free */
#include <err.h> /* warn, warnx */

#include <ff_file.h>

/** @brief File information. */
struct file {
	FILE *s;	/**< file stream */
	const char *p;	/**< file path */
	unsigned int l;	/**< current line */
	unsigned int c;	/**< current column */
};

extern int
file_ctor(s_file **file, FILE *stream, const char *path)
{
	assert(stream && path);

	*file = malloc(sizeof(**file));
	if (!*file) {
		warn("malloc()");
		if (fclose(stream))
			warn("fclose()");
		free((void *)path);
		return -1;
	}

	(*file)->s = stream;
	(*file)->p = path;
	(*file)->l = 1;
	(*file)->c = 0;

	return 0;
}

/** @brief Update position information. */
static void
update_pos(
	s_file *file,	/**< __borrow__ file to update */
	char c		/**< read or written character */
)
{
	if (c == '\n') {
		file->l++;
		file->c = 0;
	} else {
		file->c++;
	}
}

extern int
file_read(s_file *file, char *data)
{
	if (fread(data, 1, 1, file->s) != 1) {
		if (ferror(file->s)) {
			warnx("Read error");
			return -1;
		}
		if (feof(file->s))
			return 1;
	}
	assert(!feof(file->s));
	assert(!ferror(file->s));

	update_pos(file, *data);

	return 0;
}

extern int
file_write(s_file *file, const char *data, size_t len)
{
	if (fwrite(data, 1, len, file->s) != len) {
		assert(ferror(file->s));
		warnx("Write error");
		return -1;
	}
	assert(!feof(file->s));
	assert(!ferror(file->s));

	for (size_t i = 0; i < len; i++)
		update_pos(file, data[i]);

	return 0;
}

extern void
file_warn(s_file *file)
{
	warnx("%s:%u:%u", file->p, file->l, file->c);
}

extern void
file_dtor(s_file *file)
{
	if (!file)
		return;

	if (fclose(file->s))
		warn("fclose()");
	free((void *)file->p);
	free(file);
}
