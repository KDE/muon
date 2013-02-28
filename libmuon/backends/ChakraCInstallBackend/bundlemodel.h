/*
    Bundle Manager - Graphical manager for Chakra Bundles

    Copyright (C) 2011  Giuseppe Calà <jiveaxe@gmail.com>
    Copyright (C) 2011  Johannes Tröscher <fritz_van_tom@hotmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BUNDLEMODEL_H
#define BUNDLEMODEL_H

//Project Includes
#include "defines.h"

//KDE Includes
#include <KCategorizedSortFilterProxyModel>
#include <KUrl>

//Qt Includes
#include <QAbstractItemModel>
#include <QDir>

//Forward Declarations
class QProcess;
class BundleModelPrivate;
class KJob;

namespace KIO
{
    class FileCopyJob;
    class DeleteJob;
}


class BundleModel : public KCategorizedSortFilterProxyModel
{
    Q_OBJECT

    Q_PROPERTY(bool proceeding READ proceeding)
    Q_PROPERTY(bool showInstalledOnly READ showInstalledOnly WRITE setShowInstalledOnly)

public:
    BundleModel(QObject* parent = 0);
    bool filterAcceptsRow (int source_row, const QModelIndex &source_parent) const;
    int compareCategories (const QModelIndex &left, const QModelIndex &right) const;

    bool proceeding() const;
    inline bool showInstalledOnly() const { return m_showInstalledOnly; }

    void installExtnernal(const KUrl &url);

public slots:
    void reload();
    void setShowInstalledOnly(bool installedOnly);
    void cancelAll();
    void updateAll();

signals:
    void errorMessage(QString msg);
    void proceedingChanged(bool b);
    void hasUpdatableBundles(bool b);

private:
    BundleModelPrivate *m_sourceModel;

    bool m_proceedingChanges;
    bool m_showInstalledOnly;
};


class BundleModelPrivate : public QAbstractListModel
{
    Q_OBJECT

public:
    BundleModelPrivate(QObject* parent = 0);
    ~BundleModelPrivate();

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    void updateList();
    void cancelAll();
    void updateAll();

    void runBundle(const QModelIndex index);
    void downloadBundle(const int &index);
    void removeBundle(const int &index);
    void updateBundle(const int &index);
    void cancelJob(const int &index);
    void installExtnernal(const KUrl &url);

    inline bool proceeding() const {return !m_runningJobs.isEmpty();};

signals:
    void errorMessage(QString msg);
    void proceedingChanged(bool b);
    void hasUpdatableBundles(bool b);
    void m_2(QString BundlePath);

private slots:
    void addAvailableBundles(KJob *job);
    void setFinishedPercentage(KJob *job, unsigned long percent);
    void jobFinished(KJob *job);
    void setSpeed(KJob *job, ulong speed);

private:
    QList<BundleEntryInfo> m_info;
    QHash<int, BundleEntryInfo> m_updatableBundles;
    BundleEntryInfo infoForInstalledBundle(const QString &BundlePath) const;
    QList<int> m_runningJobs;
    QHash<int, KIO::FileCopyJob*> m_copyJobs;
    QString installBundle(const QString& bundle);

    void addInstalledBundles();
    void addAvailableBundles();

    QDir m_cacheDir;
    QDir m_repoDir;
    QDir m_iconsDir;

    QString m_currentArch;

    void cleanUpJobs();
};

#endif // BUNDLEMODEL_H
