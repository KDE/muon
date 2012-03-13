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

#ifndef UPDATEITEM_H
#define UPDATEITEM_H

// Qt includes
#include <QtCore/QList>
#include <QtCore/QString>

#include <KIcon>

class Application;

class UpdateItem
{
public:
    enum ItemType {
        InvalidItem = 0,
        RootItem,
        CategoryItem,
        ApplicationItem
    };

    UpdateItem();
    UpdateItem(const QString &categoryName,
               const KIcon &categoryIcon);
    explicit UpdateItem(Application *app, UpdateItem *parent = 0);

    ~UpdateItem();

    UpdateItem *parent() const;
    void setParent(UpdateItem *parent);

    void appendChild(UpdateItem *child);
    bool removeChildren(int position, int count);
    QList<UpdateItem *> children() const;
    UpdateItem *child(int row) const;
    int childCount() const;
    int row() const;

    Application *app() const;
    QString name() const;
    KIcon icon() const;
    qint64 size() const;
    Qt::CheckState checked() const;
    ItemType type() const;

private:
    Application *m_app;

    UpdateItem *m_parent;
    QList<UpdateItem *> m_children;
    QString m_categoryName;
    KIcon m_categoryIcon;
};

#endif // UPDATEITEM_H