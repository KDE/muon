/***************************************************************************
 *   Copyright © 2010-2013 Jonathan Thomas <echidnaman@kubuntu.org>        *
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

#include "QAptResource.h"

// Qt includes
#include <QFile>

// KDE includes
#include <KIO/Job>
#include <KIO/NetAccess>
#include <KLocale>

// LibQApt includes
#include <LibQApt/Changelog>
#include <LibQApt/Config>

// Own includes
#include "ApplicationBackend.h"

QAptResource::QAptResource(ApplicationBackend *parent,
                           QApt::Backend *backend,
                           QApt::Package *package)
    : AbstractResource(parent)
    , m_backend(backend)
    , m_package(package)
    , m_isValid(true)
{
}

bool QAptResource::isValid() const
{
    return m_isValid;
}

void QAptResource::clearPackage()
{
    m_package = nullptr;
}

AbstractResource::State QAptResource::state()
{
    State ret = None;
    if (package()) {
        int s = package()->state();
        if(s & QApt::Package::Upgradeable) ret = Upgradeable;
        else if(s & QApt::Package::Installed) ret = Installed;
    }

    return ret;
}


QList<PackageState> QAptResource::addonsInformation()
{
    QList<PackageState> ret;

    QApt::PackageList pkgs = addons();
    for (QApt::Package* p : pkgs) {
        ret += PackageState(p->name(), p->shortDescription(), p->isInstalled());
    }

    return ret;
}

QApt::PackageList QAptResource::addons()
{
    QApt::PackageList addons;

    QApt::Package *pkg = package();
    if (!pkg)
        return addons;

    QStringList tempList;
    // Only add recommends or suggests to the list if they aren't already going to be
    // installed
    if (!m_backend->config()->readEntry("APT::Install-Recommends", true)) {
        tempList << m_package->recommendsList();
    }
    if (!m_backend->config()->readEntry("APT::Install-Suggests", false)) {
        tempList << m_package->suggestsList();
    }
    tempList << m_package->enhancedByList();

    QStringList languagePackages;
    QFile l10nFilterFile("/usr/share/language-selector/data/pkg_depends");

    if (l10nFilterFile.open(QFile::ReadOnly)) {
        QString contents = l10nFilterFile.readAll();

        foreach (const QString &line, contents.split('\n')) {
            if (line.startsWith(QLatin1Char('#'))) {
                continue;
            }
            languagePackages << line.split(':').last();
        }

        languagePackages.removeAll("");
    }

    for (const QString &addon : tempList) {
        bool shouldShow = true;
        QApt::Package *package = m_backend->package(addon);

        if (!package || QString(package->section()).contains("lib") || addons.contains(package)) {
            continue;
        }

        foreach (const QString &langpack, languagePackages) {
            if (addon.contains(langpack)) {
                shouldShow = false;
                break;
            }
        }

        if (shouldShow) {
            addons << package;
        }
    }

    return addons;
}

void QAptResource::fetchChangelog()
{
    KIO::StoredTransferJob* getJob = KIO::storedGet(m_package->changelogUrl(), KIO::NoReload, KIO::HideProgressInfo);
    connect(getJob, SIGNAL(result(KJob*)),
            this, SLOT(processChangelog(KJob*)));
}

void QAptResource::processChangelog(KJob* j)
{
    KIO::StoredTransferJob* job = qobject_cast<KIO::StoredTransferJob*>(j);
    if (!m_package || !job) {
        return;
    }

    QString changelog;
    if(j->error() == 0)
        changelog = buildDescription(job->data(), m_package->sourcePackage());

    if (changelog.isEmpty()) {
        if (m_package->origin() == QLatin1String("Ubuntu")) {
            changelog = i18nc("@info/rich", "The list of changes is not yet available. "
                                            "Please use <link url='%1'>Launchpad</link> instead.",
                                            QString("http://launchpad.net/ubuntu/+source/" + m_package->sourcePackage()));
        } else {
            changelog = i18nc("@info", "The list of changes is not yet available.");
        }
    }
    emit changelogFetched(changelog);
}

QString QAptResource::buildDescription(const QByteArray& data, const QString& source)
{
    QApt::Changelog changelog(data, source);
    QString description;

    QApt::ChangelogEntryList entries = changelog.newEntriesSince(m_package->installedVersion());

    if (entries.size() < 1) {
        return description;
    }

    foreach(const QApt::ChangelogEntry &entry, entries) {
        description += i18nc("@info:label Refers to a software version, Ex: Version 1.2.1:",
                             "Version %1:", entry.version());

        QString issueDate = KGlobal::locale()->formatDateTime(entry.issueDateTime(), KLocale::ShortDate);
        description += QLatin1String("<p>") +
                       i18nc("@info:label", "This update was issued on %1", issueDate) +
                       QLatin1String("</p>");

        QString updateText = entry.description();
        updateText.replace('\n', QLatin1String("<br/>"));
        description += QLatin1String("<p><pre>") + updateText + QLatin1String("</pre></p>");
    }

    return description;
}
