/***************************************************************************
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

#ifndef RESOURCESORIGINSMODEL_H
#define RESOURCESORIGINSMODEL_H

#include <QAbstractListModel>
#include <libmuonprivate_export.h>

class Source;
class AbstractBackendOrigins;
class ResourcesModel;
class MUONPRIVATE_EXPORT ResourcesOriginsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        NameRole = Qt::UserRole,
        UriRole,
        SuitesRole
    };
    ResourcesOriginsModel(ResourcesModel * model);
    ~ResourcesOriginsModel();
    
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    
private slots:
    void reload();
    
private:
    QList<AbstractBackendOrigins*> m_origins;
    QList<Source*> m_sources;

};

#endif // RESOURCESORIGINSMODEL_H
