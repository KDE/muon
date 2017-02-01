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

#include "PackageViewHeader.h"

#include <QAction>
#include <QMenu>
#include <QContextMenuEvent>

#include "PackageProxyModel.h"

PackageViewHeader::PackageViewHeader(QWidget *parent)
    : QHeaderView(Qt::Horizontal, parent)
{
}

void PackageViewHeader::setModel(QAbstractItemModel* model)
{
    QAbstractItemModel* currentModel = this->model();
    if (model == currentModel)
        return;
    if (currentModel) {
        disconnect(currentModel, &QAbstractItemModel::layoutChanged,
                   this, &PackageViewHeader::modelLayoutChanged);
    }

    QHeaderView::setModel(model);

    connect(model, &QAbstractItemModel::layoutChanged,
            this, &PackageViewHeader::modelLayoutChanged);
}


void PackageViewHeader::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    createActions();
    foreach (QAction *action, m_columnActions) {
        menu.addAction(action);
    }
    menu.exec(event->globalPos());
    deleteActions();
}

void PackageViewHeader::createActions()
{
    QAbstractItemModel *m = model();
    // first 3 columns (0-2) are always shown
    for(int i = 3; i < count(); ++i) {
        QAction *action = new QAction(m->headerData(i, orientation()).toString(), this);
        action->setCheckable(true);
        action->setChecked(!isSectionHidden(i));
        action->setData(i);
        connect(action, SIGNAL(toggled(bool)), this, SLOT(toggleColumn(bool)));
        m_columnActions.append(action);
    }
}

void PackageViewHeader::deleteActions()
{
    while (!m_columnActions.isEmpty()) {
        QAction *action = m_columnActions.takeFirst();
        disconnect(action, SIGNAL(toggled(bool)), this, SLOT(toggleColumn(bool)));
        delete action;
    }
}

void PackageViewHeader::modelLayoutChanged()
{
    setSortIndicatorShown(!(static_cast<PackageProxyModel*>(model())->isSortedByRelevancy() &&
        sortIndicatorSection() == 0));
}

void PackageViewHeader::toggleColumn(bool visible)
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        int column = action->data().toInt();
        setSectionHidden(column, !visible);
    }
}
