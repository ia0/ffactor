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
#include <stdlib.h> /* free, malloc */
#include <string.h> /* memcmp, memset, strdup, strncmp */
#include <err.h> /* warn, warnx */

#include <ff_chunk.h>
#include <ff_keyset.h>
#include <ff_file.h>
#include <ff_parser.h>
#include <ff_eval.h>
#include <ff_engine.h>

/**
 * @brief Engine data and information.
 *
 * Invariants:
 * - `suf->len > 0`
 * - if `depth > 0` then `ncopy != 0`
 */
struct engine {
	s_keyset *env;		/**< __borrow__ environment */
	s_parser *parser;	/**< parser */
	s_file *in;		/**< input */
	s_file *out;		/**< output (may be null) */
	s_chunk *pre;		/**< prefix */
	size_t *kpre;		/**< prefix KMP */
	s_chunk *suf;		/**< suffix */
	size_t *ksuf;		/**< suffix KMP */
	s_chunk *save;		/**< command */
	int ncopy;		/**< -1 wait, 0 print, 1 done */
	int depth;		/**< no-print depth */
};
typedef struct engine s_engine;

/**
 * @brief Compute the KMP table of a chunk.
 *
 * Invariants:
 * - `forall 0 < i < chunk->len, 0 <= (*kmp)[i] < i`
 */
static int /** @return 0 on success, -1 on error */
kmp_table(
	size_t **kmp,		/**< __return__ KMP table */
	const s_chunk *chunk	/**< __borrow__ chunk to match */
	/**< - must satisfy `chunk->len > 0` */
)
{
	const char *const d = chunk->data;

	assert(chunk->len);
	*kmp = malloc(chunk->len * sizeof(**kmp));
	if (!*kmp) {
		warn("malloc()");
		return -1;
	}

	for (size_t i = 1; i < chunk->len; i++) {
		size_t j = i - 1;
		while (j > 0 && memcmp(d, &d[i - j], j))
			j--;
		(*kmp)[i] = j;
	}

	return 0;
}

static int /**< @return 0 on success, -1 on error */
copy_save(
	int copy,		/**< whether to copy */
	s_file *file,		/**< __borrow__ output file */
	s_chunk *save,		/**< __borrow__ save chunk (may be null) */
	const char *data,	/**< __borrow__ source */
	size_t len		/**< source length */
)
{
	if (copy && file_write(file, data, len))
		return -1;
	if (save && chunk_push(save, data, len))
		return -1;
	return 0;
}

/** @brief Read until a mark is reached. */
static int /**< @return 0 on success, 1 on end-of-file, -1 on error */
read_mark(
	s_engine *engine,	/**< __borrow__ engine */
	const s_chunk *mark,	/**< __borrow__ mark chunk */
	const size_t *kmp_,	/**< __borrow__ mark KMP */
	int copy,		/**< whether to copy */
	s_chunk *save		/**< __borrow__ save chunk (may be null) */
)
{
	int ret = 0;
	size_t imark;
	const size_t *kmp;

	if (!mark->len)
		return 0;

	kmp = kmp_;
	if (!kmp) {
		size_t *tmp;

		if (kmp_table(&tmp, mark))
			return -1;

		kmp = tmp;
	}

	if (save)
		save->len = 0;

	for (imark = 0; imark < mark->len; imark++) {
		char c;

		ret = file_read(engine->in, &c);
		if (ret == -1)
			goto fail;
		if (ret == 1)
			break;
		assert(!ret);

		while (mark->data[imark] != c) {
			if (!imark) {
				if (copy_save(copy, engine->out, save, &c, 1))
					goto fail;
				imark--;
				break;
			}

			if (copy_save(copy, engine->out, save,
				      mark->data, imark - kmp[imark]))
				goto fail;

			imark = kmp[imark];
		}
	}

	if (imark < mark->len &&
	    copy_save(copy, engine->out, save, mark->data, imark))
		goto fail;

	goto end;

fail:
	ret = -1;
end:
	if (!kmp_)
		free((void *)kmp);
	return ret;
}

/**
 * @brief Set prefix and suffix (KMP included) by reading header.
 *
 * Assigns:
 * - `engine->pre`
 * - `engine->kpre`
 * - `engine->suf`
 * - `engine->ksuf`
 */
static int /** @return 0 on success, -1 on error */
read_header(
	s_engine *engine	/**< __borrow__ engine */
)
{
	int ret = -1;
	int sret;
	s_chunk *ffactor = NULL;

	if (chunk_string_ctor(&ffactor, "ffactor"))
		return -1;

	if (chunk_empty_ctor(&engine->pre, 16))
		goto free;

	sret = read_mark(engine, ffactor, NULL, 0, engine->pre);
	if (sret) {
		if (sret == 1)
			warnx("Incomplete header (ffactor marker not found)");
		goto free;
	}

	if (!engine->pre->len) {
		warnx("Empty prefix");
		goto free;
	}

	if (kmp_table(&engine->kpre, engine->pre))
		goto free;

	if (chunk_empty_ctor(&engine->suf, 16))
		goto free;

	sret = read_mark(engine, engine->pre, engine->kpre, 0, engine->suf);
	if (sret) {
		if (sret == 1)
			warnx("Incomplete header (second prefix not found)");
		goto free;
	}

	if (!engine->suf->len) {
		warnx("Empty suffix");
		goto free;
	}

	if (kmp_table(&engine->ksuf, engine->suf))
		goto free;

	sret = read_mark(engine, engine->suf, engine->ksuf, 0, ffactor);
	if (sret) {
		if (sret == 1)
			warnx("Incomplete header (second suffix not found)");
		goto free;
	}

	if (ffactor->len) {
		warnx("Non-empty second part");
		goto free;
	}

	ret = 0;

free:
	chunk_dtor(ffactor);
	return ret;
}

enum cmd {
	CMD_IF_TRUE,
	CMD_IF_FALSE,
	CMD_ELIF_TRUE,
	CMD_ELIF_FALSE,
	CMD_ELSE,
	CMD_ENDIF,
	CMD_OTHER,
};

static int /** @return CMD_ on success, -1 on error */
command(
	s_keyset *env,		/**< __borrow__ environment */
	const char *cmd,	/**< __borrow__ command to parse */
	int copy		/**< whether we are active */
)
{
	while (*cmd == ' ')
		cmd++;

	if (MATCH(cmd, "if ")) {
		switch (eval_cond(env, cmd)) {
		case 0: return CMD_IF_FALSE;
		case 1: return CMD_IF_TRUE;
		case -1: return -1;
		default: assert(0);
		}
	} else if (MATCH(cmd, "endif")) {
		if (*cmd) {
			warnx("Garbage after endif");
			return -1;
		}
		return CMD_ENDIF;
	} else if (MATCH(cmd, "else")) {
		if (*cmd) {
			warnx("Garbage after else");
			return -1;
		}
		return CMD_ELSE;
	} else if (MATCH(cmd, "elif ")) {
		switch (eval_cond(env, cmd)) {
		case 0: return CMD_ELIF_FALSE;
		case 1: return CMD_ELIF_TRUE;
		case -1: return -1;
		default: assert(0);
		}
	} else if (MATCH(cmd, "set ")) {
		if (*key_parse(cmd)) {
			warnx("Bad key '%s'", cmd);
			return -1;
		}
		char *key = strdup(cmd);
		if (!key) {
			warn("strdup()");
			return -1;
		}
		if (keyset_add(env, key))
			return -1;
		return CMD_OTHER;
	} else if (MATCH(cmd, "- ") || !cmd[0]) {
		return CMD_OTHER;
	} else if (MATCH(cmd, "error ")) {
		if (copy) {
			warnx("User error '%s'", cmd);
			return -1;
		}
		return CMD_OTHER;
	} else {
		warnx("Unrecognized command '%s'", cmd);
		return -1;
	}

#ifdef NDEBUG
	return -1;
#endif
}

enum state {
	STATE_CONTENT,
	STATE_ELSE,
	STATE_ENDIF,
	STATE_MAX,
};

static int /** @return 0 on success, -1 on error */
fstate_content(
	char state,
	void *engine
)
{
	int ret;
	int cmd;
	s_engine *e = engine;

	ret = read_mark(e, e->pre, e->kpre, !e->ncopy, NULL);
	if (ret == -1)
		return -1;
	if (ret == 1) {
		if (state == STATE_CONTENT)
			return 0;
		warnx("Reached end-of-file before endif");
		return -1;
	}
	assert(!ret);

	ret = read_mark(e, e->suf, e->ksuf, 0, e->save);
	if (ret == -1)
		return -1;
	if (ret == 1) {
		if (state == STATE_CONTENT && !e->pre->len && !e->save->len)
			return 0;
		warnx("Reached end-of-file before endif");
		return -1;
	}
	assert(!ret);

	if (chunk_push(e->save, "\0", 1))
		return -1;

	cmd = command(e->env, e->save->data, !e->ncopy);
	switch (cmd) {
	case CMD_IF_TRUE:
	case CMD_IF_FALSE:
		if (parser_push(e->parser, state))
			return -1;
		if (parser_push(e->parser, STATE_ELSE))
			return -1;
		if (!e->ncopy) {
			assert(!e->depth);
			if (cmd == CMD_IF_FALSE)
				e->ncopy = -1;
		} else {
			e->depth++;
		}
		break;
	case CMD_ELIF_TRUE:
	case CMD_ELIF_FALSE:
		if (state != STATE_ELSE) {
			warnx("Unexpected elif");
			return -1;
		}
		if (parser_push(e->parser, STATE_ELSE))
			return -1;
		if (!e->depth) {
			if (e->ncopy == -1) {
				if (cmd == CMD_ELIF_TRUE)
					e->ncopy = 0;
			} else {
				e->ncopy = 1;
			}
		}
		break;
	case CMD_ELSE:
		if (state != STATE_ELSE) {
			warnx("Unexpected else");
			return -1;
		}
		if (parser_push(e->parser, STATE_ENDIF))
			return -1;
		if (!e->depth) {
			if (e->ncopy == -1)
				e->ncopy = 0;
			else
				e->ncopy = 1;
		}
		break;
	case CMD_ENDIF:
		if (state == STATE_CONTENT) {
			warnx("Unexpected endif");
			return -1;
		}
		if (!e->depth) {
			e->ncopy = 0;
		} else {
			e->depth--;
		}
		break;
	case CMD_OTHER:
		if (parser_push(e->parser, state))
			return -1;
		break;
	case -1: return -1;
	default: assert(0);
	}

	return 0;
}

static f_state states[STATE_MAX] = {
	[STATE_CONTENT] = fstate_content,
	[STATE_ELSE] = fstate_content,
	[STATE_ENDIF] = fstate_content,
};

/**
 * @brief Create and initialize an engine.
 *
 * If `prefsuf == NULL` then the header is read from input file.
 * Otherwise `presuf` contains the prefix and suffix.
 */
static int /** @return 0 on success, -1 on error */
engine_init(
	s_keyset *env,		/**< __borrow__ environment */
	s_file *in,		/**< __take__ input file */
	s_file *out,		/**< __take__ output file (may be null) */
	s_chunk **presuf	/**< __borrow__.__take__ prefix/suffix */
)
{
	int ret = -1;
	s_engine *engine;

	engine = malloc(sizeof(*engine));
	if (!engine) {
		warn("malloc()");
		goto free_inout;
	}

	memset(engine, 0, sizeof(*engine));
	engine->env = env;
	engine->in = in;
	engine->out = out;

	if (presuf) {

		engine->pre = presuf[0];
		engine->suf = presuf[1];

		if (engine->pre->len && kmp_table(&engine->kpre, engine->pre))
			goto free_engine;

		assert(engine->suf->len);
		if (kmp_table(&engine->ksuf, engine->suf))
			goto free_engine;

	} else {
		if (read_header(engine))
			goto free_engine;
	}

	if (chunk_empty_ctor(&engine->save, 256))
		goto free_engine;

	if (parser_ctor(&engine->parser, states, STATE_MAX))
		goto free_engine;

	if (parser_push(engine->parser, STATE_CONTENT))
		goto free_engine;

	if (parser_parse(engine->parser, engine))
		goto free_engine;

	ret = 0;

free_engine:
	parser_dtor(engine->parser);
	chunk_dtor(engine->pre);
	free(engine->kpre);
	chunk_dtor(engine->suf);
	free(engine->ksuf);
	chunk_dtor(engine->save);
	free(engine);
free_inout:
	if (ret) {
		file_warn(in);
		if (out)
			file_warn(out);
	}
	file_dtor(in);
	file_dtor(out);
	return ret;
}

extern int
engine_file(s_keyset *env, const char *p_in, const char *p_out)
{
	s_file *in, *out;
	FILE *s_in, *s_out;

	s_in = fopen(p_in, "r");
	if (!s_in) {
		warn("fopen(%s, r)", p_in);
		return -1;
	}

	s_out = fopen(p_out, "w");
	if (!s_out) {
		warn("fopen(%s, w)", p_out);
		fclose(s_in);
		return -1;
	}

	p_in = strdup(p_in);
	if (!p_in) {
		warn("strdup()");
		goto close_stream;
	}

	p_out = strdup(p_out);
	if (!p_out) {
		warn("strdup()");
		free((void *)p_in);
		goto close_stream;
	}

	if (file_ctor(&in, s_in, p_in))
		goto free_path;

	if (file_ctor(&out, s_out, p_out)) {
		file_dtor(in);
		goto free_path;
	}

	return engine_init(env, in, out, NULL);

free_path:
	free((void *)p_out);
	free((void *)p_in);
close_stream:
	fclose(s_out);
	fclose(s_in);
	return -1;
}

extern int
engine_env(s_keyset *env, const char *path)
{
	s_chunk *presuf[2];
	s_file *file;
	FILE *stream;

	if (chunk_string_ctor(&presuf[0], ""))
		return -1;

	if (chunk_string_ctor(&presuf[1], "\n")) {
		chunk_dtor(presuf[0]);
		return -1;
	}

	stream = fopen(path, "r");
	if (!stream) {
		warn("fopen(%s, r)", path);
		goto free_chunk;
	}

	path = strdup(path);
	if (!path) {
		warn("strdup()");
		goto close_stream;
	}

	if (file_ctor(&file, stream, path))
		goto free_path;

	return engine_init(env, file, NULL, presuf);

free_path:
	free((void *)path);
close_stream:
	fclose(stream);
free_chunk:
	chunk_dtor(presuf[1]);
	chunk_dtor(presuf[0]);
	return -1;
}

extern int
engine_std(s_keyset *env)
{
	s_file *in, *out;
	const char *p_in, *p_out;

	p_in = strdup("<stdin>");
	if (!p_in) {
		warn("strdup()");
		return -1;
	}

	p_out = strdup("<stdout>");
	if (!p_out) {
		warn("strdup()");
		free((void *)p_in);
		return -1;
	}

	if (file_ctor(&in, stdin, p_in))
		goto free_path;

	if (file_ctor(&out, stdout, p_out)) {
		file_dtor(in);
		goto free_path;
	}

	return engine_init(env, in, out, NULL);

free_path:
	free((void *)p_out);
	free((void *)p_in);
	return -1;
}
