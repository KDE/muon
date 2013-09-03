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

#include "ResourcesOriginsModel.h"
#include "AbstractResourcesBackend.h"
#include "ResourcesModel.h"
#include "AbstractBackendOrigins.h"
#include <QTimer>
#include <KDebug>

ResourcesOriginsModel::ResourcesOriginsModel(ResourcesModel * model)
  : QAbstractListModel(model)
{
    QHash< int, QByteArray > roles = roleNames();
    roles[NameRole] = "name";
    roles[UriRole] = "uri";
    roles[SuitesRole] = "suites";
    roles.remove(Qt::EditRole);
    roles.remove(Qt::WhatsThisRole);
    setRoleNames(roles);
    for (AbstractResourcesBackend * backend : model->backends()) {
        if (backend->origins()) {
            m_origins.append(backend->origins());
            connect(backend->origins(), SIGNAL(originsChanged()), SLOT(reload()));
        }
    }
    QTimer::singleShot(2000, this, SLOT(reload()));
}

ResourcesOriginsModel::~ResourcesOriginsModel()
{

}

void ResourcesOriginsModel::reload()
{
    if (!m_sources.isEmpty()) {
        beginRemoveRows(QModelIndex(), 0, m_sources.count() - 1);
        m_sources.clear();
        endRemoveRows();
    }
    
    QList<Source*> sources;
    for (AbstractBackendOrigins * origin : m_origins) {
        sources << origin->sources();
    }
    
    if (!sources.isEmpty()) {
        beginInsertRows(QModelIndex(), 0, sources.count() - 1);
        m_sources = sources;
        endInsertRows();
    }
    
    kDebug() << "ROWS" << rowCount();
}

QVariant ResourcesOriginsModel::data(const QModelIndex& index, int role) const
{
    if (index.row() > m_sources.count())
        return QVariant();
    switch (role) {
        case Qt::DisplayRole:
            return m_sources.at(index.row())->name();
        case NameRole:
            return m_sources.at(index.row())->name();
        case UriRole:
            return m_sources.at(index.row())->uri();
        case SuitesRole:
            QStringList list;
            foreach (Entry * e, m_sources.at(index.row())->entries()) {
                list.append(e->suite());
            }
            return list.join(", ");
    };
    return QVariant();
}

int ResourcesOriginsModel::rowCount(const QModelIndex& parent) const
{
    return m_sources.count();
}
