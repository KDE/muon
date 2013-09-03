/***************************************************************************
 *   Copyright © 2013 Lukas Appelhans <boom1992@chakra-project.org>        *
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

#ifndef MUONAKABEIBACKEND_H
#define MUONAKABEIBACKEND_H

#include <libmuonprivate_export.h>
#include "resources/AbstractResourcesBackend.h"
#include "AkabeiUpdater.h"
#include <QVariantList>
#include <QUuid>
#include <QQueue>
#include <akabeicore/akabeibackend.h>

class AkabeiTransaction;
class AkabeiOrigins;

struct ApplicationData
{
    QString pkgname;
    QString id;
    QHash<QString, QString> name;
    QHash<QString, QString> summary;
    QString icon;
    QString url;
    QHash<QString, QStringList> keywords;
    QStringList appcategories;
    QStringList mimetypes;
};

class MUONPRIVATE_EXPORT AkabeiBackend : public AbstractResourcesBackend
{
    Q_OBJECT
    Q_INTERFACES(AbstractResourcesBackend)
public:
    explicit AkabeiBackend(QObject *parent, const QVariantList& args);
    ~AkabeiBackend();

    bool isValid() const;
    AbstractReviewsBackend *reviewsBackend() const;
    Q_SCRIPTABLE AbstractResource* resourceByPackageName(const QString& name) const;

    int updatesCount() const;
    
    QVector< AbstractResource* > allResources() const;
    QList<AbstractResource*> searchPackageName(const QString& searchText);
    
    void installApplication(AbstractResource *app, AddonList addons);
    void installApplication(AbstractResource *app);
    void removeApplication(AbstractResource *app);
    void cancelTransaction(AbstractResource *app);
    
    AbstractBackendUpdater* backendUpdater() const;
    virtual QList<AbstractResource*> upgradeablePackages() const;
    
    void removeFromQueue(AkabeiTransaction * trans);
    
    bool isTransactionRunning() const;
    
    virtual AbstractBackendOrigins* origins() const;

public slots:
    void statusChanged(Akabei::Backend::Status);
    void queryComplete(QUuid,QList<Akabei::Package*>);
    void reload();
    
private:
    QHash<QString, AbstractResource*> m_packages;
    QQueue<AkabeiTransaction*> m_transactionQueue;
    AkabeiUpdater * m_updater;
    QHash<QString, ApplicationData> m_appdata;
    AkabeiOrigins * m_origins;
};

#endif
