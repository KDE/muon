/***************************************************************************
 *   Copyright © 2012 Aleix Pol Gonzalez <aleixpol@blue-systems.com>       *
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

#ifndef CATEGORYMODEL_H
#define CATEGORYMODEL_H

#include <QtGui/QStandardItemModel>

#include "libmuonprivate_export.h"

class Category;

class MUONPRIVATE_EXPORT CategoryModel : public QStandardItemModel
{
    Q_OBJECT
    Q_PROPERTY(Category* displayedCategory READ displayedCategory WRITE setDisplayedCategory)
    public:
        enum CategoryModelRole {
            CategoryTypeRole = Qt::UserRole + 1,
            AndOrFilterRole = Qt::UserRole + 2,
            NotFilterRole = Qt::UserRole + 3,
            CategoryRole
        };

        enum CatViewType {
            /// An invalid type
            InvalidType = 0,
            /// An AppView since there are no sub-cats
            CategoryType = 1,
            /// A SubCategoryView
            SubCatType = 2
        };
        Q_ENUMS(CatViewType);

        explicit CategoryModel(QObject* parent = 0);

        Category* categoryForRow(int row);

        void setDisplayedCategory(Category* c);
        Category* displayedCategory() const;

        Q_SCRIPTABLE static Category* findCategoryByName(const QString& name);
    private:
        void setCategories(const QList<Category *> &categoryList, const QString &rootName);
        static QList<Category*> populateCategories();

        QList<Category*> m_categoryList;
        Category* m_currentCategory;
};

#endif // CATEGORYMODEL_H
