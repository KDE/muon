/***************************************************************************
 *   Copyright © 2012 Jonathan Thomas <echidnaman@kubuntu.org>             *
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

#ifndef USCRESOURCE_H
#define USCRESOURCE_H

#include <QtCore/QVariantMap>

#include "QAptResource.h"

class USCResource : public QAptResource
{
    Q_OBJECT
public:
    explicit USCResource(ApplicationBackend *parent,
                         QApt::Backend *backend, const QVariantMap &data);

    QApt::Package *package();

    QString name();
    QString icon() const;
    QString comment();
    QString packageName() const;
    AbstractResource::State state();
    QString categories();
    QUrl homepage() const;
    QUrl thumbnailUrl();
    QUrl screenshotUrl();
    QString license();
    QString longDescription() const;
    QString sizeDescription() { return QString(); }
    QString availableVersion() const;
    QString origin() const;
    QString archiveId() const;
    int downloadSize();
    QString section() { return QString(); }
    QString price() const;
    void fetchScreenshots();

private:
    QString m_name;
    QString m_comment;
    QString m_longDesc;
    QString m_packageName;
    QString m_categories;
    QString m_homepage;
    QUrl m_screenshotUrl;
    QList<QUrl> m_screenshotUrls;
    QString m_license;
    QString m_version;
    QString m_origin;
    QString m_price;
    QString m_archiveId;
};

#endif // USCRESOURCE_H
