#include "server.h"

const char timeFormat[] = "yyyyMMddThhmmss.zzz";

Server::Server(QObject *parent) : QObject(parent)
{
    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    connect(this, &Server::readyFastResponse, this, &Server::fastResponse);
    connect(this, &Server::readySlowResponse, this, &Server::slowResponse);
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

void Server::newConnection()
{
    if (server_status == 1)
    {
        qDebug() << QString::fromUtf8("У нас новое соединение!");
        QTcpSocket *clientSocket = server->nextPendingConnection();
        int idusersocs = clientSocket->socketDescriptor();

        parsers[idusersocs] = new DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage>();
        Sockets[idusersocs] = clientSocket;
        timers[idusersocs] = nullptr;
        connect(Sockets[idusersocs], SIGNAL(readyRead()), this, SLOT(read()));
    }
}

void Server::read()
{
    QTcpSocket *clientSocket = (QTcpSocket *)sender();
    int idusersocs = clientSocket->socketDescriptor();
    QByteArray message = clientSocket->readAll();

    foreach (const char byte, message)
    {
        const auto &parsedMessages = parsers[idusersocs]->parse(std::string(1, byte));
        for (const auto &value : parsedMessages)
        {
            if (value->has_request_for_fast_response())
            {
                emit readyFastResponse(value, idusersocs);
            }
            else if (value->has_request_for_slow_response())
            {
                emit readySlowResponse(value, idusersocs);
            }
        }
    }
}

void Server::fastResponse(std::shared_ptr<const TestTask::Messages::WrapperMessage> msg, int idusersocs)
{
    if (timers[idusersocs] != nullptr)
    {
        if (timers[idusersocs]->isActive())
        {
            connect(&*timers[idusersocs], &QTimer::timeout, [this, msg, idusersocs]()
                    { fastResponse(msg, idusersocs); });
            return;
        }
    }

    TestTask::Messages::WrapperMessage response;
    qDebug() << QDateTime::currentDateTime().toString(timeFormat);
    *response.mutable_fast_response()->mutable_current_date_time() = QDateTime::currentDateTime().toString(timeFormat).toStdString();
    emit readyToWrite(serializeDelimited<TestTask::Messages::WrapperMessage>(response)->data(), idusersocs);
}

void Server::slowResponse(std::shared_ptr<const TestTask::Messages::WrapperMessage> msg, int idusersocs)
{
    if (timers[idusersocs] != nullptr)
    {
        if (timers[idusersocs]->isActive())
        {
            connect(&*timers[idusersocs], &QTimer::timeout, [this, msg, idusersocs]()
                    { slowResponse(msg, idusersocs); });
            return;
        }
    }
    timers[idusersocs] = new QTimer();
    timers[idusersocs]->setSingleShot(true);
    timers[idusersocs]->setInterval(msg->request_for_slow_response().time_in_seconds_to_sleep() * 1000);
    connect(&*timers[idusersocs], &QTimer::timeout, [this, msg, idusersocs]()
    {
        qDebug() << "Таймер отработал " << idusersocs;
        timers[idusersocs]->deleteLater();
        TestTask::Messages::WrapperMessage response;
        response.mutable_slow_response()->set_connected_client_count(idusersocs);
        emit readyToWrite(serializeDelimited<TestTask::Messages::WrapperMessage>(response)->data(), idusersocs); 
    });
    timers[idusersocs]->start();
}

void Server::write(QByteArray msg, int idusersocs)
{
    qDebug() << "send";
    Sockets[idusersocs]->write(msg);
    qDebug() << "sended";
}