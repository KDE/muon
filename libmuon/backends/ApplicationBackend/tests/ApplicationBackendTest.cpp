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

#include "ApplicationBackendTest.h"
#include <QStringList>
#include <QAction>
#include <QFile>
#include <KProtocolManager>
#include <KStandardDirs>
#include <KActionCollection>
#include <qtest_kde.h>

#include "modeltest.h"
#include <ApplicationBackend.h>
#include <resources/ResourcesModel.h>
#include <resources/ResourcesProxyModel.h>
#include <resources/AbstractBackendUpdater.h>
#include <Category/Category.h>
#include <Category/CategoryModel.h>
#include <MuonBackendsFactory.h>
#include <MuonMainWindow.h>
#include <QAptActions.h>

QTEST_KDEMAIN( ApplicationBackendTest, GUI )

QString getCodename(const QString& value)
{
    QString ret;
    QFile f("/etc/os-release");
    if(f.open(QIODevice::ReadOnly|QIODevice::Text)){
	QRegExp rx(QString("%1=(.+)\n").arg(value));
	while(!f.atEnd()) {
	    QByteArray line = f.readLine();
	    if(rx.exactMatch(line)) {
		ret = rx.cap(1);
		break;
	    }
	}
    }
    return ret;
}

AbstractResourcesBackend* backendByName(ResourcesModel* m, const QString& name)
{
    QVector<AbstractResourcesBackend*> backends = m->backends();
    foreach(AbstractResourcesBackend* backend, backends) {
        if(backend->metaObject()->className()==name) {
            return backend;
        }
    }
    return nullptr;
}

ApplicationBackendTest::ApplicationBackendTest()
{
    QString ratingsDir = KStandardDirs::locateLocal("data","libmuon/ratings.txt");
    QFile testRatings("~/.kde-unit-test/share/apps/libmuon/ratings.txt");
    QFile ratings(ratingsDir);
    QString codeName = getCodename("ID");
    if(!testRatings.exists()){
	if(ratings.exists()){
	    ratings.copy(testRatings.fileName());
	}
	else{
	    ratings.close();
	    if(codeName.toLower() == QLatin1String("ubuntu")){
		ratingsDir = KStandardDirs::locateLocal("data","libmuon/rnrtestratings.txt");
	    }else{
		ratingsDir = KStandardDirs::locateLocal("data","libmuon/popcontestratings.txt");
	    }
	    ratings.setFileName(ratingsDir);
	    if(ratings.exists()){
		ratings.copy(testRatings.fileName());
	    }
	}
	testRatings.close();
	ratings.close();
    }
    ResourcesModel* m = new ResourcesModel("muon-appsbackend", this);
    m_window = new MuonMainWindow;
    m->integrateMainWindow(m_window);
    new ModelTest(m,m);
    m_appBackend = backendByName(m, "ApplicationBackend");
    QVERIFY(m_appBackend); //TODO: test all backends
    QTest::kWaitForSignal(m, SIGNAL(allInitialized()));
}

ApplicationBackendTest::~ApplicationBackendTest()
{
    delete m_window;
}

void ApplicationBackendTest::testReload()
{
    ResourcesModel* model = ResourcesModel::global();
    QVector<AbstractResource*> apps = m_appBackend->allResources();
    QCOMPARE(apps.count(), model->rowCount());
    
    QVector<QVariant> appNames(apps.size());
    for(int i=0; i<model->rowCount(); ++i) {
        AbstractResource* app = apps[i];
        appNames[i]=app->property("packageName");
    }
    
    bool b = QMetaObject::invokeMethod(m_appBackend, "reload");
    Q_ASSERT(b);
    m_appBackend->updatesCount();
    QCOMPARE(apps, m_appBackend->allResources() );
    
    QVERIFY(!apps.isEmpty());
    QCOMPARE(apps.count(), model->rowCount());
    
    for(int i=0; i<model->rowCount(); ++i) {
        AbstractResource* app = apps[i];
        QCOMPARE(appNames[i], app->property("packageName"));
//         QCOMPARE(m_model->data(m_model->index(i), ResourcesModel::NameRole).toString(), app->name());
    }
}

void ApplicationBackendTest::testCategories()
{
    ResourcesModel* m = ResourcesModel::global();
    ResourcesProxyModel* proxy = new ResourcesProxyModel(m);
    proxy->setSourceModel(m);
    CategoryModel* categoryModel = new CategoryModel(proxy);
    categoryModel->setDisplayedCategory(nullptr);
    for(int i=0; i<categoryModel->rowCount(); ++i) {
        Category* cat = categoryModel->categoryForRow(i);
        proxy->setFiltersFromCategory(cat);
        qDebug() << "fuuuuuu" << proxy->rowCount() << cat->name();
    }
}

void ApplicationBackendTest::testRefreshUpdates()
{
    ResourcesModel* m = ResourcesModel::global();
    AbstractBackendUpdater* updater = m_appBackend->backendUpdater();

    QSignalSpy spy(m, SIGNAL(fetchingChanged()));
    QAptActions::self()->actionCollection()->action("update")->trigger();
//     QTest::kWaitForSignal(m, SLOT(fetchingChanged()));
    QVERIFY(!m->isFetching());
    qDebug() << spy.count();
}
