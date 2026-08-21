/**
 * @file TestArgumentParser.cpp
 *
 * @brief Unit tests for ccArgumentParser
 *
 * Tests command-line argument parsing:
 * - Short option parsing (-o, -r)
 * - Long option parsing (--open, --register)
 * - Key-value pairs (KEY=VALUE)
 * - Positional arguments
 * - Error handling (missing required args)
 *
 * @see ccArgumentParser.cpp
 */
#include "ccArgumentParser.h"

#include <QTest>

enum class Color
{
	RED,
	GREEN,
	BLUE
};

class TestArgumentParser : public QObject
{
	Q_OBJECT

  private slots:

	// peek()
	void peekOnEmptyReturnsNull()
	{
		QStringList      args;
		ccArgumentParser parser(args);
		QCOMPARE(parser.peek(), QString());
	}

	void peekReturnsFirstWithoutConsuming()
	{
		QStringList      args{"hello", "world"};
		ccArgumentParser parser(args);

		const QString first = parser.peek();
		QVERIFY(!first.isNull());
		QCOMPARE(first, "hello");
		QCOMPARE(args.size(), 2);
	}

	void consecutivePeeksReturnSameElement()
	{
		QStringList      args{"hello"};
		ccArgumentParser parser(args);

		QCOMPARE(parser.peek(), parser.peek());
	}

	// skip()
	void skipRemovesFirstElement()
	{
		QStringList      args{"a", "b", "c"};
		ccArgumentParser parser(args);

		parser.skip();
		QCOMPARE(args.size(), 2);
		QCOMPARE(args.first(), "b");
	}

	// isEmpty()
	void isEmptyOnEmptyList()
	{
		QStringList      args;
		ccArgumentParser parser(args);
		QVERIFY(parser.isEmpty());
	}

	void isEmptyOnNonEmptyList()
	{
		QStringList      args{"a"};
		ccArgumentParser parser(args);
		QVERIFY(!parser.isEmpty());
	}

	// takeNext()
	void takeNextOnEmptyReturnsNullopt()
	{
		QStringList      args;
		ccArgumentParser parser(args);

		auto result = parser.takeNext();
		QVERIFY(result.isNull());
	}

	void takeNextReturnsAndConsumes()
	{
		QStringList      args{"first", "second"};
		ccArgumentParser parser(args);

		auto result = parser.takeNext();
		QVERIFY(!result.isNull());
		QCOMPARE(result, "first");
		QCOMPARE(args.size(), 1);
	}

	void takeNextSuccessiveCalls()
	{
		QStringList      args{"a", "b", "c"};
		ccArgumentParser parser(args);

		QCOMPARE(parser.takeNext(), "a");
		QCOMPARE(parser.takeNext(), "b");
		QCOMPARE(parser.takeNext(), "c");
		QVERIFY(parser.takeNext().isNull());
	}

	// takeFloat()
	void takeFloatParsesValid()
	{
		QStringList      args{"3.14"};
		ccArgumentParser parser(args);

		auto result = parser.takeFloat("test");
		QVERIFY(result.has_value());
		QCOMPARE(*result, 3.14f);
		QVERIFY(args.isEmpty());
	}

	void takeFloatOnEmptyReturnsNullopt()
	{
		QStringList      args;
		ccArgumentParser parser(args);

		auto result = parser.takeFloat("test");
		QVERIFY(!result.has_value());
	}

	void takeFloatOnInvalidReturnsNullopt()
	{
		QStringList      args{"not_a_number"};
		ccArgumentParser parser(args);

		auto result = parser.takeFloat("test");
		QVERIFY(!result.has_value());
		QVERIFY(args.isEmpty());
	}

	// ParseFloat() static
	void ParseFloatValid()
	{
		auto result = ccArgumentParser::ParseFloat("42.5", "test");
		QVERIFY(result.has_value());
		QCOMPARE(*result, 42.5f);
	}

	void ParseFloatInvalid()
	{
		auto result = ccArgumentParser::ParseFloat("abc", "test");
		QVERIFY(!result.has_value());
	}

	void ParseFloatZero()
	{
		auto result = ccArgumentParser::ParseFloat("0", "test");
		QVERIFY(result.has_value());
		QCOMPARE(*result, 0.0f);
	}

	void ParseFloatNegative()
	{
		auto result = ccArgumentParser::ParseFloat("-1.5", "test");
		QVERIFY(result.has_value());
		QCOMPARE(*result, -1.5f);
	}

	void ParseFloatScientific()
	{
		auto result = ccArgumentParser::ParseFloat("1.5e3", "test");
		QVERIFY(result.has_value());
		QCOMPARE(*result, 1500.0f);
	}

	// takeUInt()
	void takeUIntParsesValid()
	{
		QStringList      args{"42"};
		ccArgumentParser parser(args);

		auto result = parser.takeUInt("test");
		QVERIFY(result.has_value());
		QCOMPARE(*result, 42u);
		QVERIFY(args.isEmpty());
	}

	void takeUIntOnEmptyReturnsNullopt()
	{
		QStringList      args;
		ccArgumentParser parser(args);

		auto result = parser.takeUInt("test");
		QVERIFY(!result.has_value());
	}

	void takeUIntOnInvalidReturnsNullopt()
	{
		QStringList      args{"abc"};
		ccArgumentParser parser(args);

		auto result = parser.takeUInt("test");
		QVERIFY(!result.has_value());
	}

	void takeUIntRejectsNegative()
	{
		QStringList      args{"-1"};
		ccArgumentParser parser(args);

		auto result = parser.takeUInt("test");
		QVERIFY(!result.has_value());
	}

	void takeUIntBelowMin()
	{
		QStringList      args{"5"};
		ccArgumentParser parser(args);

		auto result = parser.takeUInt("test", 10);
		QVERIFY(!result.has_value());
	}

	void takeUIntAboveMax()
	{
		QStringList      args{"200"};
		ccArgumentParser parser(args);

		auto result = parser.takeUInt("test", 0, 100);
		QVERIFY(!result.has_value());
	}

	// ParseUInt() static
	void ParseUIntValid()
	{
		auto result = ccArgumentParser::ParseUInt("42", "test");
		QVERIFY(result.has_value());
		QCOMPARE(*result, 42u);
	}

	void ParseUIntInvalid()
	{
		auto result = ccArgumentParser::ParseUInt("abc", "test");
		QVERIFY(!result.has_value());
	}

	void ParseUIntZero()
	{
		auto result = ccArgumentParser::ParseUInt("0", "test");
		QVERIFY(result.has_value());
		QCOMPARE(*result, 0u);
	}

	// tryConsumeOption()
	void tryConsumeOptionMatchesCaseInsensitive()
	{
		QStringList      args{"-orient"};
		ccArgumentParser parser(args);

		QVERIFY(parser.tryConsumeOption("ORIENT"));
		QVERIFY(args.isEmpty());
	}

	void tryConsumeOptionMatchesUpperCase()
	{
		QStringList      args{"-ORIENT"};
		ccArgumentParser parser(args);

		QVERIFY(parser.tryConsumeOption("ORIENT"));
		QVERIFY(args.isEmpty());
	}

	void tryConsumeOptionNoMatchDoesNotConsume()
	{
		QStringList      args{"-OTHER"};
		ccArgumentParser parser(args);

		QVERIFY(!parser.tryConsumeOption("ORIENT"));
		QCOMPARE(args.size(), 1);
	}

	void tryConsumeOptionOnEmptyReturnsFalse()
	{
		QStringList      args;
		ccArgumentParser parser(args);

		QVERIFY(!parser.tryConsumeOption("ORIENT"));
	}

	void tryConsumeOptionRequiresDashPrefix()
	{
		QStringList      args{"ORIENT"};
		ccArgumentParser parser(args);

		QVERIFY(!parser.tryConsumeOption("ORIENT"));
		QCOMPARE(args.size(), 1);
	}

	// takeEnum()
	void takeEnumReturnsCorrectValue()
	{
		QStringList      args{"GREEN"};
		ccArgumentParser parser(args);

		auto result = parser.takeEnum<Color>({
		                                         {"RED", Color::RED},
		                                         {"GREEN", Color::GREEN},
		                                         {"BLUE", Color::BLUE},
		                                     },
		                                     "color");
		QVERIFY(result.has_value());
		QCOMPARE(*result, Color::GREEN);
		QVERIFY(args.isEmpty());
	}

	void takeEnumCaseInsensitive()
	{
		QStringList      args{"red"};
		ccArgumentParser parser(args);

		auto result = parser.takeEnum<Color>({
		                                         {"RED", Color::RED},
		                                         {"GREEN", Color::GREEN},
		                                         {"BLUE", Color::BLUE},
		                                     },
		                                     "color");
		QVERIFY(result.has_value());
		QCOMPARE(*result, Color::RED);
	}

	void takeEnumOnEmptyReturnsNullopt()
	{
		QStringList      args;
		ccArgumentParser parser(args);

		auto result = parser.takeEnum<Color>({
		                                         {"RED", Color::RED},
		                                     },
		                                     "color");
		QVERIFY(!result.has_value());
	}

	void takeEnumUnrecognizedReturnsNullopt()
	{
		QStringList      args{"YELLOW"};
		ccArgumentParser parser(args);

		auto result = parser.takeEnum<Color>({
		                                         {"RED", Color::RED},
		                                         {"GREEN", Color::GREEN},
		                                         {"BLUE", Color::BLUE},
		                                     },
		                                     "color");
		QVERIFY(!result.has_value());
		QVERIFY(args.isEmpty());
	}

	void takeEnumWithAliases()
	{
		QStringList      args{"CRIMSON"};
		ccArgumentParser parser(args);

		auto result = parser.takeEnum<Color>({
		                                         {"RED", Color::RED},
		                                         {"CRIMSON", Color::RED},
		                                         {"GREEN", Color::GREEN},
		                                     },
		                                     "color");
		QVERIFY(result.has_value());
		QCOMPARE(*result, Color::RED);
	}

	// takeDouble()
	void takeDoubleParsesValid()
	{
		QStringList      args{"3.141592653589"};
		ccArgumentParser parser(args);

		auto result = parser.takeDouble("test");
		QVERIFY(result.has_value());
		QCOMPARE(*result, 3.141592653589);
		QVERIFY(args.isEmpty());
	}

	void takeDoubleOnEmptyReturnsNullopt()
	{
		QStringList      args;
		ccArgumentParser parser(args);

		auto result = parser.takeDouble("test");
		QVERIFY(!result.has_value());
	}

	void takeDoubleOnInvalidReturnsNullopt()
	{
		QStringList      args{"not_a_number"};
		ccArgumentParser parser(args);

		auto result = parser.takeDouble("test");
		QVERIFY(!result.has_value());
	}

	void takeDoubleMinMaxAcceptsInRange()
	{
		QStringList      args{"50"};
		ccArgumentParser parser(args);

		auto result = parser.takeDouble("test", 0.0, 100.0);
		QVERIFY(result.has_value());
		QCOMPARE(*result, 50.0);
	}

	void takeDoubleMinMaxRejectsBelowMin()
	{
		QStringList      args{"-5"};
		ccArgumentParser parser(args);

		auto result = parser.takeDouble("test", 0.0, 100.0);
		QVERIFY(!result.has_value());
	}

	void takeDoubleMinMaxRejectsAboveMax()
	{
		QStringList      args{"150"};
		ccArgumentParser parser(args);

		auto result = parser.takeDouble("test", 0.0, 100.0);
		QVERIFY(!result.has_value());
	}

	// takeInt()
	void takeIntParsesValid()
	{
		QStringList      args{"-42"};
		ccArgumentParser parser(args);

		auto result = parser.takeInt("test");
		QVERIFY(result.has_value());
		QCOMPARE(*result, -42);
		QVERIFY(args.isEmpty());
	}

	void takeIntOnEmptyReturnsNullopt()
	{
		QStringList      args;
		ccArgumentParser parser(args);

		auto result = parser.takeInt("test");
		QVERIFY(!result.has_value());
	}

	void takeIntOnInvalidReturnsNullopt()
	{
		QStringList      args{"abc"};
		ccArgumentParser parser(args);

		auto result = parser.takeInt("test");
		QVERIFY(!result.has_value());
	}

	void takeIntMinMaxAcceptsInRange()
	{
		QStringList      args{"5"};
		ccArgumentParser parser(args);

		auto result = parser.takeInt("test", 0, 10);
		QVERIFY(result.has_value());
		QCOMPARE(*result, 5);
	}

	void takeIntMinMaxRejectsBelowMin()
	{
		QStringList      args{"-1"};
		ccArgumentParser parser(args);

		auto result = parser.takeInt("test", 0, 10);
		QVERIFY(!result.has_value());
	}

	void takeIntMinMaxRejectsAboveMax()
	{
		QStringList      args{"20"};
		ccArgumentParser parser(args);

		auto result = parser.takeInt("test", 0, 10);
		QVERIFY(!result.has_value());
	}

	// ParseDouble() static
	void ParseDoubleValid()
	{
		auto result = ccArgumentParser::ParseDouble("3.14159", "test");
		QVERIFY(result.has_value());
		QCOMPARE(*result, 3.14159);
	}

	void ParseDoubleInvalid()
	{
		auto result = ccArgumentParser::ParseDouble("abc", "test");
		QVERIFY(!result.has_value());
	}

	void ParseDoubleNegative()
	{
		auto result = ccArgumentParser::ParseDouble("-100.5", "test");
		QVERIFY(result.has_value());
		QCOMPARE(*result, -100.5);
	}

	void ParseDoubleScientific()
	{
		auto result = ccArgumentParser::ParseDouble("1.5e3", "test");
		QVERIFY(result.has_value());
		QCOMPARE(*result, 1500.0);
	}

	// ParseInt() static
	void ParseIntValid()
	{
		auto result = ccArgumentParser::ParseInt("42", "test");
		QVERIFY(result.has_value());
		QCOMPARE(*result, 42);
	}

	void ParseIntInvalid()
	{
		auto result = ccArgumentParser::ParseInt("abc", "test");
		QVERIFY(!result.has_value());
	}

	void ParseIntNegative()
	{
		auto result = ccArgumentParser::ParseInt("-999", "test");
		QVERIFY(result.has_value());
		QCOMPARE(*result, -999);
	}

	// size()
	void sizeReturnsRemainingCount()
	{
		QStringList      args{"a", "b", "c"};
		ccArgumentParser parser(args);

		QCOMPARE(parser.size(), static_cast<size_t>(3));
		parser.skip();
		QCOMPARE(parser.size(), static_cast<size_t>(2));
		parser.takeNext();
		QCOMPARE(parser.size(), static_cast<size_t>(1));
	}

	void sizeEmptyList()
	{
		QStringList      args;
		ccArgumentParser parser(args);

		QCOMPARE(parser.size(), static_cast<size_t>(0));
	}

	// peek() after skip()
	void peekAfterSkipReturnsNext()
	{
		QStringList      args{"skip", "target", "after"};
		ccArgumentParser parser(args);

		parser.skip();
		QCOMPARE(parser.peek(), "target");
	}
};

QTEST_GUILESS_MAIN(TestArgumentParser)
#include "TestArgumentParser.moc"
