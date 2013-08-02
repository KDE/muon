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

#ifndef KNSBACKEND_H
#define KNSBACKEND_H

// KDE includes
#include <knewstuff3/entry.h>

// Attica includes
#include <attica/category.h>
#include <attica/provider.h>

// Libmuon includes
#include <resources/AbstractResourcesBackend.h>
#include "Transaction/AddonList.h"

#include "libmuonprivate_export.h"

class KConfigGroup;
class KNSReviews;
namespace KNS3 { class DownloadManager; }
namespace Attica {
    class ProviderManager;
    class BaseJob;
}

class MUONPRIVATE_EXPORT KNSBackend : public AbstractResourcesBackend
{
Q_OBJECT
Q_INTERFACES(AbstractResourcesBackend)
public:
    explicit KNSBackend(QObject* parent, const QVariantList& args);
    virtual ~KNSBackend();
    
    virtual void cancelTransaction(AbstractResource* app);
    virtual void removeApplication(AbstractResource* app);
    virtual void installApplication(AbstractResource* app);
    virtual void installApplication(AbstractResource* app, AddonList addons);
    virtual AbstractResource* resourceByPackageName(const QString& name) const;
    virtual int updatesCount() const;
    virtual AbstractReviewsBackend* reviewsBackend() const;
    virtual QList<AbstractResource*> searchPackageName(const QString& searchText);
    virtual QVector< AbstractResource* > allResources() const;
    virtual AbstractBackendUpdater* backendUpdater() const;
    virtual bool isFetching() const;

    bool isValid() const;
    Attica::Provider* provider() { return &m_provider; }
    QList<AbstractResource*> upgradeablePackages() const;

public slots:
    void receivedEntries(const KNS3::Entry::List& entry);
    void startFetchingCategories();
    void categoriesLoaded(Attica::BaseJob*);
    void receivedContents(Attica::BaseJob*);
    void statusChanged(const KNS3::Entry& entry);

private:
    static void initManager(KConfigGroup& group);
    static QSharedPointer<Attica::ProviderManager> m_atticaManager;
    void setFetching(bool f);
    
    bool m_isValid;
    KNS3::DownloadManager* m_manager;
    QHash<QString, AbstractResource*> m_resourcesByName;
    int m_page;
    KNSReviews* m_reviews;
    Attica::Provider m_provider;
    QMap<QString, Attica::Category> m_categories;
    QString m_name;
    bool m_fetching;
    QString m_iconName;
    AbstractBackendUpdater* m_updater;
};

#endif // KNSBACKEND_H
