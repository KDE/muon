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

#include "UbuntuPurchasePage.h"

UbuntuPurchasePage::UbuntuPurchasePage(QWidget *parent)
    : KWebPage(parent)
{
}

void UbuntuPurchasePage::javaScriptAlert(QWebFrame *originatingFrame, const QString &msg)
{
    // Ubuntu's webpage that we embed uses a JavaScript alert to communicate
    // purchase response details in JSON.
    Q_UNUSED(originatingFrame);

    emit purchaseResponse(msg);
}

void UbuntuPurchasePage::javaScriptConsoleMessage(const QString &message, int lineNumber, const QString &sourceID)
{
    // Ubuntu's webpage sends us the OAuth token via a JavaScript console message
    Q_UNUSED(lineNumber);
    Q_UNUSED(sourceID);

    emit receivedOAuthToken(message);
}
