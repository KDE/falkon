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
 * Copyright (c) 2009, Zsombor Gegesy <gzsombor@gmail.com>
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

#include "adblockrule.h"
#include "adblocksubscription.h"
#include "qztools.h"

#include <QUrl>
#include <QString>
#include <QWebEnginePage>
#include <QWebEngineUrlRequestInfo>

static QString toSecondLevelDomain(const QUrl &url)
{
    const QString topLevelDomain = url.topLevelDomain();
    const QString urlHost = url.host();

    if (topLevelDomain.isEmpty() || urlHost.isEmpty()) {
        return {};
    }

    QString domain = urlHost.left(urlHost.size() - topLevelDomain.size());

    if (domain.count(QL1C('.')) == 0) {
        return urlHost;
    }

    while (domain.count(QL1C('.')) != 0) {
        domain = domain.mid(domain.indexOf(QL1C('.')) + 1);
    }

    return domain + topLevelDomain;
}

AdBlockRule::AdBlockRule(const QString &filter, AdBlockSubscription* subscription)
    : m_subscription(subscription)
    , m_type(StringContainsMatchRule)
    , m_caseSensitivity(Qt::CaseInsensitive)
    , m_isEnabled(true)
    , m_isException(false)
    , m_isInternalDisabled(false)
    , m_regExp(nullptr)
{
    setFilter(filter);
}

AdBlockRule::~AdBlockRule()
{
    delete m_regExp;
}

AdBlockRule* AdBlockRule::copy() const
{
    auto* rule = new AdBlockRule();
    rule->m_subscription = m_subscription;
    rule->m_type = m_type;
    rule->m_options = m_options;
    rule->m_exceptions = m_exceptions;
    rule->m_filter = m_filter;
    rule->m_matchString = m_matchString;
    rule->m_caseSensitivity = m_caseSensitivity;
    rule->m_isEnabled = m_isEnabled;
    rule->m_isException = m_isException;
    rule->m_isInternalDisabled = m_isInternalDisabled;
    rule->m_allowedDomains = m_allowedDomains;
    rule->m_blockedDomains = m_blockedDomains;

    if (m_regExp) {
        rule->m_regExp = new RegExp;
        rule->m_regExp->regExp = m_regExp->regExp;
        rule->m_regExp->matchers = m_regExp->matchers;
    }

    return rule;
}

AdBlockSubscription* AdBlockRule::subscription() const
{
    return m_subscription;
}

void AdBlockRule::setSubscription(AdBlockSubscription* subscription)
{
    m_subscription = subscription;
}

QString AdBlockRule::filter() const
{
    return m_filter;
}

void AdBlockRule::setFilter(const QString &filter)
{
    m_filter = filter;
    parseFilter();
}

bool AdBlockRule::isCssRule() const
{
    return m_type == CssRule;
}

QString AdBlockRule::cssSelector() const
{
    return m_matchString;
}

bool AdBlockRule::isUnsupportedRule() const
{
    return m_type == ExtendedCssRule || m_type == SnippetRule || m_isInternalDisabled;
}

bool AdBlockRule::isDocument() const
{
    return hasOption(DocumentOption);
}

bool AdBlockRule::isElemhide() const
{
    return hasOption(ElementHideOption);
}

bool AdBlockRule::isGenerichide() const
{
    return hasOption(GenericHideOption);
}

bool AdBlockRule::isDomainRestricted() const
{
    return hasOption(DomainRestrictedOption);
}

bool AdBlockRule::isException() const
{
    return m_isException;
}

bool AdBlockRule::isComment() const
{
    return m_filter.startsWith(QL1C('!'));
}

bool AdBlockRule::isEnabled() const
{
    return m_isEnabled;
}

void AdBlockRule::setEnabled(bool enabled)
{
    m_isEnabled = enabled;
}

bool AdBlockRule::isSlow() const
{
    return m_regExp != nullptr;
}

bool AdBlockRule::isInternalDisabled() const
{
    return m_isInternalDisabled;
}

bool AdBlockRule::urlMatch(const QUrl &url) const
{
    if (!hasOption(DocumentOption) && !hasOption(ElementHideOption) && !hasOption(GenericHideOption) && !hasOption(GenericBlockOption)) {
        return false;
    }

    const QString encodedUrl = url.toEncoded();
    const QString domain = url.host();

    return stringMatch(domain, encodedUrl);
}

bool AdBlockRule::networkMatch(const QWebEngineUrlRequestInfo &request, const QString &domain, const QString &encodedUrl) const
{
    if (m_type == CssRule || !m_isEnabled || m_isInternalDisabled) {
        return false;
    }

    bool matched = stringMatch(domain, encodedUrl);

    if (matched) {
        // Check domain restrictions
        if (hasOption(DomainRestrictedOption) && !matchDomain(request.firstPartyUrl().host())) {
            return false;
        }

        // Check third-party restriction
        if (hasOption(ThirdPartyOption) && !matchThirdParty(request)) {
            return false;
        }

        // Check type restrictions
        if (((m_exceptions | m_options) & TypeOptions) && !matchType(request))
            return false;
    }

    return matched;
}

bool AdBlockRule::matchDomain(const QString &domain) const
{
    if (!m_isEnabled) {
        return false;
    }

    if (!hasOption(DomainRestrictedOption)) {
        return true;
    }

    if (m_blockedDomains.isEmpty()) {
        for (const QString &d : qAsConst(m_allowedDomains)) {
            if (isMatchingDomain(domain, d)) {
                return true;
            }
        }
    }
    else if (m_allowedDomains.isEmpty()) {
        for (const QString &d : qAsConst(m_blockedDomains)) {
            if (isMatchingDomain(domain, d)) {
                return false;
            }
        }
        return true;
    }
    else {
        for (const QString &d : qAsConst(m_blockedDomains)) {
            if (isMatchingDomain(domain, d)) {
                return false;
            }
        }

        for (const QString &d : qAsConst(m_allowedDomains)) {
            if (isMatchingDomain(domain, d)) {
                return true;
            }
        }
    }

    return false;
}

bool AdBlockRule::matchThirdParty(const QWebEngineUrlRequestInfo &request) const
{
    // Third-party matching should be performed on second-level domains
    const QString firstPartyHost = toSecondLevelDomain(request.firstPartyUrl());
    const QString host = toSecondLevelDomain(request.requestUrl());

    bool match = firstPartyHost != host;

    return hasException(ThirdPartyOption) ? !match : match;
}

bool AdBlockRule::matchType(const QWebEngineUrlRequestInfo &request) const
{
    RuleOption type;
    switch (request.resourceType()) {
    case QWebEngineUrlRequestInfo::ResourceTypeMainFrame:
        type = DocumentOption;
        break;
    case QWebEngineUrlRequestInfo::ResourceTypeSubFrame:
        type = SubdocumentOption;
        break;
    case QWebEngineUrlRequestInfo::ResourceTypeStylesheet:
        type = StyleSheetOption;
        break;
    case QWebEngineUrlRequestInfo::ResourceTypeScript:
        type = ScriptOption;
        break;
    case QWebEngineUrlRequestInfo::ResourceTypeImage:
        type = ImageOption;
        break;
    case QWebEngineUrlRequestInfo::ResourceTypeFontResource:
        type = FontOption;
        break;
    case QWebEngineUrlRequestInfo::ResourceTypeObject:
        type = ObjectOption;
        break;
    case QWebEngineUrlRequestInfo::ResourceTypeMedia:
        type = MediaOption;
        break;
    case QWebEngineUrlRequestInfo::ResourceTypeXhr:
        type = XMLHttpRequestOption;
        break;
    case QWebEngineUrlRequestInfo::ResourceTypePing:
        type = PingOption;
        break;
    case QWebEngineUrlRequestInfo::ResourceTypePluginResource:
        type = ObjectSubrequestOption;
        break;
    case QWebEngineUrlRequestInfo::ResourceTypeSubResource:
    case QWebEngineUrlRequestInfo::ResourceTypeWorker:
    case QWebEngineUrlRequestInfo::ResourceTypeSharedWorker:
    case QWebEngineUrlRequestInfo::ResourceTypePrefetch:
    case QWebEngineUrlRequestInfo::ResourceTypeFavicon:
    case QWebEngineUrlRequestInfo::ResourceTypeServiceWorker:
    case QWebEngineUrlRequestInfo::ResourceTypeCspReport:
    case QWebEngineUrlRequestInfo::ResourceTypeNavigationPreloadMainFrame:
    case QWebEngineUrlRequestInfo::ResourceTypeNavigationPreloadSubFrame:
    case QWebEngineUrlRequestInfo::ResourceTypeUnknown:
    default:
        type = OtherOption;
        break;
    }
    if (!m_exceptions)
        return m_options.testFlag(type);
    return !m_exceptions.testFlag(type);
}

void AdBlockRule::parseFilter()
{
    QString parsedLine = m_filter;

    // Empty rule or just comment
    if (m_filter.trimmed().isEmpty() || m_filter.startsWith(QL1C('!'))) {
        // We want to differentiate rule disabled by user and rule disabled in subscription file
        // m_isInternalDisabled is also used when rule is disabled due to all options not being supported
        m_isEnabled = false;
        m_isInternalDisabled = true;
        m_type = Invalid;
        return;
    }

    // Exception always starts with @@
    if (parsedLine.startsWith(QL1S("@@"))) {
        m_isException = true;
        parsedLine.remove(0, 2);
    }

    // Extended CSS element hiding
    if (parsedLine.contains(QL1S("#?#"))) {
        m_type = ExtendedCssRule;
        int pos = parsedLine.indexOf(QL1C('#'));
        if (!parsedLine.startsWith(QL1S("#"))) {
            QString domains = parsedLine.left(pos);
            parseDomains(domains, QL1C(','));
        }
        m_matchString = parsedLine.mid(pos + 3);
        // CSS rule cannot have more options -> stop parsing
        return;
    }

    // Snippet rule
    if (parsedLine.contains(QL1S("#$#"))) {
        m_type = SnippetRule;
        int pos = parsedLine.indexOf(QL1C('#'));
        if (!parsedLine.startsWith(QL1S("#"))) {
            QString domains = parsedLine.left(pos);
            parseDomains(domains, QL1C(','));
        }
        m_matchString = parsedLine.mid(pos + 3);
        return;
    }

    // CSS Element hiding rule
    if (parsedLine.contains(QL1S("##")) || parsedLine.contains(QL1S("#@#"))) {
        m_type = CssRule;
        int pos = parsedLine.indexOf(QL1C('#'));

        // Domain restricted rule
        if (!parsedLine.startsWith(QL1S("#"))) {
            QString domains = parsedLine.left(pos);
            parseDomains(domains, QL1C(','));
        }

        m_isException = parsedLine.at(pos + 1) == QL1C('@');
        m_matchString = parsedLine.mid(m_isException ? pos + 3 : pos + 2);

        // CSS rule cannot have more options -> stop parsing
        return;
    }

    // Parse all options following $ char
    int optionsIndex = parsedLine.indexOf(QL1C('$'));
    if (optionsIndex >= 0) {
        const QStringList options = parsedLine.mid(optionsIndex + 1).split(QL1C(','), Qt::SkipEmptyParts);

        int handledOptions = 0;
        for (const QString &option : options) {
            if (option.startsWith(QL1S("domain="))) {
                parseDomains(option.mid(7), QL1C('|'));
                ++handledOptions;
            }
            else if (option == QL1S("match-case")) {
                m_caseSensitivity = Qt::CaseSensitive;
                ++handledOptions;
            }
            else if (option.endsWith(QL1S("third-party"))) {
                setOption(ThirdPartyOption);
                setException(ThirdPartyOption, option.startsWith(QL1C('~')));
                ++handledOptions;
            }
            else if (option.endsWith(QL1S("object"))) {
                setOption(ObjectOption);
                setException(ObjectOption, option.startsWith(QL1C('~')));
                ++handledOptions;
            }
            else if (option.endsWith(QL1S("subdocument"))) {
                setOption(SubdocumentOption);
                setException(SubdocumentOption, option.startsWith(QL1C('~')));
                ++handledOptions;
            }
            else if (option.endsWith(QL1S("xmlhttprequest"))) {
                setOption(XMLHttpRequestOption);
                setException(XMLHttpRequestOption, option.startsWith(QL1C('~')));
                ++handledOptions;
            }
            else if (option.endsWith(QL1S("image"))) {
                setOption(ImageOption);
                setException(ImageOption, option.startsWith(QL1C('~')));
                ++handledOptions;
            }
            else if (option.endsWith(QL1S("script"))) {
                setOption(ScriptOption);
                setException(ScriptOption, option.startsWith(QL1C('~')));
                ++handledOptions;
            }
            else if (option.endsWith(QL1S("stylesheet"))) {
                setOption(StyleSheetOption);
                setException(StyleSheetOption, option.startsWith(QL1C('~')));
                ++handledOptions;
            }
            else if (option.endsWith(QL1S("object-subrequest"))) {
                setOption(ObjectSubrequestOption);
                setException(ObjectSubrequestOption, option.startsWith(QL1C('~')));
                ++handledOptions;
            }
            else if (option.endsWith(QL1S("ping"))) {
                setOption(PingOption);
                setException(PingOption, option.startsWith(QL1C('~')));
                ++handledOptions;
            }
            else if (option.endsWith(QL1S("media"))) {
                setOption(MediaOption);
                setException(MediaOption, option.startsWith(QL1C('~')));
                ++handledOptions;
            }
            else if (option.endsWith(QL1S("font"))) {
                setOption(FontOption);
                setException(FontOption, option.startsWith(QL1C('~')));
                ++handledOptions;
            }
            else if (option.endsWith(QL1S("other"))) {
                setOption(OtherOption);
                setException(OtherOption, option.startsWith(QL1C('~')));
                ++handledOptions;
            }
            else if (option == QL1S("collapse")) {
                // Hiding placeholders of blocked elements is enabled by default
                ++handledOptions;
            }
            else if (option == QL1S("popup")) {
                // doesn't do anything yet
                setOption(PopupOption);
                ++handledOptions;
            }
            else if (option == QL1S("document") && m_isException) {
                setOption(DocumentOption);
                ++handledOptions;
            }
            else if (option == QL1S("elemhide") && m_isException) {
                setOption(ElementHideOption);
                ++handledOptions;
            }
            else if (option == QL1S("generichide") && m_isException) {
                setOption(GenericHideOption);
                ++handledOptions;
            }
            else if (option == QL1S("genericblock") && m_isException) {
                // doesn't do anything yet
                setOption(GenericBlockOption);
//                 ++handledOptions;
            }
        }

        // If we don't handle all options, it's safer to just disable this rule
        if (handledOptions != options.count()) {
            m_isInternalDisabled = true;
            m_type = Invalid;
            return;
        }

        parsedLine.truncate(optionsIndex);
    }

    // Rule is classic regexp
    if (parsedLine.startsWith(QL1C('/')) && parsedLine.endsWith(QL1C('/'))) {
        parsedLine.remove(0, 1);
        parsedLine = parsedLine.left(parsedLine.size() - 1);

        m_type = RegExpMatchRule;
        m_regExp = new RegExp;
        m_regExp->regExp = QRegularExpression(parsedLine, QRegularExpression::InvertedGreedinessOption);
        if (m_caseSensitivity == Qt::CaseInsensitive) {
            m_regExp->regExp.setPatternOptions(m_regExp->regExp.patternOptions() | QRegularExpression::CaseInsensitiveOption);
        }
        m_regExp->matchers = createStringMatchers(parseRegExpFilter(parsedLine));
        return;
    }

    // Remove starting and ending wildcards (*)
    if (parsedLine.startsWith(QL1C('*'))) {
        parsedLine.remove(0, 1);
    }

    if (parsedLine.endsWith(QL1C('*'))) {
        parsedLine = parsedLine.left(parsedLine.size() - 1);
    }

    // We can use fast string matching for domain here
    if (filterIsOnlyDomain(parsedLine)) {
        parsedLine.remove(0, 2);
        parsedLine = parsedLine.left(parsedLine.size() - 1);

        m_type = DomainMatchRule;
        m_matchString = parsedLine;
        return;
    }

    // If rule contains only | at end, we can also use string matching
    if (filterIsOnlyEndsMatch(parsedLine)) {
        parsedLine = parsedLine.left(parsedLine.size() - 1);

        m_type = StringEndsMatchRule;
        m_matchString = parsedLine;
        return;
    }

    // If we still find a wildcard (*) or separator (^) or (|)
    // we must modify parsedLine to comply with QRegularExpression
    if (parsedLine.contains(QL1C('*')) ||
        parsedLine.contains(QL1C('^')) ||
        parsedLine.contains(QL1C('|'))
       ) {
        m_type = RegExpMatchRule;
        m_regExp = new RegExp;
        m_regExp->regExp = QRegularExpression(createRegExpFromFilter(parsedLine), QRegularExpression::InvertedGreedinessOption);
        if (m_caseSensitivity == Qt::CaseInsensitive) {
            m_regExp->regExp.setPatternOptions(m_regExp->regExp.patternOptions() | QRegularExpression::CaseInsensitiveOption);
        }
        m_regExp->matchers = createStringMatchers(parseRegExpFilter(parsedLine));
        return;
    }

    // This rule matches all urls
    if (parsedLine.isEmpty()) {
        if (m_options == NoOption) {
            qWarning() << "Disabling unrestricted rule that would block all requests" << m_filter;
            m_isInternalDisabled = true;
            m_type = Invalid;
            return;
        }
        m_type = MatchAllUrlsRule;
        return;
    }

    // We haven't found anything that needs use of regexp, yay!
    m_type = StringContainsMatchRule;
    m_matchString = parsedLine;
}

void AdBlockRule::parseDomains(const QString &domains, const QChar &separator)
{
    const QStringList domainsList = domains.split(separator, Qt::SkipEmptyParts);

    for (const QString &domain : domainsList) {
        if (domain.isEmpty()) {
            continue;
        }
        if (domain.startsWith(QL1C('~'))) {
            m_blockedDomains.append(domain.mid(1));
        }
        else {
            m_allowedDomains.append(domain);
        }
    }

    if (!m_blockedDomains.isEmpty() || !m_allowedDomains.isEmpty()) {
        setOption(DomainRestrictedOption);
    }
}

bool AdBlockRule::filterIsOnlyDomain(const QString &filter) const
{
    if (!filter.endsWith(QL1C('^')) || !filter.startsWith(QL1S("||")))
        return false;

    for (int i = 0; i < filter.size(); ++i) {
        switch (filter.at(i).toLatin1()) {
        case '/':
        case ':':
        case '?':
        case '=':
        case '&':
        case '*':
            return false;
        default:
            break;
        }
    }

    return true;
}

bool AdBlockRule::filterIsOnlyEndsMatch(const QString &filter) const
{
    for (int i = 0; i < filter.size(); ++i) {
        switch (filter.at(i).toLatin1()) {
        case '^':
        case '*':
            return false;
        case '|':
            return i == filter.size() - 1;
        default:
            break;
        }
    }

    return false;
}

static bool wordCharacter(const QChar &c)
{
    return c.isLetterOrNumber() || c.isMark() || c == QL1C('_');
}

QString AdBlockRule::createRegExpFromFilter(const QString &filter) const
{
    QString parsed;
    parsed.reserve(filter.size());

    bool hadWildcard = false; // Filter multiple wildcards

    for (int i = 0; i < filter.size(); ++i) {
        const QChar c = filter.at(i);
        switch (c.toLatin1()) {
        case '^':
            parsed.append(QL1S("(?:[^\\w\\d\\-.%]|$)"));
            break;

        case '*':
            if (!hadWildcard)
                parsed.append(QL1S(".*"));
            break;

        case '|':
            if (i == 0) {
                if (filter.size() > 1 && filter.at(1) == QL1C('|')) {
                    parsed.append(QL1S("^[\\w\\-]+:\\/+(?!\\/)(?:[^\\/]+\\.)?"));
                    i++;
                }
                else {
                    parsed.append(QL1C('^'));
                }
                break;
            }
            else if (i == filter.size() - 1) {
                parsed.append(QL1C('$'));
                break;
            }
            // fallthrough

        default:
            if (!wordCharacter(c))
                parsed.append(QL1C('\\') + c);
            else
                parsed.append(c);
        }

        hadWildcard = c == QL1C('*');
    }

    return parsed;
}

QList<QStringMatcher> AdBlockRule::createStringMatchers(const QStringList &filters) const
{
    QList<QStringMatcher> matchers;
    matchers.reserve(filters.size());

    for (const QString &filter : filters) {
        matchers.append(QStringMatcher(filter, m_caseSensitivity));
    }

    return matchers;
}

bool AdBlockRule::stringMatch(const QString &domain, const QString &encodedUrl) const
{
    switch (m_type) {
    case StringContainsMatchRule:
        return encodedUrl.contains(m_matchString, m_caseSensitivity);

    case DomainMatchRule:
        return isMatchingDomain(domain, m_matchString);

    case StringEndsMatchRule:
        return encodedUrl.endsWith(m_matchString, m_caseSensitivity);

    case RegExpMatchRule:
        if (!isMatchingRegExpStrings(encodedUrl)) {
            return false;
        }
        return m_regExp->regExp.match(encodedUrl).hasMatch();

    case MatchAllUrlsRule:
        return true;

    default:
        return false;
    }
}

bool AdBlockRule::isMatchingDomain(const QString &domain, const QString &filter) const
{
    return QzTools::matchDomain(filter, domain);
}

bool AdBlockRule::isMatchingRegExpStrings(const QString &url) const
{
    Q_ASSERT(m_regExp);

    const auto matchers = m_regExp->matchers;
    for (const QStringMatcher &matcher : matchers) {
        if (matcher.indexIn(url) == -1)
            return false;
    }

    return true;
}

// Split regexp filter into strings that can be used with QString::contains
// Don't use parts that contains only 1 char and duplicated parts
QStringList AdBlockRule::parseRegExpFilter(const QString &filter) const
{
    QStringList list;
    int startPos = -1;

    for (int i = 0; i < filter.size(); ++i) {
        const QChar c = filter.at(i);
        // Meta characters in AdBlock rules are | * ^
        if (c == QL1C('|') || c == QL1C('*') || c == QL1C('^')) {
            const QString sub = filter.mid(startPos, i - startPos);
            if (sub.size() > 1)
                list.append(sub);
            startPos = i + 1;
        }
    }

    const QString sub = filter.mid(startPos);
    if (sub.size() > 1)
        list.append(sub);

    list.removeDuplicates();

    return list;
}

bool AdBlockRule::hasOption(const AdBlockRule::RuleOption &opt) const
{
    return (m_options & opt);
}

bool AdBlockRule::hasException(const AdBlockRule::RuleOption &opt) const
{
    return (m_exceptions & opt);
}

void AdBlockRule::setOption(const AdBlockRule::RuleOption &opt)
{
    m_options |= opt;
}

void AdBlockRule::setException(const AdBlockRule::RuleOption &opt, bool on)
{
    if (on) {
        m_exceptions |= opt;
    }
}
