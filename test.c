#include "scrobble.h"

#include <string.h> /* for strdup */

#include <glib.h>
#include <glib-object.h>

static gboolean
load_cred(sr_session_t *s,
	  const char *id)
{
	gchar *file;
	GKeyFile *keyfile;
	gchar *username = NULL, *password = NULL;
	gboolean ok;

	keyfile = g_key_file_new();
	file = g_build_filename(g_get_user_config_dir(),
				"scrobbler", NULL);

	ok = g_key_file_load_from_file(keyfile, file, G_KEY_FILE_NONE, NULL);
	if (!ok)
		goto leave;

	username = g_key_file_get_string(keyfile, id, "username", NULL);
	password = g_key_file_get_string(keyfile, id, "password", NULL);

	ok = username && password;
	if (!ok)
		goto leave;

	sr_session_set_cred(s, username, password);

leave:
	g_free(username);
	g_free(password);
	g_key_file_free(keyfile);

	return ok;
}

int main(void)
{
	sr_session_t *s;

	g_type_init();
	if (!g_thread_supported())
		g_thread_init(NULL);

	s = sr_session_new(SR_LASTFM_URL, "tst", "1.0");
	load_cred(s, "lastfm");
	sr_session_load_list(s, "list");
	sr_session_test(s);
	sr_session_store_list(s, "foo");
	sr_session_free(s);
	return 0;
}
