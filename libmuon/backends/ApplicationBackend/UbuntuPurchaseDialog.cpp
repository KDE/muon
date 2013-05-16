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

#include "UbuntuPurchaseDialog.h"

// Qt includes
#include <QVBoxLayout>
#include <QDebug>

// KDE includes
#include <KIconLoader>
#include <KLocale>
#include <KPixmapSequence>
#include <KPixmapSequenceOverlayPainter>
#include <KWebView>

// QJson includes
#include <qjson/parser.h>
#include <qjson/serializer.h>

// Own includes
#include "UbuntuPurchasePage.h"

const QString DUMMY_HTML = QString("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN "
        "http://www.w3.org/TR/html4/loose.dtd\">"
 "<html>\n"
 "<head>\n"
 "<title></title>\n"
 "</head>\n"
 "<body>\n"
  "<script type=\"text/javascript\">\n"
   "function changeTitle(title) { document.title = title; }"
   "function success() { changeTitle('{\"successful\": true, \"deb_line\": "
     "\"deb https://user:pass@private-ppa.launchpad.net/mvo/ubuntu lucid main\", "
    "\"package_name\": \"2vcard\", "
    "\"application_name\": \"The 2vcard app\", "
    "\"signing_key_id\": \"1024R/0EB12F05\""
        "}') }"
   "function cancel() { changeTitle('{ \"successful\" : false }') }"
  "</script>\n"
  "<h1>Purchase test page</h1>"
  "<p> To buy Frobunicator for 99$ you need to enter your credit card info</p>"
  "<p>  Enter your credit card info  </p>"
  "<p>"
  "<input type=\"entry\">"
  "</p>"
  "<input type=\"button\" name=\"test_button2\""
         "value=\"Cancel\""
       "onclick='cancel()'"
  "/>\n"
  "<input type=\"button\" name=\"test_button\""
         "value=\"Buy now\""
       "onclick='success()'"
  "/>"
 "</body>"
 "</html>");

const QString LOADING_HTML = QString("<html>\n"
                                     "<head>\n"
                                      "<title></title>\n"
                                     "</head>\n"
                                     "<style type=\"text/css\">\n"
                                     "html {\n"
                                       "background: #fff;\n"
                                       "color: #000;\n"
                                       "font: sans-serif;\n"
                                       "font: caption;\n"
                                       "text-align: center;\n"
                                       "position: absolute;\n"
                                       "top: 0;\n"
                                       "bottom: 0;\n"
                                       "width: 100\%;\n"
                                       "height: 100\%;\n"
                                       "display: table;\n"
                                     "}\n"
                                     "body {\n"
                                       "display: table-cell;\n"
                                       "vertical-align: middle;\n"
                                     "}\n"
                                     "h1 {\n"
                                       "center no-repeat;\n"
                                       "padding-top: 48px;\n" /* leaves room for the spinner above */
                                       "font-size: 100\%;\n"
                                       "font-weight: normal;\n"
                                     "}\n"
                                     "</style>\n"
                                     "<body>\n"
                                      "<h1>%1</h1>\n"
                                     "</body>\n"
                                     "</html>\n");

UbuntuPurchaseDialog::UbuntuPurchaseDialog(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    setLayout(layout);

    m_webView = new KWebView(this);
    layout->addWidget(m_webView);

    UbuntuPurchasePage *page = new UbuntuPurchasePage(m_webView);
    m_webView->setPage(page);

    m_spinner = new KPixmapSequenceOverlayPainter(m_webView);
    m_spinner->setSequence(KPixmapSequence("process-working", KIconLoader::SizeSmallMedium));
    m_spinner->setWidget(m_webView);
    m_spinner->start();

    // Ubuntu's webpage we embed will send us a json response
    connect(page, SIGNAL(purchaseResponse(QString)),
            this, SLOT(parseJson(QString)));
    // We use the webpage title changing to test w/o a server
    connect(m_webView, SIGNAL(titleChanged(QString)),
            this, SLOT(parseJson(QString)));
}

void UbuntuPurchaseDialog::startPurchase(USCResource *res, const QUrl &url)
{
//    if (!res)
//        return;
    m_resource = res;

    m_webView->setHtml(LOADING_HTML.arg(i18nc("@info title","Connecting to Payment Service")));
    connect(m_webView, SIGNAL(loadFinished(bool)),
            m_spinner, SLOT(stop()));

    if (url.isEmpty())
        m_webView->setHtml(DUMMY_HTML);
    else
        m_webView->load(url);
}

void UbuntuPurchaseDialog::parseJson(const QString &json)
{
    if (json.isEmpty())
        return;

    QJson::Parser parser;
    bool ok = false;
    QVariant responseData = parser.parse(json.toLocal8Bit(), &ok);

    if (!ok) {
        qWarning() << "Failure to parse purchase server reponse json!";
        return;
    }

    auto response = responseData.toMap();
    bool successful = response.value("successful").toBool();
    qDebug() << "Successful?" << successful;

    // Handle cancel or error
    if (!successful) {
        return;
    }

    // Purchase successful
    emit purchaseSucceeded();

    QString debLine = response.value("deb_line").toString();
    QString signingKeyId = response.value("signing_key_id").toString();
    QString licenseKey = response.value("license_key").toString();
    QString licenseKeyPath = response.value("license_key_path").toString();
}