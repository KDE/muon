/***************************************************************************
 *   Copyright © 2011-2013 Jonathan Thomas <echidnaman@kubuntu.org>        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of        *
 *   the License or (at your option) version 3 or any later version        *
 *   accepted by the membership of KDE e.V. (or its successor approved     *
 *   by the membership of KDE e.V.), which shall act as a proxy            *
 *   defined in Section 14 of version 3 of the license.                    *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#include "OAuthSession.h"

// Qt includes
#include <QStringBuilder>
#include <QDebug>

// KDE includes
#include <KIO/Job>

// QJson includes
#include <qjson/serializer.h>

// OAuth includes
#include <QtOAuth/interface.h>

// Own includes
#include "OAuthPost.h"
#include "UbuntuLoginBackend.h"

OAuthSession *OAuthSession::s_self = nullptr;

OAuthSession *OAuthSession::global()
{
    if(!s_self)
        s_self = new OAuthSession;
    return s_self;
}

OAuthSession::OAuthSession(QObject *parent)
    : QObject(parent)
{
    Q_ASSERT(!s_self);
    s_self = this;

    m_loginBackend = new UbuntuLoginBackend(this);
    connect(m_loginBackend, SIGNAL(connectionStateChanged()), SIGNAL(loginStateChanged()));
    connect(m_loginBackend, SIGNAL(connectionStateChanged()), SLOT(refreshConsumerKeys()));
    m_oauthInterface = new QOAuth::Interface(this);
}

UbuntuLoginBackend *OAuthSession::loginBackend() const
{
    return m_loginBackend;
}

void OAuthSession::refreshConsumerKeys()
{
    if(m_loginBackend->hasCredentials()) {
        m_oauthInterface->setConsumerKey(m_loginBackend->consumerKey());
        m_oauthInterface->setConsumerSecret(m_loginBackend->consumerSecret());

        for (const OAuthPost &request : m_pendingRequests)
            postInformation(request);

        m_pendingRequests.clear();
    }
}

void OAuthSession::updateCredentials(const QMap<QString, QVariant> &creds)
{
    MapString credentials;
    for (auto it = creds.constBegin(); it != creds.constEnd(); ++it) {
        credentials[it.key()] = it.value().toString();
    }

    m_loginBackend->updateCredentials(credentials);
    qDebug() << "credentials updated!";
}

static QByteArray authorization(QOAuth::Interface* oauth, const KUrl& url, AbstractLoginBackend* login)
{
    return oauth->createParametersString(url.url(), QOAuth::POST, login->token(), login->tokenSecret(),
                                         QOAuth::HMAC_SHA1, QOAuth::ParamMap(), QOAuth::ParseForHeaderArguments);
}

KIO::StoredTransferJob *OAuthSession::postInformation(const OAuthPost &info)
{
    if(!m_loginBackend->hasCredentials()) {
        m_pendingRequests.append(info);
        m_loginBackend->login();
        return nullptr;
    }

    KUrl url(info.baseUrl(), info.path());
    url.setScheme("https");

    KIO::StoredTransferJob* job = KIO::storedHttpPost(QJson::Serializer().serialize(info.data()),
                                                      url, KIO::Overwrite | KIO::HideProgressInfo);
    job->addMetaData("content-type", "Content-Type: application/json");
    job->addMetaData("customHTTPHeader", "Authorization: " + authorization(m_oauthInterface, url, m_loginBackend));
    qDebug() << "job metadata:" << job->outgoingMetaData()["customHTTPHeader"];
    connect(job, SIGNAL(result(KJob*)), this, SLOT(informationPosted(KJob*)));
    job->start();

    return job;
}

KIO::StoredTransferJob *OAuthSession::getInformation(const OAuthPost &info)
{
    if(!m_loginBackend->hasCredentials()) {
        m_pendingRequests.append(info);
        m_loginBackend->login();
        return nullptr;
    }

    KUrl url(info.baseUrl(), info.path());
    url.setScheme("https");

    KIO::StoredTransferJob* job = KIO::storedGet(url, KIO::NoReload, KIO::Overwrite | KIO::HideProgressInfo);
    job->addMetaData("customHTTPHeader", "Authorization: " + authorization(m_oauthInterface, url, m_loginBackend));
    qDebug() << "job metadata:" << job->outgoingMetaData()["customHTTPHeader"];
    connect(job, SIGNAL(result(KJob*)), this, SLOT(informationPosted(KJob*)));
    job->start();

    return job;
}

void OAuthSession::informationPosted(KJob* j)
{
    KIO::StoredTransferJob* job = qobject_cast<KIO::StoredTransferJob*>(j);
    if(job->error())
        qDebug() << "error..." << job->error() << job->errorString() << job->errorText();
}
