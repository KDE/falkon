/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2026 Pavel Sobolev <contact@paveloom.dev>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* ============================================================ */
#include "autotests.h"
#include "opensearchengine.h"
#include "opensearchenginetest.h"

// See https://github.com/dewitt/opensearch/blob/master/mediawiki/Specifications/OpenSearch/Extensions/Suggestions/1.1/Draft%201.wiki

void OpenSearchEngineTest::parseSuggestionsBasicTest()
{
    const QByteArray response = R"json([
        "Example",
        ["Example 1", "Example 2"],
        ["Description 1", "Description 2"],
        ["https://example.org/1", "https://example.org/2"]
    ])json";

    const auto suggestions = OpenSearchEngine::parseSuggestions(response);

    QCOMPARE(suggestions.completions.size(), 2);
    QCOMPARE(suggestions.completions.at(0), QSL("Example 1"));
    QCOMPARE(suggestions.completions.at(1), QSL("Example 2"));
    QCOMPARE(suggestions.urls.size(), 2);
    QCOMPARE(suggestions.urls.at(0), QUrl(QSL("https://example.org/1")));
    QCOMPARE(suggestions.urls.at(1), QUrl(QSL("https://example.org/2")));
}

void OpenSearchEngineTest::parseSuggestionsNoUrlsTest()
{
    const QByteArray response = R"json([
        "Example",
        ["Example 1", "Example 2"]
    ])json";

    const auto suggestions = OpenSearchEngine::parseSuggestions(response);

    QCOMPARE(suggestions.completions.size(), 2);
    QCOMPARE(suggestions.completions.at(0), QSL("Example 1"));
    QCOMPARE(suggestions.completions.at(1), QSL("Example 2"));
    QVERIFY(suggestions.urls.isEmpty());
}

void OpenSearchEngineTest::parseSuggestionsInvalidTest()
{
    QByteArray responses[] = {"invalid", R"json({"a":["x"]})json"};

    for (const auto &response : responses) {
        const auto suggestions = OpenSearchEngine::parseSuggestions(response);
        QVERIFY(suggestions.completions.isEmpty());
        QVERIFY(suggestions.urls.isEmpty());
    }
}

FALKONTEST_MAIN(OpenSearchEngineTest)
