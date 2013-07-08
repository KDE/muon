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

#include "FirefoxAppsBackend.h"
#include "FirefoxAppsResource.h"
#include <resources/StandardBackendUpdater.h>
#include <Transaction/Transaction.h>
#include <Transaction/TransactionModel.h>

#include <KAboutData>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KStandardDirs>
#include <QDebug>
#include <QThread>
#include <QTimer>
#include <QFile>
#include <QFileSystemWatcher>
#include <QFileInfo>
#include <QDir>
#include <QDesktopServices>

K_PLUGIN_FACTORY(MuonFirefoxAppsBackendFactory, registerPlugin<FirefoxAppsBackend>(); )
K_EXPORT_PLUGIN(MuonFirefoxAppsBackendFactory(KAboutData("muon-dummybackend","muon-dummybackend",ki18n("FirefoxApps Backend"),"0.1",ki18n("FirefoxApps backend to test muon frontends"), KAboutData::License_GPL)))

FirefoxAppsBackend::FirefoxAppsBackend(QObject* parent, const QVariantList&)
    : AbstractResourcesBackend(parent)
    , m_updater(new StandardBackendUpdater(this))
    , m_watcher(new QFileSystemWatcher(this))
{
    QStringList resources = KGlobal::dirs()->findAllResources("xdgdata-apps", "*.desktop");
    foreach(const QString& resource, resources) {
        if(resource.contains("/owa-http")) {
            m_resources.insert(resource, new FirefoxAppsResource(resource, this));
            m_watcher->addPath(QFileInfo(resource).dir().path());
        }
    }
    connect(m_watcher, SIGNAL(fileChanged(QString)), SLOT(fileChanged(QString)));
    connect(m_watcher, SIGNAL(directoryChanged(QString)), SLOT(directoryChanged(QString)));
}

void FirefoxAppsBackend::fileChanged(const QString& path)
{
    if(FirefoxAppsResource* res = m_resources.value(path)) {
        res->notifyStateChange();
    }
}

void FirefoxAppsBackend::directoryChanged(const QString& path)
{
    for(QHash<QString, FirefoxAppsResource*>::const_iterator it=m_resources.constBegin(), itEnd=m_resources.constEnd(); it!=itEnd; ++it) {
        if(it.key().startsWith(path))
            it.value()->notifyStateChange();
    }
}

QVector<AbstractResource*> FirefoxAppsBackend::allResources() const
{
    QVector<AbstractResource*> ret;
    ret.reserve(m_resources.size());
    foreach(AbstractResource* res, m_resources) {
        ret += res;
    }
    return ret;
}

int FirefoxAppsBackend::updatesCount() const
{
    return upgradeablePackages().count();
}

QList<AbstractResource*> FirefoxAppsBackend::upgradeablePackages() const
{
    QList<AbstractResource*> updates;
    foreach(AbstractResource* res, m_resources) {
        if(res->state()==AbstractResource::Upgradeable)
            updates += res;
    }
    return updates;
}

AbstractResource* FirefoxAppsBackend::resourceByPackageName(const QString& name) const
{
    return m_resources.value(name);
}

QList<AbstractResource*> FirefoxAppsBackend::searchPackageName(const QString& searchText)
{
    QList<AbstractResource*> ret;
    foreach(AbstractResource* r, m_resources) {
        if(r->name().contains(searchText, Qt::CaseInsensitive) || r->comment().contains(searchText, Qt::CaseInsensitive))
            ret += r;
    }
    return ret;
}

AbstractBackendUpdater* FirefoxAppsBackend::backendUpdater() const
{
    return m_updater;
}

AbstractReviewsBackend* FirefoxAppsBackend::reviewsBackend() const
{
    return nullptr;
}

void FirefoxAppsBackend::installApplication(AbstractResource* app, AddonList )
{
    installApplication(app);
}

void FirefoxAppsBackend::installApplication(AbstractResource* /*app*/)
{
    Q_ASSERT(false && "not possible");
// 	TransactionModel *transModel = TransactionModel::global();
// 	transModel->addTransaction(new FirefoxAppsTransaction(qobject_cast<FirefoxAppsResource*>(app), Transaction::InstallRole));
}

void FirefoxAppsBackend::removeApplication(AbstractResource* app)
{
    Transaction* t = new Transaction(this, app, Transaction::RemoveRole);
    TransactionModel *transModel = TransactionModel::global();
    transModel->addTransaction(t);
    FirefoxAppsResource* r = qobject_cast<FirefoxAppsResource*>(app);
    Q_ASSERT(r);
    r->remove();
    transModel->removeTransaction(t);
}

void FirefoxAppsBackend::cancelTransaction(AbstractResource*)
{}

