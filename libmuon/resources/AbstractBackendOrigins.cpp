/***************************************************************************
 *   Copyright © 2012 Aleix Pol Gonzalez <aleixpol@blue-systems.com>       *
 *   Copyright © 2013 Lukas Appelhans <l.appelhans@gmx.de>                 *
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
#include "AbstractBackendOrigins.h"
#include "AbstractResourcesBackend.h"

Entry::Entry(const QString& suite, bool enabled, Source* p)
  : QObject(p)
  , m_suite(suite)
  , m_enabled(enabled)
  , m_source(p)
{

}

Source::Source(const QString& uri, const QString& name, QList< Entry* > entries, AbstractBackendOrigins* parent)
  : QObject(parent)
  , m_uri(uri)
  , m_name(name)
  , m_entries(entries)
{

}

AbstractBackendOrigins::AbstractBackendOrigins(AbstractResourcesBackend* parent)
  : QObject(parent)
{
    connect(parent, SIGNAL(backendReady()), SLOT(load()));
}

AbstractBackendOrigins::~AbstractBackendOrigins()
{

}
