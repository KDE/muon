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

#include "AkabeiOrigins.h"
#include <akabeiclientdatabasehandler.h>
#include <akabeiclientbackend.h>
#include <KDebug>

AkabeiOrigins::AkabeiOrigins(AbstractResourcesBackend* parent)
  : AbstractBackendOrigins(parent)
{

}

AkabeiOrigins::~AkabeiOrigins()
{
}

void AkabeiOrigins::load()
{
    kDebug() << "LOAD WITH" << AkabeiClient::Backend::instance()->databases().count();
    foreach (AkabeiClient::DatabaseHandler * db, AkabeiClient::Backend::instance()->databases()) {
        QUrl url = db->mirror();
        QString host = url.host();
        if (!m_sources[host]) {
            m_sources.insert(host, new Source(QString(), host, QList<Entry*>(), this));
        }
        m_sources[host]->addEntry(new Entry(db->name(), true, m_sources[host]));
    }
    emit originsChanged();
}

void AkabeiOrigins::removeRepository(const QString& repository)
{
    //TODO!!!
}

void AkabeiOrigins::addRepository(const QString& repository)
{
    //TODO!!!
}

Source * AkabeiOrigins::sourceForUri(const QString& uri)
{
    return m_sources[uri];
}

QList<Source*> AkabeiOrigins::sources() const
{
    return m_sources.values();
}
