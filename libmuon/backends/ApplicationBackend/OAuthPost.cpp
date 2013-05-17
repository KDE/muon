/***************************************************************************
 *   Copyright © 2013 Jonathan Thomas <echidnaman@kubuntu.org>             *
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

#include "OAuthPost.h"
#include <QSharedData>

class OAuthPostData : public QSharedData
{
public:
    OAuthPostData(const KUrl &url, const QString &p,
                  const QVariantMap &postData)
        : baseUrl(url)
        , path(p)
        , data(postData) {}

    // Data
    KUrl baseUrl;
    QString path;
    QVariantMap data;
};

OAuthPost::OAuthPost(const KUrl &baseUrl, const QString &path,
                     const QVariantMap &d)
    : d(new OAuthPostData(baseUrl, path, d))
{
}

OAuthPost::OAuthPost(const OAuthPost &rhs)
    : d(rhs.d)
{
}

OAuthPost &OAuthPost::operator=(const OAuthPost &rhs)
{
    if (this != &rhs)
        d.operator=(rhs.d);
    return *this;
}

OAuthPost::~OAuthPost()
{
}

KUrl OAuthPost::baseUrl() const
{
    return d->baseUrl;
}

QString OAuthPost::path() const
{
    return d->path;
}

QVariantMap OAuthPost::data() const
{
    return d->data;
}
