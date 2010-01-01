#include "scrobble.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <glib.h>

struct sr_session_priv {
	GQueue *queue;
};

sr_session_t *
sr_session_new(void)
{
	sr_session_t *s;
	struct sr_session_priv *priv;
	s = calloc(1, sizeof(*s));
	s->priv = priv = calloc(1, sizeof(*priv));
	priv->queue = g_queue_new();
	return s;
}

void
sr_session_free(sr_session_t *s)
{
	struct sr_session_priv *priv = s->priv;
	while (!g_queue_is_empty(priv->queue)) {
		sr_track_t *t;
		t = g_queue_pop_head(priv->queue);
		sr_track_free(t);
	}
	g_queue_free(priv->queue);
	free(s->priv);
	free(s);
}

sr_track_t *
sr_track_new(void)
{
	sr_track_t *t;
	t = calloc(1, sizeof(*t));
	return t;
}

void
sr_track_free(sr_track_t *t)
{
	free(t->artist);
	free(t->title);
	free(t);
}

void
sr_session_add_track(sr_session_t *s,
		     sr_track_t *t)
{
	struct sr_session_priv *priv = s->priv;
	g_queue_push_tail(priv->queue, t);
}

static inline void
got_field(sr_track_t *t,
	  char k,
	  const char *value)
{
	switch (k) {
	case 'a':
		t->artist = strdup(value);
		break;
	case 't':
		t->title = strdup(value);
		break;
	default:
		break;
	}
}

static inline bool
track_is_valid(sr_track_t *t)
{
	return !!t->artist;
}

int
sr_session_load_list(sr_session_t *s,
		     const char *file)
{
	FILE *f;
	char c, *p;
	char k, v[255];
	int stage = 1;
	sr_track_t *t;

	f = fopen(file, "r");
	if (!f)
		return 1;
	t = sr_track_new();
	while (true) {
		c = getc(f);
		if (stage == 1) {
			if (c == '\n') {
				sr_session_add_track(s, t);
				t = sr_track_new();
				continue;
			}
			if (c == EOF) {
				if (track_is_valid(t))
					sr_session_add_track(s, t);
				break;
			}
			k = c;
			p = v;
			fseek(f, 2, SEEK_CUR);
			stage++;
		}
		else if (stage == 2) {
			*p = c;
			if (c == '\n') {
				*p = '\0';
				got_field(t, k, v);
				stage = 1;
			}
			p++;
		}
	}
	fclose(f);
	return 0;
}

static void
store_track(void *data,
	    void *user_data)
{
	sr_track_t *t = data;
	FILE *f = user_data;

	fprintf(f, "a: %s\n", t->artist);
	fprintf(f, "t: %s\n", t->title);

	fputc('\n', f);
}

int
sr_session_store_list(sr_session_t *s,
		      const char *file)
{
	FILE *f;
	struct sr_session_priv *priv = s->priv;

	f = fopen(file, "w");
	g_queue_foreach(priv->queue, store_track, f);
	fclose(f);
	return 0;
}

void
sr_session_test(sr_session_t *s)
{
	struct sr_session_priv *priv = s->priv;
	g_queue_foreach(priv->queue, store_track, stdout);
}
