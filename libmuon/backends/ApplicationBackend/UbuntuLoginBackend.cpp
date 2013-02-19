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

#include "UbuntuLoginBackend.h"
#include <QDebug>
#include <QDBusMetaType>
#include <QApplication>
#include <QWidget>
#include <KLocalizedString>
#include "ubuntu_sso.h"
#include "LoginMetaTypes.h"

//NOTE: this is needed because the method is called register. see the xml file for more info
struct HackedComUbuntuSsoCredentialsManagementInterface : public ComUbuntuSsoCredentialsManagementInterface
{
    HackedComUbuntuSsoCredentialsManagementInterface(const QString& service, const QString& path, const QDBusConnection& connection, QObject* parent = 0)
        : ComUbuntuSsoCredentialsManagementInterface(service, path, connection, parent)
    {}
    
    inline QDBusPendingReply<> register_hack(const QString &app_name, MapString args)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(app_name) << QVariant::fromValue(args);
        return asyncCallWithArgumentList(QLatin1String("register"), argumentList);
    }
};

UbuntuLoginBackend::UbuntuLoginBackend(QObject* parent)
    : AbstractLoginBackend(parent)
{
    qDBusRegisterMetaType<MapString>();
    m_interface = new HackedComUbuntuSsoCredentialsManagementInterface( "com.ubuntu.sso", "/com/ubuntu/sso/credentials", QDBusConnection::sessionBus(), this);
    connect(m_interface, SIGNAL(CredentialsError(QString,MapString)), SLOT(credentialsError(QString,MapString)));
    connect(m_interface, SIGNAL(AuthorizationDenied(QString)), SLOT(authorizationDenied(QString)));
    connect(m_interface, SIGNAL(CredentialsFound(QString,MapString)), this, SLOT(successfulLogin(QString,MapString)));
    
    m_interface->find_credentials(appname(), MapString());
}

void UbuntuLoginBackend::login()
{
    MapString data;
    data["help_text"] = i18n("Log in to the Ubuntu SSO service");
    data["window_id"] = winId();
    QDBusPendingReply< void > ret = m_interface->login(appname(), data);
}

void UbuntuLoginBackend::registerAndLogin()
{
    MapString data;
    data["help_text"] = i18n("Log in to the Ubuntu SSO service");
    data["window_id"] = winId();
    m_interface->register_hack(appname(), data);
}

QString UbuntuLoginBackend::displayName() const
{
    return m_credentials["name"];
}

bool UbuntuLoginBackend::hasCredentials() const
{
    return !m_credentials.isEmpty();
}

void UbuntuLoginBackend::successfulLogin(const QString& app, const QMap<QString,QString>& credentials)
{
//     qDebug() << "logged in" << appname() << app << credentials;
    if(app==appname()) {
        m_credentials = credentials;
        emit connectionStateChanged();
    }
}

QString UbuntuLoginBackend::appname() const
{
    return QCoreApplication::instance()->applicationName();
}

QString UbuntuLoginBackend::winId() const
{
    QString windowId;
    QApplication *app = qobject_cast<QApplication*>(qApp);

    if (app->activeWindow())
        windowId = QString::number(app->activeWindow()->winId());

    return windowId;
}

void UbuntuLoginBackend::authorizationDenied(const QString& app)
{
    qDebug() << "denied" << app;
    if(app==appname())
        emit connectionStateChanged();
}

void UbuntuLoginBackend::credentialsError(const QString& app, const MapString& a)
{
    //TODO: provide error message?
    qDebug() << "error" << app << a;
    if(app==appname())
        emit connectionStateChanged();
}

void UbuntuLoginBackend::logout()
{
    m_interface->clear_credentials(appname(), MapString());
    m_credentials.clear();
    emit connectionStateChanged();
}

QByteArray UbuntuLoginBackend::token() const
{
    return m_credentials["token"].toLatin1();
}

QByteArray UbuntuLoginBackend::tokenSecret() const
{
    return m_credentials["token_secret"].toLatin1();
}

QByteArray UbuntuLoginBackend::consumerKey() const
{
    return m_credentials["consumer_key"].toLatin1();
}

QByteArray UbuntuLoginBackend::consumerSecret() const
{
    return m_credentials["consumer_secret"].toLatin1();
}
