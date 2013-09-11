/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright 2013  Lukas Appelhans <l.appelhans@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "ApplicationOrigins.h"
#include <resources/ResourcesModel.h>
#include <QProcess>
#include <QDebug>
#include <QDir>
#include <QMainWindow>
#include <qdeclarative.h>
#include <LibQApt/Backend>
#include <LibQApt/Config>
#include <KMessageBox>
#include <KLocalizedString>

ApplicationOrigins::ApplicationOrigins(ApplicationBackend* backend)
  : AbstractBackendOrigins(backend),
    m_backend(backend)
{

}

ApplicationOrigins::~ApplicationOrigins()
{

}

void ApplicationOrigins::addRepository(const QString& repository)
{

}

void ApplicationOrigins::removeRepository(const QString& repository)
{

}

void ApplicationOrigins::load()
{
    QApt::Backend* backend = qobject_cast<QApt::Backend*>(m_backend->property("backend").value<QObject*>());
    if (!backend) {
        connect(m_backend, SIGNAL(backendReady()), SLOT(load()));
        return;
    }
    
    m_sourcesList.reload();

    for (const QApt::SourceEntry &sEntry : m_sourcesList.entries()) {
        if (!sEntry.isValid())
            continue;

        Source* newSource = sourceForUri(sEntry.uri());
        Entry* entry = new Entry(sEntry.dist(), sEntry.isEnabled(), newSource);
        newSource->addEntry(entry);
    }

    emit originsChanged();
}

Source* ApplicationOrigins::sourceForUri(const QString& uri)
{
    foreach(Source* s, m_sources) {
        if (s->uri()==uri)
            return s;
    }
    Source* s = new Source(QString(), uri, QList<Entry*>(), this);//FIXME Resolve name here
    m_sources += s;
    return s;
}

QList< Source* > ApplicationOrigins::sources() const
{
    return m_sources;
}
