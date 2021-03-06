/**************************************************************************
**
** This file is part of QSsh
**
** Copyright (c) 2012 LVK
**
** Contact: andres.pagliano@lvklabs.com
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
**************************************************************************/

#include "securefileuploader.h"

#include <QtDebug>
#include <QFileInfo>
#include "sftpdefs.h"
SecureFileUploader::SecureFileUploader(QObject *parent) :
    QObject(parent), m_connection(0)
{
    qDebug()<<"SecureFileUploaderID";
}

void SecureFileUploader::upload(const QString &localFile, const QString &dest, const QString &host,
                                const QString &username, const QString &passwd)
{
    QFileInfo info(localFile);

    m_localFilename = info.canonicalFilePath();
    m_remoteFilename = dest + QLatin1Char('/') + info.fileName();

    QSsh::SshConnectionParameters params;
    params.setHost(host);
    params.setUserName(username);
    params.setPassword(passwd);
    params.authenticationType = QSsh::SshConnectionParameters::AuthenticationTypeTryAllPasswordBasedMethods;
    params.timeout = 30;
    params.setPort(22);

    m_connection = new QSsh::SshConnection(params, this); // TODO free this pointer!

    connect(m_connection, SIGNAL(connected()), SLOT(onConnected()));
    connect(m_connection, SIGNAL(error(QSsh::SshError)), SLOT(onConnectionError(QSsh::SshError)));

    qDebug() << "SecureUploader: Connecting to host" << host;

    m_connection->connectToHost();
}

void SecureFileUploader::onConnected()
{
    qDebug() << "SecureUploader: Connected";
    qDebug() << "SecureUploader: Creating SFTP channel...";

    m_channel = m_connection->createSftpChannel();

    if (m_channel) {
        connect(m_channel.data(), SIGNAL(initialized()),
                SLOT(onChannelInitialized()));
        connect(m_channel.data(), SIGNAL(channelError(QString)),
                SLOT(onChannelError(QString)));
        connect(m_channel.data(), SIGNAL(finished(QSsh::SftpJobId job, const QSsh::SftpError, const QString &)),
                SLOT(onOpfinished(QSsh::SftpJobId job, const QSsh::SftpError, const QString &)));
        //void transferProgress(QSsh::SftpJobId job, quint64 progress, quint64 total);
        connect(m_channel.data(), SIGNAL(transferProgress(QSsh::SftpJobId, quint64, quint64)),
                SLOT(onTransferProgress(QSsh::SftpJobId, quint64, quint64)));
        m_channel->initialize();

    } else {
        qDebug() << "SecureUploader: Error null channel";
    }
}

void SecureFileUploader::onConnectionError(QSsh::SshError err)
{
    qDebug() << "SecureUploader: Connection error" << err;
    emit SftpEndcomplete();
}

void SecureFileUploader::onChannelInitialized()
{
    qDebug() << "SecureUploader: Channel Initialized";

    // TODO Choose the overwrite mode: SftpOverwriteExisting, SftpAppendToExisting, SftpSkipExisting
    QSsh::SftpJobId job = m_channel->uploadFile(m_localFilename, m_remoteFilename,
                                                QSsh::SftpOverwriteExisting);

    if (job != QSsh::SftpInvalidJob) {
        qDebug() << "SecureUploader: Starting job #" << job;
    } else {
        qDebug() << "SecureUploader: Invalid Job";
    }
}

void SecureFileUploader::onChannelError(const QString &err)
{
    qDebug() << "SecureUploader: Error: " << err;
     emit SftpEndcomplete();
}

void SecureFileUploader::onOpfinished(QSsh::SftpJobId job, const QSsh::SftpError errorType, const QString &err)
{
    qDebug() << "SecureUploader: Finished job #" << job << ":" << (err.isEmpty() ? QStringLiteral("OK") : err);
    emit SftpEndcomplete();
}

void SecureFileUploader::onTransferProgress(QSsh::SftpJobId job, quint64 progress, quint64 total)
{
    qDebug() << "SecureUploader: progresss"<<progress<<"total"<<total;

    if(progress>=total)
        emit SftpEndcomplete();
}





