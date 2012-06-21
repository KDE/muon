/*
 *   Copyright (C) 2012 Aleix Pol Gonzalez <aleixpol@blue-systems.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library/Lesser General Public License
 *   version 2, or (at your option) any later version, as published by the
 *   Free Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library/Lesser General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef MUONDISCOVERMAINWINDOW_H
#define MUONDISCOVERMAINWINDOW_H

#include <QtCore/QUrl>

#include <LibQApt/Globals>
#include <MuonMainWindow.h>

class Category;
class QDeclarativeView;
class ApplicationBackend;
namespace QApt { class Backend; }

class MuonDiscoverMainWindow : public MuonMainWindow
{
    Q_OBJECT
    public:
        explicit MuonDiscoverMainWindow();
        virtual ~MuonDiscoverMainWindow();

        ApplicationBackend* appBackend() const;
        QApt::Backend* backend() const;
        Q_SCRIPTABLE QAction* getAction(const QString& name);
        virtual QSize sizeHint() const;

    public slots:
        void setBackend(QApt::Backend* b);
        void openApplication(const QString& app);
        QUrl featuredSource() const;
        void openMimeType(const QString& mime);
        void openCategory(const QString& category);

    private slots:
        void triggerOpenApplication();

    signals:
        void openApplicationInternal(const QString& appname);
        void listMimeInternal(const QString& mime);
        void listCategoryInternal(Category* c);

    private:
        QString m_appToBeOpened;
        QString m_mimeToBeOpened;
        QString m_categoryToBeOpened;
        QDeclarativeView* m_view;
};

#endif // MUONINSTALLERDECLARATIVEVIEW_H