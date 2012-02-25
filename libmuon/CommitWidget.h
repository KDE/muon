/***************************************************************************
 *   Copyright © 2010 Jonathan Thomas <echidnaman@kubuntu.org>             *
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

#ifndef COMMITWIDGET_H
#define COMMITWIDGET_H

#include <QtGui/QWidget>

#include "libmuonprivate_export.h"

class QLabel;
class QProgressBar;

namespace DebconfKde
{
    class DebconfGui;
}

class MUONPRIVATE_EXPORT CommitWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CommitWidget(QWidget *parent = 0);
    ~CommitWidget();

    void setHeaderText(const QString &text);
    void setLabelText(const QString &text);
    void setProgress(int percentage);
    void clear();

private:
    QLabel *m_headerLabel;
    DebconfKde::DebconfGui *m_debconfGui;
    QLabel *m_commitLabel;
    QProgressBar *m_progressBar;

public Q_SLOTS:
    void updateCommitProgress(const QString &message, int percentage);
};

#endif