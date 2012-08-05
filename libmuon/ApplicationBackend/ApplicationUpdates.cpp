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

#include "ApplicationUpdates.h"

#include "Application.h"
#include "ApplicationBackend.h"
#include <LibQApt/Backend>
#include <KDebug>
#include <QIcon>

ApplicationUpdates::ApplicationUpdates(ApplicationBackend* parent)
    : AbstractBackendUpdater(parent)
    , m_aptBackend(0)
    , m_appBackend(parent)
{
}

void ApplicationUpdates::setBackend(QApt::Backend* backend)
{
    Q_ASSERT(!m_aptBackend || m_aptBackend==backend);
    m_aptBackend = backend;
}

void ApplicationUpdates::start()
{
    connect(m_aptBackend, SIGNAL(commitProgress(QString,int)),
            SLOT(progress(QString,int)));
    connect(m_aptBackend, SIGNAL(downloadMessage(int,QString)), SLOT(downloadMessage(int,QString)));

    m_aptBackend->saveCacheState();
    m_aptBackend->markPackagesForUpgrade();
    m_aptBackend->commitChanges();
}

bool ApplicationUpdates::hasUpdates() const
{
    return m_appBackend->updatesCount()>0;
}

qreal ApplicationUpdates::progress() const
{
    return m_progress;
}

void ApplicationUpdates::progress(const QString& msg, int percentage)
{
    m_progress = qreal(percentage)/100;
    emit message(QIcon(), msg);
    emit progressChanged(m_progress);
}

void ApplicationUpdates::downloadMessage(int flag, const QString& msg)
{
    Q_UNUSED(flag)
    emit message(QIcon::fromTheme("download"), msg);
}

void ApplicationUpdates::installMessage(const QString& msg)
{
    emit message(QIcon(), msg);
}