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

// LibQApt includes
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
    m_package = 0;
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
