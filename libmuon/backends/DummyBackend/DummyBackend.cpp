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

#include "DummyBackend.h"
#include "DummyResource.h"
#include "DummyReviewsBackend.h"
#include <Transaction/Transaction.h>

#include <KAboutData>
#include <KLocalizedString>
#include <KPluginFactory>
#include <QDebug>

K_PLUGIN_FACTORY(MuonDummyBackendFactory, registerPlugin<DummyBackend>(); )
K_EXPORT_PLUGIN(MuonDummyBackendFactory(KAboutData("muon-dummybackend","muon-dummybackend",ki18n("Dummy Backend"),"0.1",ki18n("Dummy backend to test muon frontends"), KAboutData::License_GPL)))

DummyBackend::DummyBackend(QObject* parent, const QVariantList&)
    : AbstractResourcesBackend(parent)
    , m_updater(0)
{
    for(int i=0; i<32; i++) {
        QString name = "alalala"+QString::number(i);
        DummyResource* res = new DummyResource(name, this);
        res->setState(AbstractResource::State(1+(i%3)));
        m_resources.insert(name, res);
    }
    
    QMetaObject::invokeMethod(this, "backendReady", Qt::QueuedConnection);
    m_reviews = new DummyReviewsBackend(this);
}

QVector<AbstractResource*> DummyBackend::allResources() const
{
    QVector<AbstractResource*> ret;
    ret.reserve(m_resources.size());
    foreach(AbstractResource* res, m_resources) {
        ret += res;
    }
    return ret;
}

int DummyBackend::updatesCount() const
{
    return upgradeablePackages().count();
}

QList<AbstractResource*> DummyBackend::upgradeablePackages() const
{
    QList<AbstractResource*> updates;
    foreach(AbstractResource* res, m_resources) {
        if(res->state()==AbstractResource::Upgradeable)
            updates += res;
    }
    return updates;
}

AbstractResource* DummyBackend::resourceByPackageName(const QString& name) const
{
    return m_resources.value(name);
}

QStringList DummyBackend::searchPackageName(const QString& searchText)
{
    QStringList ret;
    foreach(AbstractResource* r, m_resources) {
        if(r->name().contains(searchText) || r->comment().contains(searchText))
            ret += r->packageName();
    }
    return ret;
}

AbstractBackendUpdater* DummyBackend::backendUpdater() const
{
    return m_updater;
}

AbstractReviewsBackend* DummyBackend::reviewsBackend() const
{
    return m_reviews;
}

QPair<TransactionStateTransition, Transaction*> DummyBackend::currentTransactionState() const
{
    return QPair<TransactionStateTransition, Transaction*>(FinishedCommitting, 0);
}

QList<Transaction*> DummyBackend::transactions() const
{
    return QList<Transaction*>();
}

void DummyBackend::installApplication(AbstractResource* app, const QHash< QString, bool >&)
{
    installApplication(app);
}

void DummyBackend::installApplication(AbstractResource* app)
{
    Transaction* t = new Transaction(app, InstallApp);
    emit transactionAdded(t);
    qobject_cast<DummyResource*>(app)->setState(AbstractResource::Installed);
    emit transactionRemoved(t);
    t->deleteLater();
}

void DummyBackend::removeApplication(AbstractResource* app)
{
    Transaction* t = new Transaction(app, RemoveApp);
    emit transactionAdded(t);
    qobject_cast<DummyResource*>(app)->setState(AbstractResource::None);
    emit transactionRemoved(t);
    t->deleteLater();
}

void DummyBackend::cancelTransaction(AbstractResource*)
{}