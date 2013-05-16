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

#ifndef UBUNTUPURCHASEPAGE_H
#define UBUNTUPURCHASEPAGE_H

#include <KWebPage>

/**
 * @brief The UbuntuPurchasePage class subclasses KWebPage so that it can
 * reimplement javaScriptAlert in order to emit the scripting alert rather
 * than showing it in a window. Ubuntu's app purchase webpage sends its
 * purchase response details via a scripting alert. Additionally, it sends
 * OAuth tokens that we need via the JavaScript console
 */
class UbuntuPurchasePage : public KWebPage
{
    Q_OBJECT
public:
    explicit UbuntuPurchasePage(QWidget *parent = nullptr);

protected:
    void javaScriptAlert(QWebFrame *originatingFrame, const QString &msg) override;
    void javaScriptConsoleMessage(const QString &message, int lineNumber, const QString &sourceID) override;

signals:
    void purchaseResponse(const QString &json);
    void receivedOAuthToken(const QString &json);
};

#endif // UBUNTUPURCHASEPAGE_H
