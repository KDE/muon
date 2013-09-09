/*
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

#ifndef APPLICATIONORIGINS_H
#define APPLICATIONORIGINS_H

#include <resources/AbstractBackendOrigins.h>
#include "ApplicationBackend.h"

class ApplicationOrigins : public AbstractBackendOrigins
{
    Q_OBJECT
public:
    ApplicationOrigins(ApplicationBackend * backend);
    ~ApplicationOrigins();
    
    virtual QList<Source*> sources() const;
    virtual Source* sourceForUri(const QString& uri);

public slots:
    virtual void addRepository(const QString& repository);
    virtual void removeRepository(const QString& repository);
    virtual void load();

};

#endif // APPLICATIONORIGINS_H
