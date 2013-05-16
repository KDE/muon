/***************************************************************************
 *   Copyright © 2013 Jonathan Thomas <echidnaman@kubuntu.org>             *
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

#ifndef UBUNTUPURCHASEDIALOG_H
#define UBUNTUPURCHASEDIALOG_H

#include <QWidget>

// Qt includes
#include <QMap>

class QUrl;

class KPixmapSequenceOverlayPainter;
class KWebView;

class ApplicationBackend;
class USCResource;

class UbuntuPurchaseDialog : public QWidget
{
    Q_OBJECT
public:
    explicit UbuntuPurchaseDialog(QWidget *parent = nullptr);

private:
    // Data
    USCResource *m_resource;
    QMap<QString, QVariant> m_oauthToken;

    // Widgets
    KWebView *m_webView;
    KPixmapSequenceOverlayPainter *m_spinner;
    
signals:
    void purchaseCancelledByUser();
    void purchaseFailed();
    void purchaseSucceeded();
    void termsOfServiceDeclined();

public slots:
    void startPurchase(USCResource *res, const QUrl &url);
    
private slots:
    void parseJson(const QString &json);
    void onReceivedOAuthToken(const QString &json);
};

#endif // UBUNTUPURCHASEDIALOG_H
