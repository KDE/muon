/***************************************************************************
 *   Copyright © 2010-2013 Jonathan Thomas <echidnaman@kubuntu.org>        *
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

#ifndef PACKAGERESOURCE_H
#define PACKAGERESOURCE_H

#include "QAptResource.h"

namespace QApt {
    class Package;
}

class PackageResource : public QAptResource
{
    Q_OBJECT
public:
    explicit PackageResource(ApplicationBackend *parent, QApt::Backend *backend, QApt::Package *package);

    QApt::Package *package();

    QString name();
    QString comment();
    QString icon() const;
    QString mimetypes() const;
    QString categories();
    QString license();
    QUrl screenshotUrl();
    QUrl thumbnailUrl();
    bool isTechnical() const;
    QString packageName() const;
    QUrl homepage() const;
    QString longDescription() const;
    QString installedVersion() const;
    QString availableVersion() const;
    QString sizeDescription();
    QString origin() const;
    int downloadSize();
    QString section();

    QStringList executables() const;
    void invokeApplication() const;
    bool canExecute() const;

    void fetchScreenshots();

private:
    QString m_packageName;
    
signals:
    
public slots:
    
};

#endif // PACKAGERESOURCE_H
