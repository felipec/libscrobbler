#include "scrobble.h"

#include <stdlib.h>

sr_session_t *
sr_session_new(void)
{
	sr_session_t *s;
	s = calloc(1, sizeof(*s));
	return s;
}

void
sr_session_free(sr_session_t *s)
{
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
