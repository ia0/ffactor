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
#include <err.h> /* warnx */

#include <ff_keyset.h>
#include <ff_parser.h>
#include <ff_eval.h>

/**
 * @brief Evaluation structure.
 *
 * Invariants:
 * - if `depth > 0` then `nvalue != -1`
 */
struct eval {
	const s_keyset *env;	/**< __borrow__ */
	const char *data;	/**< __borrow__ */
	s_parser *parser;
	int value;		/**< -1 unknown, 0 false, 1 true */
	int depth;		/**< no-eval depth */
};
typedef struct eval s_eval;

enum state {
	STATE_COND,
	STATE_OP_NOT,
	STATE_OP_AND,
	STATE_OP_OR,
	STATE_MAX,
};

static int /** @return 0 on success, -1 on error */
fstate_cond(
#ifdef NDEBUG
	__attribute__((unused))
#endif
	char state,
	void *eval
)
{
	s_eval *e = eval;

	assert(state == STATE_COND);

	if (0) {
	} else if (MATCH(e->data, "0")) {
		if (!e->depth)
			e->value = 0;
	} else if (MATCH(e->data, "1")) {
		if (!e->depth)
			e->value = 1;
	} else if (MATCH(e->data, "(not ")) {
		if (parser_push(e->parser, STATE_OP_NOT))
			return -1;
		if (parser_push(e->parser, STATE_COND))
			return -1;
		if (e->depth)
			e->depth++;
	} else if (MATCH(e->data, "(and")) {
		if (parser_push(e->parser, STATE_OP_AND))
			return -1;
		if (!e->depth)
			e->value = 1;
		else
			e->depth++;
	} else if (MATCH(e->data, "(or")) {
		if (parser_push(e->parser, STATE_OP_OR))
			return -1;
		if (!e->depth)
			e->value = 0;
		else
			e->depth++;
	} else {
		const char *end = key_parse(e->data);
		size_t len = (size_t)(end - e->data);
		assert(end >= e->data);
		if (!len) {
			warnx("Expected cond; Got '%s'", e->data);
			return -1;
		}
		if (!e->depth)
			e->value = keyset_member(e->env, e->data, len);
		e->data = end;
	}

	return 0;
}

static int /** @return 0 on success, -1 on error */
fstate_op(
	char state,
	void *eval
)
{
	s_eval *e = eval;

	if (MATCH(e->data, " ")) {
		if (state == STATE_OP_NOT) {
			warnx("Expected ')'; Got ' %s'", e->data);
			return -1;
		}
		if (parser_push(e->parser, state))
			return -1;
		if (parser_push(e->parser, STATE_COND))
			return -1;
		if (e->depth > 0)
			return 0;
		assert(!e->depth);
		assert(state == STATE_OP_AND || state == STATE_OP_OR);
		/* Last value is absorbent? */
		if (e->value != (state == STATE_OP_AND))
			e->depth = 1;
	} else if (MATCH(e->data, ")")) {
		if (e->depth > 0) {
			e->depth--;
			return 0;
		}
		assert(!e->depth);
		assert(e->value >= 0);
		if (state == STATE_OP_NOT)
			e->value = !e->value;
	} else {
		warnx("Expected ')' or ' '; Got '%s'", e->data);
		return -1;
	}

	return 0;
}

static f_state states[STATE_MAX] = {
	[STATE_COND] = fstate_cond,
	[STATE_OP_NOT] = fstate_op,
	[STATE_OP_AND] = fstate_op,
	[STATE_OP_OR] = fstate_op,
};

extern int
eval_cond(const s_keyset *env, const char *cond)
{
	int ret = -1;
	s_eval eval = {
		.env = env,
		.data = cond,
		.value = -1,
	};

	if (parser_ctor(&eval.parser, states, STATE_MAX))
		return -1;

	if (parser_push(eval.parser, STATE_COND))
		goto free_parser;

	if (parser_parse(eval.parser, &eval))
		goto free_parser;

	if (eval.data[0]) {
		warnx("Extra data '%s'", eval.data);
		goto free_parser;
	}

	assert(eval.value >= 0);
	assert(!eval.depth);
	ret = eval.value;

free_parser:
	parser_dtor(eval.parser);
	return ret;
}
