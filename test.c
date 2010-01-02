#include "scrobble.h"

#include <string.h> /* for strdup */

#include <glib.h>
#include <glib-object.h>

static GMainLoop *main_loop;

static gboolean
load_cred(sr_session_t *s,
	  GKeyFile *keyfile,
	  const char *id)
{
	gchar *username = NULL, *password = NULL;
	gboolean ok;

	username = g_key_file_get_string(keyfile, id, "username", NULL);
	password = g_key_file_get_string(keyfile, id, "password", NULL);

	ok = username && password;
	if (!ok)
		goto leave;

	sr_session_set_cred(s, username, password);

leave:
	g_free(username);
	g_free(password);

	return ok;
}

static void error_cb(int fatal,
		     const char *msg)
{
	g_warning(msg);
	if (fatal)
		g_main_loop_quit(main_loop);
}

static gboolean timeout(void *data)
{
	sr_session_submit(data);
	return TRUE;
}

int main(void)
{
	sr_session_t *s = NULL;
	GKeyFile *keyfile;
	gchar *file;
	gboolean ok;

	g_type_init();
	if (!g_thread_supported())
		g_thread_init(NULL);

	keyfile = g_key_file_new();

	file = g_build_filename(g_get_user_config_dir(),
				"scrobbler", NULL);

	ok = g_key_file_load_from_file(keyfile, file, G_KEY_FILE_NONE, NULL);
	if (!ok)
		goto leave;

	s = sr_session_new(SR_LASTFM_URL, "tst", "1.0");
	s->error_cb = error_cb;
	load_cred(s, keyfile, "lastfm");
	sr_session_load_list(s, "list");
	sr_session_test(s);
	sr_session_store_list(s, "foo");
	sr_session_handshake(s);
	g_timeout_add_seconds(30, timeout, s);

	main_loop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(main_loop);

leave:
	sr_session_free(s);
	g_key_file_free(keyfile);

	return 0;
}
