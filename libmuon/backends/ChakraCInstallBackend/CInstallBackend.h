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

#ifndef CINSTALLBACKEND_H
#define CINSTALLBACKEND_H

#include <resources/AbstractResourcesBackend.h>
#include "libmuonprivate_export.h"
#include <QVariantList>
#include <QDir>

class StandardBackendUpdater;
class KJob;
class MUONPRIVATE_EXPORT CInstallBackend : public AbstractResourcesBackend
{
Q_OBJECT
Q_INTERFACES(AbstractResourcesBackend)
public:
    explicit CInstallBackend(QObject* parent, const QVariantList& args);
    virtual ~CInstallBackend();
    
    virtual void cancelTransaction(AbstractResource* app);
    virtual void removeApplication(AbstractResource* app);
    virtual void installApplication(AbstractResource* app, AddonList addons);
    virtual void installApplication(AbstractResource* app) { installApplication(app, AddonList()); }
    virtual AbstractResource* resourceByPackageName(const QString& name) const;
    virtual int updatesCount() const;
    virtual AbstractReviewsBackend* reviewsBackend() const;
    virtual QStringList searchPackageName(const QString& searchText);
    virtual QVector< AbstractResource* > allResources() const;
    virtual AbstractBackendUpdater* backendUpdater() const;
    virtual bool isValid() const { return true; } // No external file dependencies that could cause runtime errors

    QList<AbstractResource*> upgradeablePackages() const;

    QString architecture() const;
    QDir repoDir() const { return m_repoDir; }
    QDir cacheDir() const { return m_cacheDir; }

private slots:
    void addInstalledBundles();
    void addAvailableBundles(KJob *job);

private:
    QString m_currentArch;
    QHash<QString, AbstractResource*> m_bundles;
    QDir m_cacheDir;
    QDir m_repoDir;
    QDir m_iconsDir;
    StandardBackendUpdater* m_updater;
};

#endif // CINSTALLBACKEND_H
