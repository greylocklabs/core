#pragma once

#include <QByteArray>
#include <QHostAddress>
#include <QList>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUrl>

#include <utility>

/// A loopback HTTP/1.1 server that records each request's target and answers every one
/// with the same canned JSON.
class FakeHttpServer
{
public:
    FakeHttpServer()
    {
        m_server.listen(QHostAddress::LocalHost, 0);
        QObject::connect(&m_server, &QTcpServer::newConnection, &m_server, [this] {
            while (QTcpSocket *socket = m_server.nextPendingConnection())
                serve(socket);
        });
    }

    [[nodiscard]] QUrl url() const
    {
        return QUrl(QStringLiteral("http://127.0.0.1:%1").arg(m_server.serverPort()));
    }

    void respond(int status, QByteArray json)
    {
        m_status = status;
        m_body = std::move(json);
    }

    QList<QByteArray> targets;

private:
    void serve(QTcpSocket *socket)
    {
        auto *buffer = new QByteArray;
        QObject::connect(socket, &QTcpSocket::disconnected, socket, [socket, buffer] {
            delete buffer;
            socket->deleteLater();
        });
        QObject::connect(socket, &QTcpSocket::readyRead, socket, [this, socket, buffer] {
            buffer->append(socket->readAll());
            if (!buffer->contains("\r\n\r\n"))
                return;
            targets.append(buffer->left(buffer->indexOf('\n')).split(' ').value(1));
            socket->write("HTTP/1.1 " + QByteArray::number(m_status) +
                          " X\r\nContent-Type: application/json\r\nContent-Length: " +
                          QByteArray::number(m_body.size()) + "\r\nConnection: close\r\n\r\n" +
                          m_body);
            socket->disconnectFromHost();
        });
    }

    QTcpServer m_server;
    int m_status = 200;
    QByteArray m_body = "{}";
};
