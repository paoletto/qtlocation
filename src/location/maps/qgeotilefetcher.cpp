/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtLocation module of the Qt Toolkit.
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgeomappingmanagerengine.h"
#include "qgeotilefetcher.h"
#include "qgeotilefetcher_p.h"
#include "qgeotiledmapreply.h"
#include "qgeotilespec.h"
#include "qgeotiledmapdata_p.h"

#include <QThread>
#include <QNetworkProxy>

QT_BEGIN_NAMESPACE

QGeoTileFetcher::QGeoTileFetcher(QGeoTiledMappingManagerEngine *engine, QObject *parent)
    : QObject(parent),
      d_ptr(new QGeoTileFetcherPrivate(engine))
{
    Q_D(QGeoTileFetcher);
    d->engine_ = engine;
}

QGeoTileFetcher::~QGeoTileFetcher()
{
    Q_D(QGeoTileFetcher);
    delete d;
}

void QGeoTileFetcher::threadStarted()
{
    Q_D(QGeoTileFetcher);

    if (d->stopped_)
        return;

    d->timer_ = new QTimer(this);

    d->timer_->setInterval(0);

    connect(d->timer_,
            SIGNAL(timeout()),
            this,
            SLOT(requestNextTile()));

    d->started_ = true;
    if (!d->queue_.isEmpty())
        d->timer_->start();
}

bool QGeoTileFetcher::init()
{
    return false;
}

void QGeoTileFetcher::threadFinished()
{
    Q_D(QGeoTileFetcher);
    d->stopped_ = true;
    disconnect(d->timer_);
    d->timer_->stop();
    this->deleteLater();
}

void QGeoTileFetcher::updateTileRequests(const QSet<QGeoTileSpec> &tilesAdded,
                                                  const QSet<QGeoTileSpec> &tilesRemoved)
{
    Q_D(QGeoTileFetcher);

    QMutexLocker ml(&d->queueMutex_);

    if (d->stopped_)
        return;

    cancelTileRequests(tilesRemoved);

    d->queue_ += tilesAdded.toList();

    if (!d->queue_.empty())
        d->timer_->start();
}

void QGeoTileFetcher::cancelTileRequests(const QSet<QGeoTileSpec> &tiles)
{
    Q_D(QGeoTileFetcher);

    typedef QSet<QGeoTileSpec>::const_iterator tile_iter;
    tile_iter tile = tiles.constBegin();
    tile_iter end = tiles.constEnd();
    for (; tile != end; ++tile) {
        QGeoTiledMapReply* reply = d->invmap_.value(*tile, 0);
        if (reply) {
            d->invmap_.remove(*tile);
            reply->abort();
        }
        d->queue_.removeAll(*tile);
    }

    if (d->queue_.isEmpty())
        d->timer_->stop();
}

void QGeoTileFetcher::requestNextTile()
{
    Q_D(QGeoTileFetcher);

    QMutexLocker ml(&d->queueMutex_);

    if (d->stopped_)
        return;

    if (d->queue_.isEmpty()) {
        d->timer_->stop();
        return;
    }

    QGeoTileSpec ts = d->queue_.takeFirst();

    QGeoTiledMapReply *reply = getTileImage(ts);

    if (reply->isFinished()) {
        handleReply(reply, ts);
    } else {
        connect(reply,
                SIGNAL(finished()),
                this,
                SLOT(finished()),
                Qt::QueuedConnection);

        d->invmap_.insert(ts, reply);
    }

    if (d->queue_.isEmpty())
        d->timer_->stop();
}

void QGeoTileFetcher::finished()
{
    Q_D(QGeoTileFetcher);

    QMutexLocker ml(&d->queueMutex_);

    QGeoTiledMapReply *reply = qobject_cast<QGeoTiledMapReply*>(sender());
    if (!reply)
        return;

    QGeoTileSpec spec = reply->tileSpec();

    if (!d->invmap_.contains(spec)) {
        reply->deleteLater();
        return;
    }

    d->invmap_.remove(spec);

    handleReply(reply, spec);
}

void QGeoTileFetcher::handleReply(QGeoTiledMapReply *reply, const QGeoTileSpec &spec)
{
    Q_D(QGeoTileFetcher);

    if (d->stopped_) {
        reply->deleteLater();
        return;
    }

    if (reply->error() == QGeoTiledMapReply::NoError) {
        emit tileFinished(spec, reply->mapImageData(), reply->mapImageFormat());
    } else {
        emit tileError(spec, reply->errorString());
    }

    reply->deleteLater();
}

/*******************************************************************************
*******************************************************************************/

QGeoTileFetcherPrivate::QGeoTileFetcherPrivate(QGeoTiledMappingManagerEngine *engine)
    : started_(false),
      stopped_(false),
      timer_(0),
      engine_(engine) {}

QGeoTileFetcherPrivate::~QGeoTileFetcherPrivate()
{
}

QT_END_NAMESPACE








