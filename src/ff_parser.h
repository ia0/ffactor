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

#ifndef FF_PARSER_H
#define FF_PARSER_H

/** @brief Match a token and update data accordingly. */
extern int /** @return 0 or 1 whether it matched */
match(
	const char **data,	/**< __borrow__ where to match */
	const char *token,	/**< __borrow__ what to match */
	size_t len		/**< token length */
);
/* MATCH(lvalue, string-literal) */
#define MATCH(data, token) (match(&(data), token, sizeof(token) - 1))

/** @brief Abstract LL(1) parser. */
struct parser;
typedef struct parser s_parser;

typedef int (*f_state)(char, void *);

/** @brief Create a parser. */
extern int /** @return 0 on success, -1 on error */
parser_ctor(
	s_parser **parser,	/**< __return__ created parser */
	const f_state *states,	/**< __borrow__ states */
	/**< - borrow has `parser` scope (until `parser_dtor`) */
	char length		/**< states length */
	/**< - must satisfy `0 < length < CHAR_MAX` */
);

/** @brief Push a state. */
extern int /** @return 0 on success, -1 on error */
parser_push(
	s_parser *parser,	/**< __borrow__ parser */
	/**< - unchanged on error */
	char state		/**< state to push */
	/**< - must satisfy `0 <= state < length` */
);

/** @brief Parse data. */
extern int /** @return 0 on success, state error otherwise */
parser_parse(
	s_parser *parser,	/**< __borrow__ parser */
	void *data		/**< __borrow__ initial data */
);

/** @brief Delete a parser. */
extern void
parser_dtor(
	s_parser *parser	/**< __take__ parser */
);

#endif /* ff_parser.h */
