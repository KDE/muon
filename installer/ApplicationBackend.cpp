/***************************************************************************
 *   Copyright © 2010 Jonathan Thomas <echidnaman@kubuntu.org>             *
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

#include "ApplicationBackend.h"

#include <QtCore/QDir>
#include <QtCore/QStringList>

#include <KLocale>
#include <KMessageBox>
#include <KDebug>

#include <LibQApt/Backend>
#include <DebconfGui.h>

#include "Application.h"
#include "ReviewsBackend/ReviewsBackend.h"
#include "Transaction.h"

ApplicationBackend::ApplicationBackend(QObject *parent)
    : QObject(parent)
    , m_backend(0)
    , m_reviewsBackend(new ReviewsBackend(this))
    , m_currentTransaction(0)
{
    m_pkgBlacklist << "kdebase-runtime" << "kdepim-runtime" << "kdelibs5-plugins" << "kdelibs5-data";
}

ApplicationBackend::~ApplicationBackend()
{
}

void ApplicationBackend::setBackend(QApt::Backend *backend)
{
    m_backend = backend;
    m_backend->setUndoRedoCacheSize(1);
    init();

    connect(m_backend, SIGNAL(workerEvent(QApt::WorkerEvent)),
            this, SLOT(workerEvent(QApt::WorkerEvent)));
    connect(m_backend, SIGNAL(errorOccurred(QApt::ErrorCode, QVariantMap)),
            this, SLOT(errorOccurred(QApt::ErrorCode, QVariantMap)));

    emit appBackendReady();
}

void ApplicationBackend::init()
{
    m_reviewsBackend->setAptBackend(m_backend);

    QDir appDir("/usr/share/app-install/desktop/");
    QStringList fileList = appDir.entryList(QDir::Files);

    QList<Application *> tempList;
    foreach(const QString &fileName, fileList) {
        Application *app = new Application("/usr/share/app-install/desktop/" + fileName, m_backend);
        tempList << app;
    }

    foreach (QApt::Package *package, m_backend->availablePackages()) {
        Application *app = new Application(package, m_backend);
        tempList << app;
    }

    foreach (Application *app, tempList) {
        if (app->isValid()) {
            QApt::Package *pkg = app->package();
            if ((pkg) && !m_pkgBlacklist.contains(pkg->latin1Name())) {
                m_appList << app;
                if (pkg->isInstalled()) {
                    m_instOriginList << pkg->origin();
                    m_originList << pkg->origin();
                } else {
                    m_originList << pkg->origin();
                }
            } else {
                delete app;
            }
        } else {
            // Invalid .desktop file
            delete app;
        }
    }

    if (m_originList.contains(QLatin1String(""))) {
        m_originList.remove(QLatin1String(""));
    }

    if (m_instOriginList.contains(QLatin1String(""))) {
        m_instOriginList.remove(QLatin1String(""));
    }
}

void ApplicationBackend::reload()
{
    emit reloadStarted();
    qDeleteAll(m_appList);
    m_appList.clear();
    qDeleteAll(m_queue);
    m_queue.clear();
    m_backend->reloadCache();
    m_reviewsBackend->clearReviewCache();

    init();

    emit reloadFinished();
}

void ApplicationBackend::workerEvent(QApt::WorkerEvent event)
{
    m_workerState.first = event;

    if (event == QApt::XapianUpdateFinished) {
        emit xapianReloaded();
    }

    if (!m_queue.isEmpty()) {
        m_workerState.second = m_currentTransaction;
    } else {
        return;
    }

    emit workerEvent(event, m_currentTransaction);

    if (!m_currentTransaction) {
        return;
    }

    switch (event) {
    case QApt::PackageDownloadStarted:
        m_currentTransaction->setState(RunningState);
        connect(m_backend, SIGNAL(downloadProgress(int, int, int)),
                this, SLOT(updateDownloadProgress(int)));
        break;
    case QApt::PackageDownloadFinished:
        disconnect(m_backend, SIGNAL(downloadProgress(int, int, int)),
                   this, SLOT(updateDownloadProgress(int)));
        break;
    case QApt::CommitChangesStarted:
        m_debconfGui = new DebconfKde::DebconfGui("/tmp/qapt-sock");
        m_currentTransaction->setState(RunningState);
        connect(m_backend, SIGNAL(commitProgress(QString, int)),
                this, SLOT(updateCommitProgress(QString, int)));
        m_debconfGui->connect(m_debconfGui, SIGNAL(activated()), m_debconfGui, SLOT(show()));
        m_debconfGui->connect(m_debconfGui, SIGNAL(deactivated()), m_debconfGui, SLOT(hide()));
        break;
    case QApt::CommitChangesFinished:
        disconnect(m_backend, SIGNAL(commitProgress(QString, int)),
                   this, SLOT(updateCommitProgress(QString, int)));

        m_currentTransaction->setState(DoneState);

        m_queue.dequeue();
        if (m_currentTransaction->action() == InstallApp) {
            m_appLaunchList << m_currentTransaction->application()->package()->name();
        }

        m_workerState.first = QApt::InvalidEvent;
        m_workerState.second = 0;

        if (m_queue.isEmpty()) {
            reload();
        } else {
            runNextTransaction();
        }
        break;
    default:
        break;
    }
}

void ApplicationBackend::errorOccurred(QApt::ErrorCode error, const QVariantMap &details)
{
    Q_UNUSED(details);

    if (m_queue.isEmpty()) {
        emit errorSignal(error, details);
        return;
    }

    disconnect(m_backend, SIGNAL(downloadProgress(int, int, int)),
                   this, SLOT(updateDownloadProgress(int)));
    disconnect(m_backend, SIGNAL(commitProgress(QString, int)),
                   this, SLOT(updateCommitProgress(QString, int)));

    // Undo marking if an AuthError is encountered, since our install/remove
    // buttons do both marking and committing
    switch (error) {
    case QApt::AuthError:
        cancelTransaction(m_currentTransaction->application());
        m_backend->undo();
        break;
    case QApt::UserCancelError:
        // Handled in transactionCancelled()
        return;
    default:
        break;
    }

    // Reset worker state on failure
    if (m_workerState.second) {
        m_workerState.first = QApt::InvalidEvent;
        m_workerState.second = 0;
    }

    // A CommitChangesFinished signal will still be fired in this case,
    // and workerEvent will take care of queue management for us
    emit errorSignal(error, details);
}

void ApplicationBackend::updateDownloadProgress(int percentage)
{
    emit progress(m_currentTransaction, percentage);
}

void ApplicationBackend::updateCommitProgress(const QString &text, int percentage)
{
    Q_UNUSED(text);

    emit progress(m_currentTransaction, percentage);
}

void ApplicationBackend::addTransaction(Transaction *transaction)
{
    transaction->setState(QueuedState);
    m_queue.enqueue(transaction);

    if (m_queue.count() == 1) {
        m_currentTransaction = m_queue.head();
        runNextTransaction();
    }
}

void ApplicationBackend::cancelTransaction(Application *app)
{
    disconnect(m_backend, SIGNAL(downloadProgress(int, int, int)),
               this, SLOT(updateDownloadProgress(int)));
    disconnect(m_backend, SIGNAL(commitProgress(QString, int)),
               this, SLOT(updateCommitProgress(QString, int)));
    QQueue<Transaction *>::iterator iter = m_queue.begin();

    while (iter != m_queue.end()) {
        if ((*iter)->application() == app) {
            if ((*iter)->state() == RunningState) {
                m_backend->cancelDownload();
                m_backend->undo();
            }

            m_queue.erase(iter);
            break;
        }
        ++iter;
    }

    emit transactionCancelled(app);
}

void ApplicationBackend::runNextTransaction()
{
    m_currentTransaction = m_queue.head();
    if (!m_currentTransaction) {
        return;
    }
    QApt::CacheState oldCacheState = m_backend->currentCacheState();
    m_backend->saveCacheState();

    Application *app = m_currentTransaction->application();

    switch (m_currentTransaction->action()) {
    case InstallApp:
        app->package()->setInstall();
        break;
    case RemoveApp:
        app->package()->setRemove();
        break;
    default:
        break;
    }

    QHash<QApt::Package *, QApt::Package::State> addons = m_currentTransaction->addons();
    QHash<QApt::Package *, QApt::Package::State>::const_iterator iter = addons.constBegin();

    while (iter != addons.constEnd()) {
        switch (iter.value()) {
        case QApt::Package::ToInstall:
            iter.key()->setInstall();
            break;
        case QApt::Package::ToRemove:
            iter.key()->setRemove();
            break;
        default:
            break;
        }
        ++iter;
    }

    if (app->package()->wouldBreak()) {
        m_backend->restoreCacheState(oldCacheState);
        //TODO Notify of error
    }

    m_backend->commitChanges();
}

QStringList ApplicationBackend::launchList() const
{
    return m_appLaunchList;
}

void ApplicationBackend::clearLaunchList()
{
    m_appLaunchList.clear();
}

ReviewsBackend *ApplicationBackend::reviewsBackend() const
{
    return m_reviewsBackend;
}

QList<Application *> ApplicationBackend::applicationList() const
{
    return m_appList;
}

QSet<QString> ApplicationBackend::appOrigins() const
{
    return m_originList;
}

QSet<QString> ApplicationBackend::installedAppOrigins() const
{
    return m_instOriginList;
}

QPair<QApt::WorkerEvent, Transaction *> ApplicationBackend::workerState() const
{
    return m_workerState;
}

QList<Transaction *> ApplicationBackend::transactions() const
{
    return m_queue;
}
