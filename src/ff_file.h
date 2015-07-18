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

#ifndef FF_FILE_H
#define FF_FILE_H

#include <stdio.h> /* FILE */

/** @brief Abstract file. */
struct file;
typedef struct file s_file;

/** @brief Create a read or write file. */
extern int /** @return 0 on success, -1 on error */
file_ctor(
	s_file **file,		/**< __return__ created file */
	FILE *stream,		/**< __take__ read or write stream */
	const char *path	/**< __take__ pretty-print path */
);

/** @brief Read a character from a file. */
extern int /** @return 0 on success, 1 on end-of-file, -1 on error */
file_read(
	s_file *file,	/**< __borrow__ file to read */
	char *data	/**< __borrow__ character read */
	/**< - initialized only on success */
);

/** @brief Write a character to a file. */
extern int /** @return 0 on success, -1 on error */
file_write(
	s_file *file,		/**< __borrow__ file to write */
	const char *data,	/**< __borrow__ data to write */
	size_t len		/**< data length */
);

/** @brief Print file and position. */
extern void
file_warn(
	s_file *file	/**< __borrow__ file */
);

/** @brief Delete and close a file. */
extern void
file_dtor(
	s_file *file	/**< __take__ file */
);

#endif /* ff_file.h */
