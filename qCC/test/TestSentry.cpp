/**
 * @file TestSentry.cpp
 *
 * @brief Sentry crash/reporting SDK smoke tests
 *
 * CloudCompare is distributed under the GNU General Public License v2 or later.
 *
 * These tests verify that the Sentry SDK is correctly linked and its public API
 * is callable at runtime.  They do NOT test event delivery to Sentry — that
 * requires a valid DSN and network access which may not be available in CI.
 */

#include <QTemporaryDir>
#include <QTest>

#ifdef CC_USE_SENTRY
#include <sentry.h>
#endif

class TestSentry : public QObject
{
	Q_OBJECT

  private slots:

	/**
	 * @brief Smoke test: verify the Sentry SDK API is callable without crashing.
	 *
	 * We initialise the SDK with the same DSN as main.cpp, create a message
	 * event, capture it, and shut down cleanly.  Whether the captured event
	 * actually reaches Sentry depends on network availability and is not
	 * asserted here.
	 */
	void smokeTest()
	{
#ifdef CC_USE_SENTRY
		QTemporaryDir dbDir;
		QVERIFY(dbDir.isValid());

		sentry_options_t* options = sentry_options_new();
		QVERIFY(options != nullptr);

		// Same DSN as qCC/main.cpp
		sentry_options_set_dsn(
			options,
			"https://ac001120bfa96ba91d2ed97c62e632ad@o494653.ingest.sentry.io/4511938553053184");
		sentry_options_set_database_path(options, dbDir.path().toUtf8().constData());
		sentry_options_set_debug(options, 0);

		// Initialise — return value intentionally unchecked.  The SDK may return
		// non-zero if the transport cannot connect, but the API is still callable.
		sentry_init(options);

		// Build a minimal message event (sentry-native 0.16.x API)
		sentry_value_t event    = sentry_value_new_object();
		sentry_value_t message = sentry_value_new_string("Hello from CloudCompare test suite");
		sentry_value_t level   = sentry_value_new_string("info");
		sentry_value_t platform = sentry_value_new_string("native");
		sentry_value_t type    = sentry_value_new_string("event");

		sentry_value_set_by_key(event, "message",  message);
		sentry_value_set_by_key(event, "level",    level);
		sentry_value_set_by_key(event, "platform", platform);
		sentry_value_set_by_key(event, "type",     type);

		// Capture the event — must not crash even when the SDK is in offline mode.
		// The UUID may be nil if the event was dropped by sampling; that is not
		// a crash and does not indicate a linking or ABI problem.
		sentry_uuid_t eventId = sentry_capture_event(event);
		(void)eventId; // suppress unused-variable warning

		// Clean shutdown — must not crash.
		sentry_close();
#else
		QSKIP("CC_USE_SENTRY is not defined — sentry-native was not found at build time");
#endif
	}
};

QTEST_MAIN(TestSentry)
#include "TestSentry.moc"
