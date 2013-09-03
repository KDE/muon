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

#ifndef ABSTRACTBACKENDORIGINS_H
#define ABSTRACTBACKENDORIGINS_H

#include "libmuonprivate_export.h"

#include <QObject>

class Source;
class AbstractResourcesBackend;

class MUONPRIVATE_EXPORT Entry : public QObject
{
    Q_OBJECT
    //Q_PROPERTY(QStringList args READ args CONSTANT)
    Q_PROPERTY(QString suite READ suite CONSTANT)
    Q_PROPERTY(bool enabled READ isEnabled CONSTANT)
    Q_PROPERTY(Source * source READ source CONSTANT)
    public:
        Entry(const QString &suite, bool enabled, Source* p);
        
        //QStringList args() const { return m_entry.components(); }
        
        QString suite() const { return m_suite; }
        
        void setEnabled(bool enabled) { m_enabled = enabled; }
        bool isEnabled() const { return m_enabled; }
        Source * source() const { return m_source; }
        
    private:
       QString m_suite;
       bool m_enabled;
       Source * m_source;
};

class AbstractBackendOrigins;

class MUONPRIVATE_EXPORT Source : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString uri READ uri CONSTANT)
    Q_PROPERTY(QList<Entry*> entries READ entries CONSTANT)
    public:
        Source(const QString &uri, const QString &name, QList<Entry*> entries, AbstractBackendOrigins* parent);
        
        QString uri() { return m_uri; }
        void setUri(const QString& uri) { m_uri = uri; }
        void addEntry(Entry* entry) { m_entries.append(entry); }
        void removeEntry(Entry* entry) { m_entries.removeAll(entry); }
        QList<Entry*> entries() const { return m_entries; };
        QString name() const { return m_name; };
        
    private:
        QString m_uri;
        QString m_name;
        QList<Entry*> m_entries;
};

class MUONPRIVATE_EXPORT AbstractBackendOrigins : public QObject
{
    Q_OBJECT
    public:
        ~AbstractBackendOrigins();
        
        virtual QList<Source*> sources() const = 0;
        virtual Source* sourceForUri(const QString& uri) = 0;

    public slots:
        virtual void addRepository(const QString& repository) = 0;
        virtual void removeRepository(const QString& repository) = 0;
        virtual void load() = 0;

    signals:
        void originsChanged();
        
    protected:
        explicit AbstractBackendOrigins(AbstractResourcesBackend* parent = 0);
};

#endif // ABSTRACTBACKENDORIGINS_H
