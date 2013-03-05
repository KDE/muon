/***************************************************************************
 *   Copyright © 2013 Aleix Pol Gonzalez <aleixpol@blue-systems.com>       *
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

#include "CInstallResource.h"
#include "CInstallBackend.h"
#include <KProcess>
#include <KDesktopFile>
#include <QDebug>
#include <QFileInfo>

CInstallResource::CInstallResource(const QString& bundlePath, CInstallBackend* parent)
    : AbstractResource(parent)
    , m_state(Broken)
{
    initFromPath(bundlePath);
}

CInstallResource::CInstallResource(const BundleEntryInfo& info, AbstractResourcesBackend* parent)
    : AbstractResource(parent)
    , m_info(info)
    , m_state(None)
{}

QUrl CInstallResource::screenshotUrl()
{
    return QUrl();
}

QUrl CInstallResource::thumbnailUrl()
{
    return QUrl();
}

AbstractResource::State CInstallResource::state()
{
    return m_state;
}

QString CInstallResource::icon() const
{
    return m_info.icon.isEmpty() ? "applications-other" : m_info.icon;
}

int CInstallResource::downloadSize()
{
    return m_info.size;
}

void CInstallResource::fetchChangelog()
{
    emit changelogFetched("N/A");
}

void CInstallResource::invokeApplication() const
{
    if(KProcess::startDetached("cinstall", QStringList("-b") << m_info.name)!=0)
        qWarning() << "wtf man";
}

void CInstallResource::updateData(const BundleEntryInfo& b)
{
    Q_ASSERT(b.name == m_info.name);
    if (m_state==Installed && (b.version < m_info.version || (b.version == m_info.version && b.release < m_info.release))) {
        setState(Upgradeable);
    }
    m_info = b;
}

void CInstallResource::initFromPath(const QString& path)
{
    CInstallBackend* b = qobject_cast<CInstallBackend*>(parent());

    static QRegExp rx("(.+)-([.\\d\\w]+)-(\\d+)-(x86_64|i686).desktop");
    rx.exactMatch(path);
    m_info.name = rx.cap(1);
    m_info.version = rx.cap(2);
    m_info.release = rx.cap(3).toInt();
    m_info.completeName = rx.cap(1) + "-" + rx.cap(2) + "-" + rx.cap(3);
    m_info.size = QFileInfo(b->repoDir().filePath(m_info.completeName) + "-" + b->architecture() + ".cb").size();

    KDesktopFile desktopFile(b->cacheDir().filePath(path));
    m_info.icon = desktopFile.readIcon();
    m_info.comment = desktopFile.readComment();

    m_installedVersionCompleteName = m_info.completeName;
    setState(Installed);
}

void CInstallResource::removeBundle()
{
    m_installedVersionCompleteName.clear();
    setState(None);
}

void CInstallResource::setState(AbstractResource::State newState)
{
    m_state = newState;
    emit stateChanged();
}
