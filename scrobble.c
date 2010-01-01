#include "scrobble.h"

#include <stdlib.h>

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
