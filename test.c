#include "scrobble.h"

int main(void)
{
	sr_session_t *s;
	s = sr_session_new();
	sr_session_free(s);
	return 0;
}
