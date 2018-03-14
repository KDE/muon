/***************************************************************************
 *   Copyright © 2010,2011 Jonathan Thomas <echidnaman@kubuntu.org>        *
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

#include "ChangelogTab.h"

// Qt includes
#include <QDir>
#include <QtCore/QTextStream>
#include <QTemporaryFile>
#include <QTextBrowser>
#include <QUrl>

// KDE includes
#include <KIO/Job>
#include <KJob>
#include <KLocalizedString>
#include <KPixmapSequence>
#include <KPixmapSequenceOverlayPainter>
#include <KIconLoader>

// QApt includes
#include <QApt/Package>
#include <QApt/Changelog>

ChangelogTab::ChangelogTab(QWidget *parent)
    : DetailsTab(parent)
{
    m_name = i18nc("@title:tab", "Changes List");

    m_changelogBrowser = new QTextBrowser(this);

    m_busyWidget = new KPixmapSequenceOverlayPainter(this);
    m_busyWidget->setSequence(KIconLoader::global()->loadPixmapSequence("process-working", KIconLoader::SizeSmallMedium));
    m_busyWidget->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_busyWidget->setWidget(m_changelogBrowser->viewport());

    m_layout->addWidget(m_changelogBrowser);
}

void ChangelogTab::setPackage(QApt::Package *package)
{
    m_package = package;
}

void ChangelogTab::refresh()
{
    fetchChangelog();
}

void ChangelogTab::clear()
{
    DetailsTab::clear();

    // Delete any KJobs lying around. We could get stale package pointers if the jobs
    // finish during a cache reload
    auto iter = m_jobFilenames.constBegin();
    while (iter != m_jobFilenames.constEnd()) {
        iter.key()->deleteLater();
        ++iter;
    }

    m_jobFilenames.clear();
}

void ChangelogTab::changelogFetched(KJob *job)
{
    // Work around http://bugreports.qt.nokia.com/browse/QTBUG-2533 by forcibly resetting the CharFormat
    QTextCharFormat format;
    m_changelogBrowser->setCurrentCharFormat(format);
    QFile changelogFile(m_jobFilenames[job]);

    m_jobFilenames.remove(job);

    m_busyWidget->stop();
    if (job->error() || !changelogFile.open(QFile::ReadOnly)) {
        if (m_package->origin() == QStringLiteral("Ubuntu")) {
            m_changelogBrowser->setText(xi18nc("@info/rich", "The list of changes is not yet available. "
                                               "Please use <link url='%1'>Launchpad</link> instead.",
                                               QStringLiteral("http://launchpad.net/ubuntu/+source/") + m_package->sourcePackage()));
        } else {
            m_changelogBrowser->setText(i18nc("@info", "The list of changes is not yet available."));
        }
    }
    else {
        QTextStream stream(&changelogFile);
        const QApt::Changelog log(stream.readAll(), m_package->sourcePackage());
        m_changelogBrowser->setText(log.text());
    }

    changelogFile.remove();
    job->deleteLater();
}

void ChangelogTab::fetchChangelog()
{
    if (!m_package) {
        return;
    }

    m_changelogBrowser->clear();
    m_busyWidget->start();

    QTemporaryFile *changelogFile = new QTemporaryFile(QDir::tempPath() + QLatin1String("/muon_XXXXXX.txt"));
    changelogFile->setAutoRemove(false);
    changelogFile->open();
    QString filename = changelogFile->fileName();
    delete changelogFile;

    KIO::FileCopyJob *getJob = KIO::file_copy(m_package->changelogUrl(),
                               QUrl::fromLocalFile(filename), -1,
                               KIO::Overwrite | KIO::HideProgressInfo);
    getJob->setAutoDelete(false);
    m_jobFilenames.insert(getJob, filename);
    connect(getJob, SIGNAL(result(KJob*)),
            this, SLOT(changelogFetched(KJob*)));
}
