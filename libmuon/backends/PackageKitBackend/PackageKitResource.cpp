/***************************************************************************
 *   Copyright © 2012 Aleix Pol Gonzalez <aleixpol@blue-systems.com>       *
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

#include "PackageKitResource.h"
#include <MuonDataSources.h>
#include <KGlobal>
#include <KLocale>
#include <PackageKit/packagekit-qt2/Transaction>

PackageKitResource::PackageKitResource(const PackageKit::Package& p, AbstractResourcesBackend* parent)
    : AbstractResource(parent)
    , m_package(p)
{
    setObjectName(p.id());
}

QString PackageKitResource::name()
{
    return m_package.name();
}

QString PackageKitResource::packageName() const
{
    return m_package.name();
}

QString PackageKitResource::comment()
{
    return m_package.description();
}

QString PackageKitResource::longDescription() const
{
    return m_package.changelog();
}

QUrl PackageKitResource::homepage() const
{
    return QUrl(m_package.url());
}

QString PackageKitResource::icon() const
{
    return m_package.iconPath();
}

QString PackageKitResource::license()
{
    fetchDetails();
    return m_package.license();
}

QList<PackageState> PackageKitResource::addonsInformation()
{
    return QList<PackageState>();
}

QString PackageKitResource::availableVersion() const
{
    return m_package.version();
}

QString PackageKitResource::installedVersion() const
{
    return m_package.version();
}

QString PackageKitResource::sizeDescription()
{
    return KGlobal::locale()->formatByteSize(m_package.size());
}

QString PackageKitResource::origin() const
{
    //FIXME
    return "PackageKit";
}

QString PackageKitResource::section()
{
    //FIXME
    return QString::number(m_package.group());
}

QUrl PackageKitResource::screenshotUrl()
{
    return KUrl(MuonDataSources::screenshotsSource(), "screenshot/"+packageName());
}

QUrl PackageKitResource::thumbnailUrl()
{
    return KUrl(MuonDataSources::screenshotsSource(), "thumbnail/"+packageName());
}

AbstractResource::State PackageKitResource::state()
{
    if(m_package.hasUpdateDetails())
        return Upgradeable;
    else {
        PackageKit::Package::Info info = m_package.info();
        if(info & PackageKit::Package::InfoInstalled) {
            return Installed;
        } else if(info & PackageKit::Package::InfoAvailable) {
            return None;
        }
    }
    return Broken;
}

void PackageKitResource::updatePackage(const PackageKit::Package& p)
{
    bool changeState = p.state()!=state();
    m_package = p;
    if(changeState) {
        emit stateChanged();
    }
}

QStringList PackageKitResource::categories()
{
    return QStringList();
}

bool PackageKitResource::isTechnical() const
{
    return true;
}

void PackageKitResource::fetchDetails()
{
    if(m_package.hasDetails())
        return;

    PackageKit::Transaction* transaction = new PackageKit::Transaction(this);
    transaction->getDetails(m_package);
    connect(transaction, SIGNAL(package(PackageKit::Package)), SLOT(updatePackage(PackageKit::Package)));
    connect(transaction, SIGNAL(finished(PackageKit::Transaction::Exit,uint)), SIGNAL(licenseChanged()));
    connect(transaction, SIGNAL(finished(PackageKit::Transaction::Exit,uint)), transaction, SLOT(deleteLater()));
}

PackageKit::Package PackageKitResource::package() const
{
    return m_package;
}