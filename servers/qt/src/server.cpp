#include "server.h"

Server::Server(QObject *parent) : QObject(parent)
{
    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(newUser()));
    connect(this, &Server::readyToWrite, this, &Server::write);
    if (!server->listen(QHostAddress::Any, 33333) && server_status == 0)
    {
        qDebug() << QObject::tr("Unable to start the server: %1.").arg(server->errorString());
    }
    else
    {
        server_status = 1;
        qDebug() << QString::fromUtf8("Сервер запущен!");
    }
}

Server::~Server()
{
    delete server;
}

void Server::newUser()
{
    if (server_status == 1)
    {
        qDebug() << QString::fromUtf8("У нас новое соединение!");
        QTcpSocket *clientSocket = server->nextPendingConnection();
        int idusersocs = clientSocket->socketDescriptor();
        Sockets[idusersocs] = clientSocket;
        connect(Sockets[idusersocs], SIGNAL(readyRead()), this, SLOT(read()));
    }
}

void Server::read()
{
    QTcpSocket *clientSocket = (QTcpSocket *)sender();
    int idusersocs = clientSocket->socketDescriptor();
    QByteArray message = clientSocket->readAll();
    DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> parser;
    auto lst = parser.parse(message.toStdString());
    for (auto i : lst)
    {
        qDebug() << "fast response" << i->has_fast_response();
        qDebug() << "slow response" << i->has_slow_response();
        qDebug() << "fast request" << i->has_request_for_fast_response();
        qDebug() << "slow request" << i->has_request_for_slow_response();
        qDebug() << "---------";
    }
    emit readyToWrite("test", idusersocs);
}

void Server::write(QByteArray msg, int idusersocs)
{
    qDebug() << "send";
    Sockets[idusersocs]->write(msg);
    qDebug() << "sended";
}