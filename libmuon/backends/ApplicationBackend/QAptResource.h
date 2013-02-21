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

#ifndef QAPTRESOURCE_H
#define QAPTRESOURCE_H

#include <LibQApt/Globals>

// Libmuon includes
#include "resources/AbstractResource.h"

class KJob;

namespace QApt {
    class Backend;
    class Package;
}

class ApplicationBackend;

class QAptResource : public AbstractResource
{
    Q_OBJECT
public:
    explicit QAptResource(ApplicationBackend *parent, QApt::Backend *backend, QApt::Package *package = nullptr);

    virtual QApt::Package *package() = 0;
    bool isValid() const;
    void clearPackage();
    QString installedVersion() const;
    State state();
    QList<PackageState> addonsInformation();

    void fetchChangelog();

protected:
    QApt::Backend *m_backend;
    QApt::Package *m_package;
    bool m_isValid;

private:
    QApt::PackageList addons();
    QString buildDescription(const QByteArray& data, const QString& source);

private slots:
    void processChangelog(KJob *job);
};

#endif // QAPTRESOURCE_H
