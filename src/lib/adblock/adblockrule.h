/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2017 David Rosca <nowrep@gmail.com>
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
/**
 * Copyright (c) 2009, Benjamin C. Meyer <ben@meyerhome.net>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Benjamin Meyer nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef ADBLOCKRULE_H
#define ADBLOCKRULE_H

#include <QObject>
#include <QStringList>
#include <QStringMatcher>
#include <QRegularExpression>

#include "qzcommon.h"

class QUrl;
class QWebEngineUrlRequestInfo;

class AdBlockSubscription;

class FALKON_EXPORT AdBlockRule
{
    Q_DISABLE_COPY(AdBlockRule)

public:
    AdBlockRule(const QString &filter = QString(), AdBlockSubscription* subscription = nullptr);
    ~AdBlockRule();

    AdBlockRule* copy() const;

    AdBlockSubscription* subscription() const;
    void setSubscription(AdBlockSubscription* subscription);

    QString filter() const;
    void setFilter(const QString &filter);

    bool isCssRule() const;
    QString cssSelector() const;

    bool isUnsupportedRule() const;

    bool isDocument() const;
    bool isElemhide() const;
    bool isGenerichide() const;

    bool isDomainRestricted() const;
    bool isException() const;

    bool isComment() const;
    bool isEnabled() const;
    void setEnabled(bool enabled);

    bool isSlow() const;
    bool isInternalDisabled() const;

    bool urlMatch(const QUrl &url) const;
    bool networkMatch(const QWebEngineUrlRequestInfo &request, const QString &domain, const QString &encodedUrl) const;

    bool matchDomain(const QString &domain) const;
    bool matchThirdParty(const QWebEngineUrlRequestInfo &request) const;

    bool matchType(const QWebEngineUrlRequestInfo &request) const;

protected:
    bool stringMatch(const QString &domain, const QString &encodedUrl) const;
    bool isMatchingDomain(const QString &domain, const QString &filter) const;
    bool isMatchingRegExpStrings(const QString &url) const;
    QStringList parseRegExpFilter(const QString &filter) const;

private:
    enum RuleType {
        CssRule = 0,
        DomainMatchRule = 1,
        RegExpMatchRule = 2,
        StringEndsMatchRule = 3,
        StringContainsMatchRule = 4,
        MatchAllUrlsRule = 5,
        ExtendedCssRule = 6,
        SnippetRule = 7,
        Invalid = 8
    };

    enum RuleOption {
        NoOption                = 0,
        DomainRestrictedOption  = 1,
        ThirdPartyOption        = 1 << 1,

        ObjectOption            = 1 << 2,
        SubdocumentOption       = 1 << 3,
        XMLHttpRequestOption    = 1 << 4,
        ImageOption             = 1 << 5,
        ScriptOption            = 1 << 6,
        StyleSheetOption        = 1 << 7,
        ObjectSubrequestOption  = 1 << 8,
        PingOption              = 1 << 9,
        MediaOption             = 1 << 10,
        FontOption              = 1 << 11,
        OtherOption             = 1 << 12,
        TypeOptions = ObjectOption
                    | SubdocumentOption
                    | XMLHttpRequestOption
                    | ImageOption
                    | ScriptOption
                    | StyleSheetOption
                    | ObjectSubrequestOption
                    | PingOption
                    | MediaOption
                    | FontOption
                    | OtherOption,

        PopupOption             = 1 << 13,

        // Exception only options
        DocumentOption          = 1 << 20,
        ElementHideOption       = 1 << 21,
        GenericHideOption       = 1 << 22,
        GenericBlockOption      = 1 << 23,
    };

    Q_DECLARE_FLAGS(RuleOptions, RuleOption)

    inline bool hasOption(const RuleOption &opt) const;
    inline bool hasException(const RuleOption &opt) const;

    inline void setOption(const RuleOption &opt);
    inline void setException(const RuleOption &opt, bool on);

    void parseFilter();
    void parseDomains(const QString &domains, const QChar &separator);
    bool filterIsOnlyDomain(const QString &filter) const;
    bool filterIsOnlyEndsMatch(const QString &filter) const;
    QString createRegExpFromFilter(const QString &filter) const;
    QList<QStringMatcher> createStringMatchers(const QStringList &filters) const;

    AdBlockSubscription* m_subscription;

    RuleType m_type;
    RuleOptions m_options;
    RuleOptions m_exceptions;

    // Original rule filter
    QString m_filter;
    // Parsed rule for string matching (CSS Selector for CSS rules)
    QString m_matchString;
    // Case sensitivity for string matching
    Qt::CaseSensitivity m_caseSensitivity;

    bool m_isEnabled;
    bool m_isException;
    bool m_isInternalDisabled;

    QStringList m_allowedDomains;
    QStringList m_blockedDomains;

    struct RegExp {
        QRegularExpression regExp;
        QList<QStringMatcher> matchers;
    };

    // Use dynamic allocation to save memory
    RegExp* m_regExp;

    friend class AdBlockMatcher;
    friend class AdBlockSearchTree;
    friend class AdBlockSubscription;
};

#endif // ADBLOCKRULE_H

