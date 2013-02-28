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

//Self Includes
#include "bundlemodel.h"
#include "defines.h"

//Project Includes
#include "cbundle/bundle.h"
#include "cbundle/MountPointException.h"
#include "bundlelauncher.h"
#include "utils.h"

//KDE Includes
#include <KDesktopFile>
#include <KIO/Job>
#include <KIO/FileCopyJob>
#include <KIO/DeleteJob>
#include <KUrl>
#include <KLocale>
#include <KProcess>

#include "qjson/parser.h"

void BundleModelPrivate::downloadBundle(const int& index)
{
    QString bundleName;
    if (m_updatableBundles.contains(index))
        bundleName = m_updatableBundles.value(index).completeName + "-" + m_currentArch;
    else
        bundleName = m_info[index].completeName + "-" + m_currentArch;

    KUrl url ("http://chakra-project.org/repo/bundles/" + m_currentArch + "/" + bundleName + ".cb");

    KIO::FileCopyJob *fcJob = KIO::file_copy(url, "/tmp/" + bundleName + ".cb",
                                             -1, KIO::Overwrite | KIO::HideProgressInfo);
    connect(fcJob, SIGNAL(percent(KJob*,ulong)), this, SLOT(setFinishedPercentage(KJob*,ulong)));
    connect(fcJob, SIGNAL(finished(KJob*)), this, SLOT(jobFinished(KJob*)));
    connect(fcJob, SIGNAL(speed(KJob*,ulong)), this, SLOT(setSpeed(KJob*,ulong)));
    fcJob->setProperty("index", index);
    m_copyJobs.insert(index, fcJob);
    m_runningJobs.append(index);
    emit proceedingChanged(true);
}


void BundleModelPrivate::removeBundle(const int& index)
{
    QString bundleName = m_info[index].completeName + "-" + m_currentArch;
    QString bundleFile(m_repoDir.absolutePath() + QDir::separator() + bundleName + ".cb");

    if (Bundle::remove(bundleFile, "cinstall"))
    {
        m_info[index].amountProceeded = 0;
        m_info[index].sizeProceeded = 0;
        m_info[index].speed = 0;
        m_info[index].installed = false;
        m_info[index].icon.clear();
        emit dataChanged(createIndex(index, 0), createIndex(index, 0));
        emit proceedingChanged(!m_runningJobs.isEmpty());
    }
}

void BundleModelPrivate::updateAll()
{
    QHashIterator<int, BundleEntryInfo> i(m_updatableBundles);
    while (i.hasNext())
    {
        i.next();
        updateBundle(i.key());
    }
}


void BundleModelPrivate::cancelJob(const int& index)
{
    KIO::FileCopyJob *j = m_copyJobs.take(index);
    j->disconnect();
    j->kill();
    delete j;

    m_info[index].amountProceeded = 0;
    m_info[index].sizeProceeded = 0;
    m_info[index].speed = 0;
    m_runningJobs.removeOne(index);

    emit proceedingChanged(!m_runningJobs.isEmpty());
    emit dataChanged(createIndex(index, 0), createIndex(index, 0));
}

void BundleModelPrivate::setFinishedPercentage(KJob* job, long unsigned int percent)
{
    const int index = job->property("index").toInt();
    const QModelIndex mIndex = createIndex(index, 0);

    m_info[index].amountProceeded = percent;
    m_info[index].sizeProceeded = job->processedAmount(KJob::Bytes);
    emit dataChanged(mIndex, mIndex);
}


void BundleModelPrivate::jobFinished(KJob* job)
{
    if (job->property("external").toBool())
    {
        KIO::FileCopyJob *cj = qobject_cast<KIO::FileCopyJob*>(job);
        QString bundleName = installBundle(cj->destUrl().path());

        if (bundleName != QString())
        {
            beginInsertRows(QModelIndex(), 0, 0);
            m_info.prepend(infoForInstalledBundle(bundleName + ".desktop"));
            endInsertRows();
            emit dataChanged(createIndex(0, 0), createIndex(0, 0));
        }
        return;
    }

    const int index = job->property("index").toInt();

    if (m_updatableBundles.contains(index))
    {
        QString oldBundle = m_info[index].completeName + "-" + m_currentArch;
        QString newBundle = m_updatableBundles.value(index).completeName + "-" + m_currentArch;

        Bundle::remove(m_repoDir.absolutePath() + QDir::separator() + oldBundle + ".cb", "cinstall");
        if (installBundle("/tmp/" + newBundle + ".cb") != QString())
        {
            m_info[index] = infoForInstalledBundle(newBundle + ".desktop");
            m_updatableBundles.remove(index);
        }
    }
    else
    {
        QString bundleName = m_info[index].completeName + "-" + m_currentArch;
        if (installBundle("/tmp/" + bundleName + ".cb") != QString())
            m_info[index] = infoForInstalledBundle(bundleName + ".desktop");
    }

    m_runningJobs.removeOne(index);

    emit proceedingChanged(!m_runningJobs.isEmpty());
    emit dataChanged(createIndex(index, 0), createIndex(index, 0));
}


void BundleModelPrivate::setSpeed(KJob* job, ulong speed)
{
    const int index = job->property("index").toInt();
    const QModelIndex mIndex = createIndex(index, 0);

    m_info[index].speed = speed;
    emit dataChanged(mIndex, mIndex);
}


QString BundleModelPrivate::installBundle(const QString& bundle)
{
    try
    {
        Bundle b("cinstall", bundle, QStringList(), this);
        b.setExecuting(false);
        b.run();
        b.unmount();
        return b.getBundleName() + "-" + b.getBundleVersion() + "-" + b.getBundleArch();
    }
    catch (MountPointException &ex)
    {
        emit errorMessage(ex.what());
        return QString();
    }
}


void BundleModelPrivate::cleanUpJobs()
{
    qDeleteAll(m_copyJobs);
    m_copyJobs.clear();
}


void BundleModelPrivate::installExtnernal(const KUrl &url)
{
    KIO::FileCopyJob *fcJob = KIO::file_copy(url, "/tmp/" + url.fileName(),
                                             -1, KIO::Overwrite | KIO::HideProgressInfo);
    fcJob->setProperty("external", true);
    connect(fcJob, SIGNAL(finished(KJob*)), this, SLOT(jobFinished(KJob*)));
}
