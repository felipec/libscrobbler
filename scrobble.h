#ifndef SCROBBLE_H
#define SCROBBLE_H

typedef struct sr_session sr_session_t;

struct sr_session {
};

sr_session_t *sr_session_new(void);
void sr_session_free(sr_session_t *s);

#endif /* SCROBBLE_H */
