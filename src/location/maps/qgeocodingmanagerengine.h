/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGEOCODINGMANAGERENGINE_H
#define QGEOCODINGMANAGERENGINE_H

#include "qgeocodingmanager.h"
#include "qgeocodereply.h"
#include "qgeoboundingbox.h"

#include <QObject>
#include <QList>

QT_BEGIN_NAMESPACE

class QGeocodingManagerEnginePrivate;

class Q_LOCATION_EXPORT QGeocodingManagerEngine : public QObject
{
    Q_OBJECT
public:
    QGeocodingManagerEngine(const QMap<QString, QVariant> &parameters, QObject *parent = 0);
    virtual ~QGeocodingManagerEngine();

    QString managerName() const;
    int managerVersion() const;

    virtual QGeocodeReply* geocode(const QGeoAddress &address,
                                     QGeoBoundingArea *bounds);
    virtual QGeocodeReply* geocode(const QString &address,
                                    int limit,
                                    int offset,
                                    QGeoBoundingArea *bounds);
    virtual QGeocodeReply* reverseGeocode(const QGeoCoordinate &coordinate,
                                            QGeoBoundingArea *bounds);



    bool supportsGeocoding() const;
    bool supportsReverseGeocoding() const;

    void setLocale(const QLocale &locale);
    QLocale locale() const;

Q_SIGNALS:
    void finished(QGeocodeReply* reply);
    void error(QGeocodeReply* reply, QGeocodeReply::Error error, QString errorString = QString());

protected:
    void setSupportsGeocoding(bool supported);
    void setSupportsReverseGeocoding(bool supported);

private:
    void setManagerName(const QString &managerName);
    void setManagerVersion(int managerVersion);

    QGeocodingManagerEnginePrivate *d_ptr;
    Q_DISABLE_COPY(QGeocodingManagerEngine)

    friend class QGeoServiceProvider;
};

QT_END_NAMESPACE

#endif