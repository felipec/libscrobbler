#ifndef SCROBBLE_H
#define SCROBBLE_H

typedef struct sr_track sr_track_t;

struct sr_track {
	char *artist;
	char *title;
};

typedef struct sr_session sr_session_t;

struct sr_session {
	void *priv;
};

sr_session_t *sr_session_new(void);
void sr_session_free(sr_session_t *s);

void sr_session_add_track(sr_session_t *s, sr_track_t *t);
int sr_session_load_list(sr_session_t *s, const char *file);
void sr_session_test(sr_session_t *s);

sr_track_t *sr_track_new(void);
void sr_track_free(sr_track_t *t);

#endif /* SCROBBLE_H */
