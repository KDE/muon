/***************************************************************************
 *   Copyright © 2011 Jonathan Thomas <echidnaman@kubuntu.org>             *
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

#include "UpdateModel.h"

// Qt includes
#include <QtGui/QFont>
#include <QApplication>

// KDE includes
#include <KGlobal>
#include <KIconLoader>
#include <KLocale>
#include <KDebug>

// Own includes
#include "UpdateItem.h"
#include <resources/AbstractResource.h>
#include <resources/ResourcesUpdatesModel.h>

#define ICON_SIZE KIconLoader::SizeSmallMedium


UpdateModel::UpdateModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_updates(nullptr)
{
    m_rootItem = new UpdateItem();
}

UpdateModel::~UpdateModel()
{
    delete m_rootItem;
}

QVariant UpdateModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    UpdateItem *item = static_cast<UpdateItem*>(index.internalPointer());
    int column = index.column();

    switch (role) {
    case Qt::DisplayRole:
        switch (column) {
        case NameColumn:
            return item->name();
        case VersionColumn:
            return item->version();
        case SizeColumn:
            return KGlobal::locale()->formatByteSize(item->size());
        }
        break;
    case Qt::DecorationRole:
        if (column == NameColumn) {
            return item->icon().pixmap(ICON_SIZE, ICON_SIZE);
        }
        break;
    case Qt::FontRole: {
        QFont font;
        if ((item->type() == UpdateItem::ItemType::CategoryItem) && column == SizeColumn) {
            font.setBold(true);
            return font;
        }
        return font;
    }
    case Qt::CheckStateRole:
        if (column == NameColumn) {
            return item->checked();
        }
        break;
    default:
        break;
    }

    return QVariant();
}

void UpdateModel::checkResources(const QList<AbstractResource*>& resource, bool checked)
{
    if (resource.size() > 1) {
        QApplication::setOverrideCursor(Qt::WaitCursor);
    }
    if(checked)
        m_updates->addResources(resource);
    else
        m_updates->removeResources(resource);
    QApplication::restoreOverrideCursor();
}

QVariant UpdateModel::headerData(int section, Qt::Orientation orientation,
                                int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch (section) {
        case NameColumn:
            return i18nc("@label Column label", "Updates");
        case VersionColumn:
            return i18nc("@label Column label", "Version");
        case SizeColumn:
            return i18nc("@label Column label", "Download Size");
        }
    }

    return QVariant();
}

Qt::ItemFlags UpdateModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QModelIndex UpdateModel::index(int row, int column, const QModelIndex &index) const
{
    // Bounds checks
    if (!m_rootItem || row < 0 || column < 0 || column > 3 ||
        (index.isValid() && index.column() != 0)) {
        return QModelIndex();
    }

    if (UpdateItem *parent = itemFromIndex(index)) {
        if (UpdateItem *childItem = parent->child(row))
            return createIndex(row, column, childItem);
    }

    return QModelIndex();
}

QModelIndex UpdateModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QModelIndex();
    }

    UpdateItem *childItem = itemFromIndex(index);
    UpdateItem *parentItem = childItem->parent();

    if (parentItem == m_rootItem) {
        return QModelIndex();
    }

    return createIndex(parentItem->row(), 0, parentItem);
}

int UpdateModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return 0;

    UpdateItem *parentItem = itemFromIndex(parent);

    return parentItem ? parentItem->childCount() : 0;
}

int UpdateModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

UpdateItem* UpdateModel::itemFromIndex(const QModelIndex &index) const
{
    if (index.isValid())
         return static_cast<UpdateItem*>(index.internalPointer());
    return m_rootItem;
}

bool UpdateModel::setData(const QModelIndex &idx, const QVariant &value, int role)
{
    if (role == Qt::CheckStateRole) {
        UpdateItem *item = static_cast<UpdateItem*>(idx.internalPointer());
        bool newValue = value.toBool();
        UpdateItem::ItemType type = item->type();

        QList<AbstractResource *> apps;
        if (type == UpdateItem::ItemType::CategoryItem) {
            // Collect items to (un)check
            foreach (UpdateItem *child, item->children()) {
                apps << child->app();
            }
        } else if (type == UpdateItem::ItemType::ApplicationItem) {
            apps << item->app();
        }

        checkResources(apps, newValue);
        dataChanged(idx, idx);
        if (type == UpdateItem::ItemType::ApplicationItem) {
            QModelIndex parentIndex = idx.parent();
            emit dataChanged(parentIndex, parentIndex);
        } else {
            emit dataChanged(index(0,0, idx), index(item->childCount()-1, 0, idx));
        }

        return true;
    }

    return false;
}

void UpdateModel::packageChanged()
{
    // We don't know what changed or not, so say everything changed
    emit dataChanged(index(0, 0), QModelIndex());
}

void UpdateModel::setResources(const QList< AbstractResource* >& resources)
{
    beginResetModel();
    delete m_rootItem;
    m_rootItem = new UpdateItem;

    UpdateItem *securityItem = new UpdateItem(i18nc("@item:inlistbox", "Important Security Updates"),
                                              KIcon("security-medium"));

    UpdateItem *appItem = new UpdateItem(i18nc("@item:inlistbox", "Application Updates"),
                                          KIcon("applications-other"));

    UpdateItem *systemItem = new UpdateItem(i18nc("@item:inlistbox", "System Updates"),
                                             KIcon("applications-system"));

    foreach(AbstractResource* res, resources) {
        UpdateItem *updateItem = new UpdateItem(res);
        if (res->isFromSecureOrigin()) {
            securityItem->appendChild(updateItem);
        } else if(!res->isTechnical()) {
            appItem->appendChild(updateItem);
        } else {
            systemItem->appendChild(updateItem);
        }
    }

    // Add populated items to the model
    if (securityItem->childCount()) {
        securityItem->sort();
        m_rootItem->appendChild(securityItem);
    } else {
        delete securityItem;
    }

    if (appItem->childCount()) {
        appItem->sort();
        m_rootItem->appendChild(appItem);
    } else {
        delete appItem;
    }

    if (systemItem->childCount()) {
        systemItem->sort();
        m_rootItem->appendChild(systemItem);
    } else {
        delete systemItem;
    }
    endResetModel();
}

void UpdateModel::setBackend(ResourcesUpdatesModel* updates)
{
    m_updates = updates;
}
