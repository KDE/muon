/***************************************************************************
 *   Copyright © 2013 Aleix Pol Gonzalez <aleixpol@blue-systems.com>       *
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

#ifndef CINSTALLRESOURCE_H
#define CINSTALLRESOURCE_H

#include <resources/AbstractResource.h>

class CInstallBackend;
namespace CInstall {
    class AssetInfo;
    class AssetOperations;
}

struct BundleEntryInfo
{
    BundleEntryInfo()
    {}

    QString completeName; //name"-"version"-"release
    QString name;
    QString version;
    int release;
    qulonglong size;
    QString icon;
    QString comment;
    QString license;
    QUrl screenshotUrl;
    QUrl thumbnailUrl;
    QUrl homepage;
};

class CInstallResource : public AbstractResource
{
    Q_OBJECT
    public:
        explicit CInstallResource(const QString& bundlePath, CInstallBackend* parent);
        explicit CInstallResource(const BundleEntryInfo& info, AbstractResourcesBackend* parent);
        
        virtual QString section() { return "123"; }
        virtual QString installedVersion() const { return "only"; }
        virtual QString origin() const { return "fuuu"; }
        virtual QString categories() { return "cbundle"; }

        virtual QList<PackageState> addonsInformation() { return QList<PackageState>(); }
        virtual QString name() { return m_info.name; }
        virtual QString availableVersion() const { return m_info.version; }
        virtual QString comment() { return m_info.comment; }
        virtual QString longDescription() const { return m_info.comment; }
        virtual QString packageName() const { return m_info.completeName; }
        virtual QString license() { return m_info.license; }
        virtual int downloadSize() { return m_info.size; }
        virtual QUrl homepage() const { return m_info.homepage; }
        virtual QUrl screenshotUrl() { return m_info.screenshotUrl; }
        virtual QUrl thumbnailUrl() { return m_info.thumbnailUrl; }
        virtual void fetchChangelog();
        virtual State state();
        virtual QString icon() const;
        virtual bool canExecute() const { return true; }
        virtual void invokeApplication() const;

        void initFromPath(const QString& path);
        QString installedVersionCompleteName() const { return m_installedVersionCompleteName; }
        void updateData(const BundleEntryInfo& info);
        const BundleEntryInfo& bundleInfo() const { return m_info; }
        void removeBundle();

    private:
        void setState(State newState);

        BundleEntryInfo m_info;
        AbstractResource::State m_state;
        QString m_installedVersionCompleteName;
};

#endif // CINSTALLRESOURCE_H
