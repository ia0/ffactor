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

#include <assert.h> /* assert */
#include <limits.h> /* CHAR_MAX */
#include <stdlib.h> /* free, malloc */
#include <string.h> /* strncmp */
#include <err.h> /* warn */

#include <ff_chunk.h>
#include <ff_parser.h>

extern int
match(const char **data, const char *token, size_t len)
{
	if (!strncmp(*data, token, len)) {
		*data += len;
		return 1;
	}

	return 0;
}

/**
 * Invariants:
 * - for all `0 <= i < stack->len`, `0 <= stack->data[i] < len`
 * - `states` is an array of length `len`
 * - `0 < len < CHAR_MAX`
 */
struct parser {
	s_chunk *stack;		/**< states stack */
	const f_state *states;	/**< __borrow__ state functions */
	char len;		/**< number of states */
};

extern int
parser_ctor(s_parser **parser, const f_state *states, char len)
{
	assert(0 < len && len < CHAR_MAX);

	*parser = malloc(sizeof(**parser));
	if (!*parser) {
		warn("malloc()");
		return -1;
	}

	if (chunk_empty_ctor(&(*parser)->stack, 16)) {
		free(*parser);
		*parser = NULL;
		return -1;
	}

	(*parser)->states = states;
	(*parser)->len = len;

	return 0;
}

extern int
parser_push(s_parser *parser, char state)
{
	assert(0 <= state && state < parser->len);

	return chunk_push(parser->stack, &state, 1);
}

extern int
parser_parse(s_parser *parser, void *data)
{
	while (parser->stack->len) {
		int ret;
		char state;

		parser->stack->len--;
		state = parser->stack->data[parser->stack->len];

		ret = parser->states[(size_t)state](state, data);
		if (ret)
			return ret;
	}

	return 0;
}

extern void
parser_dtor(s_parser *parser)
{
	if (!parser)
		return;

	chunk_dtor(parser->stack);
	free(parser);
}
