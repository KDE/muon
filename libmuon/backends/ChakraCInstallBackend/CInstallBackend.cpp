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

#include <KAboutData>
#include <KPluginFactory>
#include <KIO/FileCopyJob>
#include <KIO/Job>
#include "CInstallBackend.h"
#include "CInstallResource.h"
#include <resources/StandardBackendUpdater.h>
#include <QFile>
#include <QDebug>
#include <qjson/parser.h>
#include <sys/utsname.h>

K_PLUGIN_FACTORY(MuonCInstallBackendFactory, registerPlugin<CInstallBackend>(); )
K_EXPORT_PLUGIN(MuonCInstallBackendFactory(KAboutData("muon-cinstallbackend","muon-cinstallbackend",ki18n("CInstall Backend"),"0.1",ki18n("Install CInstall data in your system"), KAboutData::License_GPL)))

namespace {
static QString getArchitecture() {
    struct utsname un;
    if (uname(&un) == -1) {
        qWarning() << "couldn't figure out the architecture";
    }
    return un.machine;
}
}

CInstallBackend::CInstallBackend(QObject* parent, const QVariantList& args)
    : AbstractResourcesBackend(parent)
    , m_currentArch(getArchitecture())
    , m_cacheDir(QDir::homePath() + "/.cinstall/cache")
    , m_repoDir(QDir::homePath() + "/.cinstall/repo")
    , m_iconsDir(QDir::homePath() + "/.cinstall/icons")
    , m_updater(new StandardBackendUpdater(this))
{
    KIO::FileCopyJob *bundleListJob = KIO::file_copy(KUrl("http://www.chakra-project.org/akabei/rpc.php?type=list_bundles&arg=" + m_currentArch)
                                                         , KUrl("/tmp/cinstRepo"), -1, KIO::Overwrite | KIO::HideProgressInfo);
    connect(bundleListJob, SIGNAL(finished(KJob*)), this, SLOT(addAvailableBundles(KJob*)));
}

CInstallBackend::~CInstallBackend()
{}

void CInstallBackend::addInstalledBundles()
{
    m_cacheDir.refresh();
    QStringList desktopFileNames = m_cacheDir.entryList();

    foreach (const QString &desktopFileName, desktopFileNames)
    {
        if (!desktopFileName.endsWith(".desktop"))
            continue;

        CInstallResource* res = new CInstallResource(desktopFileName, this);
        m_bundles.insert(res->name(), res);
    }
}

void CInstallBackend::addAvailableBundles(KJob* job)
{
    Q_ASSERT(m_bundles.isEmpty());
    addInstalledBundles();

    KIO::FileCopyJob *repoJob = qobject_cast<KIO::FileCopyJob*>(job);
    QList<BundleEntryInfo> availableBundlesList;
    QFile repoFile(repoJob->destUrl().pathOrUrl());

    if (!repoFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QJson::Parser parser;
    bool ok;

    QList<QVariant> result = parser.parse(repoFile.readAll(), &ok).toMap()["results"].toList();
    if (!ok) return;

    repoFile.close();

    foreach (const QVariant &propertys, result)
    {
        const QVariantMap propertyMap = propertys.toMap();
        BundleEntryInfo info;
        info.name = propertyMap["name"].toString();
        info.version = propertyMap["version"].toString();
        info.release = propertyMap["release"].toInt();
        info.size = propertyMap["size"].toULongLong();
        info.completeName = info.name + "-" + info.version + "-" + QString::number(info.release);

        // skip hidden (unfinished) files
        if (info.name.startsWith('.'))
            continue;

        QHash<QString, AbstractResource*>::const_iterator it = m_bundles.constFind(info.name);
        if(it!=m_bundles.constEnd()) {
            CInstallResource* res = qobject_cast<CInstallResource*>(*it);
            res->updateData(info);
        } else {
            CInstallResource* res = new CInstallResource(info, this);
            m_bundles.insert(res->name(), res);
        }
    }
    emit backendReady();
}

QString CInstallBackend::architecture() const
{
    return m_currentArch;
}

QVector<AbstractResource*> CInstallBackend::allResources() const
{
    return m_bundles.values().toVector();
}

AbstractBackendUpdater* CInstallBackend::backendUpdater() const
{
    return m_updater;
}

AbstractResource* CInstallBackend::resourceByPackageName(const QString& name) const
{
    return m_bundles.value(name);
}

AbstractReviewsBackend* CInstallBackend::reviewsBackend() const
{
    return 0;
}

int CInstallBackend::updatesCount() const
{
    return upgradeablePackages().count(); 
}

QList<AbstractResource*> CInstallBackend::upgradeablePackages() const
{
    QList<AbstractResource*> ret;
    foreach(AbstractResource* res, m_bundles) {
        if(res->state()==AbstractResource::Upgradeable)
            ret += res;
    }
    return ret;
}

QStringList CInstallBackend::searchPackageName(const QString& searchText)
{
    QStringList ret;
    foreach(AbstractResource* res, m_bundles) {
        QString name = res->name();
        if(name.contains(searchText, Qt::CaseInsensitive))
            ret += name;
    }
    return ret;
}

void CInstallBackend::cancelTransaction(AbstractResource* app)
{}

void CInstallBackend::installApplication(AbstractResource* app, AddonList addons)
{}

void CInstallBackend::removeApplication(AbstractResource* app)
{}
