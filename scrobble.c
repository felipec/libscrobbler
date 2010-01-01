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
