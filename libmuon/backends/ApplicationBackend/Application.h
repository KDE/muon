/***************************************************************************
 *   Copyright © 2010-2011 Jonathan Thomas <echidnaman@kubuntu.org>        *
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

#ifndef APPLICATION_H
#define APPLICATION_H

#include <QtCore/QByteArray>
#include <QtCore/QHash>
#include <QtCore/QStringList>

#include <KUrl>
#include <KService>

#include <LibQApt/Package>

#include "QAptResource.h"

class KJob;
namespace QApt {
    class Backend;
}

class Application : public QAptResource
{
Q_OBJECT
// Q_PROPERTY(QString mimetypes READ mimetypes CONSTANT)
Q_PROPERTY(QString menuPath READ menuPath CONSTANT)
public:
    explicit Application(const QString &fileName, QApt::Backend *backend);

    QString name();
    QString untranslatedName();
    QString comment();
    QApt::Package *package();
    QString icon() const;
    QString mimetypes() const;
    QString menuPath();
    QString categories();
    QString license();
    QUrl screenshotUrl();
    QUrl thumbnailUrl();
    bool isValid() const;
    bool isTechnical() const;
    QString packageName() const;

    //QApt::Package forwarding
    QUrl homepage() const;
    QString longDescription() const;
    QString installedVersion() const;
    QString availableVersion() const;
    QString sizeDescription();
    QString origin() const;
    int downloadSize();

    bool hasScreenshot() const { return m_sourceHasScreenshot; }
    void setHasScreenshot(bool has);

    void clearPackage();
    QVector<KService::Ptr> findExecutables() const;
    virtual QStringList executables() const;
    
    /** Used to trigger the stateChanged signal from the ApplicationBackend */
    void emitStateChanged();
    
    void invokeApplication() const;
    
    bool canExecute() const;
    QString section();
    
    State state();
    void fetchScreenshots();
    
    bool isSecure() const;

private:
    KSharedConfigPtr m_data;
    QByteArray m_packageName;

    bool m_isValid;
    bool m_isTechnical;
    bool m_sourceHasScreenshot;

    QByteArray getField(const char* field, const QByteArray& defaultvalue = QByteArray()) const;
    KSharedConfigPtr desktopContents(const QString& filename);
    QVector<QPair<QString, QString> > locateApplication(const QString &_relPath, const QString &menuId) const;
    bool hasField(const char* field) const;
};

#endif
