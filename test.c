#include "scrobble.h"

#include <string.h> /* for strdup */

static void add_tracks(sr_session_t *s)
{
	sr_track_t *t = sr_track_new();
	t->artist = strdup("Weezer");
	t->title = strdup("Island in the Sun");
	t->timestamp = 1262338291;
	t->length = 210;
	t->source = 'P';
	sr_session_add_track(s, t);
}

int main(void)
{
	sr_session_t *s;
	s = sr_session_new(SR_LASTFM_URL, "tst", "1.0");
	add_tracks(s);
	sr_session_load_list(s, "list");
	sr_session_test(s);
	sr_session_store_list(s, "foo");
	sr_session_free(s);
	return 0;
}
