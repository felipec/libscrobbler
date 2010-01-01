#include "scrobble.h"

#include <string.h> /* for strdup */

static void add_tracks(sr_session_t *s)
{
	sr_track_t *t = sr_track_new();
	t->artist = strdup("Weezer");
	t->title = strdup("Island in the Sun");
	sr_session_add_track(s, t);
}

int main(void)
{
	sr_session_t *s;
	s = sr_session_new();
	add_tracks(s);
	sr_session_load_list(s, "list");
	sr_session_test(s);
	sr_session_store_list(s, "foo");
	sr_session_free(s);
	return 0;
}
