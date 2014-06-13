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

#ifndef APPLICATIONBACKEND_H
#define APPLICATIONBACKEND_H

#include <QFutureWatcher>
#include <QtCore/QObject>
#include <QtCore/QQueue>
#include <QtCore/QSet>
#include <QtCore/QStringList>
#include <QtCore/QVector>

#include <LibQApt/Package>
#include <LibQApt/Backend>

#include <libmuonprivate_export.h>
#include "resources/AbstractResourcesBackend.h"

namespace QApt {
    class Backend;
    class Transaction;
}
namespace DebconfKde {
    class DebconfGui;
}

class Application;
class ApplicationUpdates;
class ReviewsBackend;
class Transaction;
class QAptActions;
class KJob;

class MUONPRIVATE_EXPORT ApplicationBackend : public AbstractResourcesBackend
{
    Q_OBJECT
    Q_INTERFACES(AbstractResourcesBackend)
    Q_PROPERTY(QObject* backend READ backend)
public:
    explicit ApplicationBackend(QObject *parent, const QVariantList& args);
    ~ApplicationBackend();

    bool isValid() const;
    AbstractReviewsBackend *reviewsBackend() const;
    Q_SCRIPTABLE AbstractResource* resourceByPackageName(const QString& name) const;
    QApt::Backend* backend() const;

    int updatesCount() const;

    bool confirmRemoval(QApt::StateChanges changes);
    bool isFetching() const;
    void markTransaction(Transaction *transaction);
    void markLangpacks(Transaction *transaction);
    
    QVector< AbstractResource* > allResources() const;
    QList<AbstractResource*> searchPackageName(const QString& searchText);
    
    void installApplication(AbstractResource *app, AddonList addons);
    void installApplication(AbstractResource *app);
    void removeApplication(AbstractResource *app);
    void cancelTransaction(AbstractResource *app);
    
    AbstractBackendUpdater* backendUpdater() const;
    void integrateMainWindow(MuonMainWindow* w);
    QWidget* mainWindow() const;
    virtual QList<AbstractResource*> upgradeablePackages() const;
    void aptListBugs(QStringList packageName);
    
private:
    void setFetching(bool f);
    
    QApt::Backend *m_backend;
    ReviewsBackend *m_reviewsBackend;
    bool m_isFetching;

    QFutureWatcher<QVector<Application*> >* m_watcher;
    QVector<Application *> m_appList;

    // Transactions
    QHash<Transaction *, QApt::Transaction *> m_transQueue;
    Transaction *m_wantedTransaction;
    Transaction *m_currentTransaction;

    DebconfKde::DebconfGui *m_debconfGui;
    ApplicationUpdates* m_backendUpdater;
    MuonMainWindow *m_aptify;
    bool m_aptBackendInitialized;
public Q_SLOTS:
    void reload();
    void addTransaction(Transaction *transaction);
    //helper functions
    void initAvailablePackages(KJob*);

private Q_SLOTS:
    void setApplications();
    void aptTransactionsChanged(QString active);
    void transactionEvent(QApt::TransactionStatus status);
    void errorOccurred(QApt::ErrorCode error);
    void updateProgress(int percentage);
    void initBackend();
    void setupTransaction(QApt::Transaction *trans);
    void sourcesEditorClosed();
    void checkForUpdates();
    void updateFinished(QApt::ExitStatus);
    void listBugsFinished();

Q_SIGNALS:
    void transactionOk();
    void startingFirstTransaction();
    void sourcesEditorFinished();
    void aptBackendInitialized(QApt::Backend* backend);
};

#endif
