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

#include "ChangelogWidget.h"
#include <resources/AbstractResource.h>

// Qt includes
#include <QtCore/QParallelAnimationGroup>
#include <QtCore/QPropertyAnimation>
#include <QtCore/QStringBuilder>
#include <QtCore/QTextStream>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>

// KDE includes
#include <KGlobal>
#include <KIO/Job>
#include <KJob>
#include <KLocale>
#include <KPixmapSequence>
#include <KPixmapSequenceOverlayPainter>
#include <KTemporaryFile>
#include <KTextBrowser>
#include <KDebug>

ChangelogWidget::ChangelogWidget(QWidget *parent)
        : QWidget(parent)
        , m_package(0)
        , m_show(false)
{
    QWidget *sideWidget = new QWidget(this);

    QToolButton *hideButton = new QToolButton(sideWidget);
    hideButton->setText(i18nc("@action:button", "Hide"));
    hideButton->setArrowType(Qt::DownArrow);
    hideButton->setAutoRaise(true);
    hideButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(hideButton, SIGNAL(clicked()), this, SLOT(animatedHide()));

    QVBoxLayout *sideLayout = new QVBoxLayout(sideWidget);
    sideLayout->setMargin(0);
    sideLayout->setSpacing(0);
    sideLayout->addWidget(hideButton);
    sideLayout->addStretch();
    sideWidget->setLayout(sideLayout);

    m_changelogBrowser = new KTextBrowser(this);
    m_changelogBrowser->setFrameShape(QFrame::NoFrame);
    m_changelogBrowser->setFrameShadow(QFrame::Plain);
    m_changelogBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget *viewport = m_changelogBrowser->viewport();
    QPalette palette = viewport->palette();
    palette.setColor(viewport->backgroundRole(), Qt::transparent);
    palette.setColor(viewport->foregroundRole(), palette.color(QPalette::WindowText));
    viewport->setPalette(palette);

    m_busyWidget = new KPixmapSequenceOverlayPainter(this);
    m_busyWidget->setSequence(KPixmapSequence("process-working", KIconLoader::SizeSmallMedium));
    m_busyWidget->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    m_busyWidget->setWidget(this);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(sideWidget);
    mainLayout->addWidget(m_changelogBrowser);

    int finalHeight = sizeHint().height();

    QPropertyAnimation *anim1 = new QPropertyAnimation(this, "maximumHeight", this);
    anim1->setDuration(500);
    anim1->setEasingCurve(QEasingCurve::OutQuart);
    anim1->setStartValue(0);
    anim1->setEndValue(finalHeight);

    QPropertyAnimation *anim2 = new QPropertyAnimation(this, "minimumHeight", this);
    anim2->setDuration(500);
    anim2->setEasingCurve(QEasingCurve::OutQuart);
    anim2->setStartValue(0);
    anim2->setEndValue(finalHeight);

    m_expandWidget = new QParallelAnimationGroup(this);
    m_expandWidget->addAnimation(anim1);
    m_expandWidget->addAnimation(anim2);
}

void ChangelogWidget::setResource(AbstractResource* package)
{
    if (m_package==package)
        return;

    if(m_package)
        disconnect(m_package, SIGNAL(changelogFetched(QString)), this, SLOT(changelogFetched(QString)));

    m_package = package;
    if (m_package) {
        connect(m_package, SIGNAL(changelogFetched(QString)), SLOT(changelogFetched(QString)));
        fetchChangelog();
    } else
        animatedHide();
}

void ChangelogWidget::show()
{
    QWidget::show();

    if (!m_show) {
        m_show = true;
        disconnect(m_expandWidget, SIGNAL(finished()), this, SLOT(hide()));
        m_expandWidget->setDirection(QAbstractAnimation::Forward);
        m_expandWidget->start();
    }
}

void ChangelogWidget::animatedHide()
{
    m_show = false;
    m_changelogBrowser->clear();

    m_expandWidget->setDirection(QAbstractAnimation::Backward);
    m_expandWidget->start();
    connect(m_expandWidget, SIGNAL(finished()), this, SLOT(hide()));
}

void ChangelogWidget::changelogFetched(const QString& changelog)
{
    if (m_package) {
        // Work around http://bugreports.qt.nokia.com/browse/QTBUG-2533 by forcibly resetting the CharFormat
        m_changelogBrowser->setCurrentCharFormat(QTextCharFormat());
        m_changelogBrowser->setHtml(changelog);
    }

    m_busyWidget->stop();
    if (!m_show) {
        animatedHide();
    }
}

void ChangelogWidget::fetchChangelog()
{
    show();
    m_changelogBrowser->clear();
    m_busyWidget->start();
    m_package->fetchChangelog();
}
