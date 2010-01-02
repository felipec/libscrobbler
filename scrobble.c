#include "scrobble.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <glib.h>
#include <libsoup/soup.h>

struct sr_session_priv {
	char *url;
	char *client_id;
	char *client_ver;
	char *user, *hash_pwd;
	GQueue *queue;
	SoupSession *soup;
};

sr_session_t *
sr_session_new(const char *url,
	       const char *client_id,
	       const char *client_ver)
{
	sr_session_t *s;
	struct sr_session_priv *priv;
	s = calloc(1, sizeof(*s));
	s->priv = priv = calloc(1, sizeof(*priv));
	priv->queue = g_queue_new();
	priv->url = strdup(url);
	priv->client_id = strdup(client_id);
	priv->client_ver = strdup(client_ver);
	priv->soup = soup_session_async_new();
	return s;
}

void
sr_session_free(sr_session_t *s)
{
	struct sr_session_priv *priv = s->priv;

	soup_session_abort(priv->soup);
	g_object_unref(priv->soup);
	while (!g_queue_is_empty(priv->queue)) {
		sr_track_t *t;
		t = g_queue_pop_head(priv->queue);
		sr_track_free(t);
	}
	g_queue_free(priv->queue);
	free(priv->url);
	free(priv->client_id);
	free(priv->client_ver);
	free(priv->user);
	g_free(priv->hash_pwd);
	free(s->priv);
	free(s);
}

void sr_session_set_cred(sr_session_t *s,
			 char *user,
			 char *password)
{
	struct sr_session_priv *priv = s->priv;
	priv->user = strdup(user);
	priv->hash_pwd = g_compute_checksum_for_string(G_CHECKSUM_MD5, password, -1);
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
	free(t->album);
	free(t->mbid);
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
	case 'i':
		t->timestamp = atoi(value);
		break;
	case 'o':
		t->source = value[0];
		break;
	case 'r':
		t->rating = value[0];
		break;
	case 'l':
		t->length = atoi(value);
		break;
	case 'b':
		t->album = strdup(value);
		break;
	case 'n':
		t->position = atoi(value);
		break;
	case 'm':
		t->mbid = strdup(value);
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
	fprintf(f, "i: %u\n", t->timestamp);
	fprintf(f, "o: %c\n", t->source);
	if (t->rating)
		fprintf(f, "r: %c\n", t->rating);
	fprintf(f, "l: %i\n", t->length);
	if (t->album)
		fprintf(f, "b: %s\n", t->album);
	if (t->position)
		fprintf(f, "n: %i\n", t->position);
	if (t->mbid)
		fprintf(f, "m: %s\n", t->mbid);

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

void
sr_session_handshake(sr_session_t *s)
{
	struct sr_session_priv *priv = s->priv;
	gchar *auth, *tmp;
	glong timestamp;
	gchar *handshake_url;
	SoupMessage *message;
	GTimeVal time_val;

	g_get_current_time(&time_val);
	timestamp = time_val.tv_sec;

	tmp = g_strdup_printf("%s%li", priv->hash_pwd, timestamp);
	auth = g_compute_checksum_for_string(G_CHECKSUM_MD5, tmp, -1);
	g_free(tmp);

	handshake_url = g_strdup_printf("%s&p=1.2.1&c=%s&v=%s&u=%s&t=%li&a=%s",
					priv->url,
					priv->client_id,
					priv->client_ver,
					priv->user,
					timestamp,
					auth);

	message = soup_message_new("GET", handshake_url);
	soup_session_queue_message(priv->soup,
				   message,
				   NULL,
				   s);

	g_free(handshake_url);
	g_free(auth);
}
