/***************************************************************************
 *   Copyright © 2010 Jonathan Thomas <echidnaman@kubuntu.org>             *
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

#include "PackageView.h"

#include "PackageViewHeader.h"

PackageView::PackageView(QWidget *parent)
    : QTreeView(parent)
{
    setHeader(new PackageViewHeader());
    setAlternatingRowColors(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setRootIsDecorated(false);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setUniformRowHeights(true);
    header()->setStretchLastSection(false);
    header()->setDefaultAlignment(Qt::AlignLeft);
}

int PackageView::selectionCount() const
{
    return selectionModel()->selectedRows().count();
}

void PackageView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    if (previous.row() != -1 && current.isValid()) {
        emit currentPackageChanged(current);
    }
    QAbstractItemView::currentChanged(current, previous);
}

void PackageView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    QTreeView::selectionChanged(selected, deselected);

    const int count = selectionCount();
    if (count <= 0) {
        emit selectionEmpty();
        return;
    }

    if (!selected.indexes().isEmpty()) {
        emit currentPackageChanged(selected.indexes().first());
        if(count > 1) {
          emit selectionMulti();
        }
    }
}

void PackageView::updateView()
{
    QModelIndex oldIndex = currentIndex();
    reset();
    setCurrentIndex(oldIndex);
}
