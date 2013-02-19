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

#include "PackageResource.h"

// KDE includes
#include <KGlobal>
#include <KLocale>

// LibQApt includes
#include <LibQApt/Backend>
#include <LibQApt/Package>

PackageResource::PackageResource(ApplicationBackend *parent, QApt::Backend *backend, QApt::Package *package)
    : QAptResource(parent, backend, package)
{
    m_packageName = package->name().latin1();

    if (package->architecture() != m_backend->nativeArchitecture())
        m_packageName.append(QByteArray(":") + m_package->architecture().toLatin1());
}

QApt::Package *PackageResource::package()
{
    return m_package;
}

QString PackageResource::name()
{
    QString name;

    if (m_package) {
        name = m_package->name();

        if(m_package->isForeignArch())
            name = QString("%1 (%2)").arg(name, m_package->architecture());
    }

    return name;
}

QString PackageResource::comment()
{
    if (m_package)
        return m_package->shortDescription();

    return QString();
}

QString PackageResource::icon() const
{
    return QLatin1String("applications-other");
}

QString PackageResource::mimetypes() const
{
    // Data unavailable from package-type QApt resources
    return QString();
}

QString PackageResource::categories()
{
    // Data unavailable from package-type QApt resources
    return QString();
}

QString PackageResource::license()
{
    QString component = package()->component();
    if (component == "main" || component == "universe") {
        return i18nc("@info license", "Open Source");
    } else if (component == "restricted") {
        return i18nc("@info license", "Proprietary");
    } else {
        return i18nc("@info license", "Unknown");
    }
}

QUrl PackageResource::screenshotUrl()
{
    // FIXME, MuonDataSources integration
    return m_package->screenshotUrl(QApt::Screenshot);
}

QUrl PackageResource::thumbnailUrl()
{
    // FIXME, MuonDataSources integration
    return m_package->screenshotUrl(QApt::Thumbnail);
}

bool PackageResource::isTechnical() const
{
    return true;
}

QString PackageResource::packageName() const
{
    return m_packageName;
}

QUrl PackageResource::homepage() const
{
    if (m_package)
        m_package->homepage();

    return QString();
}

QString PackageResource::longDescription() const
{
    if (m_package)
        return m_package->longDescription();

    return QString();
}

QString PackageResource::installedVersion() const
{
    if (m_package)
        return m_package->installedVersion();

    return QString();
}

QString PackageResource::availableVersion() const
{
    if (m_package)
        return m_package->availableVersion();

    return QString();
}

QString PackageResource::sizeDescription()
{
    if (!isInstalled()) {
        return i18nc("@info app size", "%1 to download, %2 on disk",
                              KGlobal::locale()->formatByteSize(package()->downloadSize()),
                              KGlobal::locale()->formatByteSize(package()->availableInstalledSize()));
    } else {
        return i18nc("@info app size", "%1 on disk",
                              KGlobal::locale()->formatByteSize(package()->currentInstalledSize()));
    }
}

QString PackageResource::origin() const
{
    if (m_package)
        return m_package->origin();

    return QString();
}

int PackageResource::downloadSize()
{
    if (m_package)
        return m_package->downloadSize();

    return 0;
}

QString PackageResource::section()
{
    if (m_package)
        return m_package->section();

    return QString();
}

QStringList PackageResource::executables() const
{
    // Data unavailable from package-type QApt resources
    return QStringList();
}

void PackageResource::invokeApplication() const
{
    // Action unavailable from package-type QApt resources
}

bool PackageResource::canExecute() const
{
    // Action unavailable from package-type QApt resources
    return false;
}

AbstractResource::State PackageResource::state()
{
    State ret = None;

    if (m_package) {
        int s = package()->state();
        if (s & QApt::Package::Upgradeable)
            ret = Upgradeable;
        else if(s & QApt::Package::Installed)
            ret = Installed;
    }

    return ret;
}

void PackageResource::fetchScreenshots()
{
    // FIXME: Implement
}

void PackageResource::fetchChangelog()
{
    // FIXME: Implement
}
