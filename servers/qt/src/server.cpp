#include "server.h"

const char timeFormat[] = "yyyyMMddThhmmss.zzz";

Server::Server(int port, std::shared_ptr<Logger> logger, QObject *parent) 
    : port(port), logger(std::move(logger))
{
    server = new QTcpServer(this);
    connectionCount = 0;
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    connect(this, &Server::fastResponse, this, &Server::onFastResponse);
    connect(this, &Server::slowResponse, this, &Server::onSlowResponse);
    connect(this, &Server::readyToWrite, this, &Server::write);
    if (!server->listen(QHostAddress::Any, port) && server_status == 0)
    {
        //qDebug() << QObject::tr("Unable to start the server: %1.").arg(server->errorString());
    }
    else
    {
        server_status = 1;
        //qDebug() << QString::fromUtf8("Сервер запущен!");
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
        qDebug() << "new connection";
        connectionCount++;
        QTcpSocket *clientSocket = server->nextPendingConnection();
        int idusersocs = clientSocket->socketDescriptor();

        logger->log(Level::LOG_INFO, "New connection id: " + QString::number(idusersocs));

        parsers[idusersocs] = new DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage>();
        Sockets[idusersocs] = clientSocket;
        timers[idusersocs] = nullptr;
        connect(clientSocket, &QTcpSocket::disconnected, this, [this, idusersocs]()
        {
            connectionCount--;
            logger->log(Level::LOG_INFO, "Disconnected id: " + QString::number(idusersocs));
            delete parsers[idusersocs];
        });
        connect(Sockets[idusersocs], SIGNAL(readyRead()), this, SLOT(read()));
    }
}

void Server::read()
{
    QTcpSocket *clientSocket = (QTcpSocket *)sender();
    int idusersocs = clientSocket->socketDescriptor();
    QByteArray message = clientSocket->readAll();
    //qDebug() << message;
    foreach (const char byte, message)
    {
        const auto &parsedMessages = parsers[idusersocs]->parse(std::string(1, byte));
        for (const auto &value : parsedMessages)
        {
            if (value->has_request_for_fast_response())
            {
                emit fastResponse(value, idusersocs);
            }
            else if (value->has_request_for_slow_response())
            {
                emit slowResponse(value, idusersocs);
            }
        }
    }
}

void Server::onFastResponse(std::shared_ptr<const TestTask::Messages::WrapperMessage> msg, int idusersocs)
{
    if (timers[idusersocs] != nullptr)
    {
        if (timers[idusersocs]->isActive())
        {
            connect(&*timers[idusersocs], &QTimer::timeout, [this, msg, idusersocs]()
                    { onFastResponse(msg, idusersocs); });
            return;
        }
    }

    TestTask::Messages::WrapperMessage response;
    //qDebug() << QDateTime::currentDateTime().toString(timeFormat);
    *response.mutable_fast_response()->mutable_current_date_time() = QDateTime::currentDateTime().toString(timeFormat).toStdString();
    auto serializedData = serializeDelimited<TestTask::Messages::WrapperMessage>(response);
    emit readyToWrite(QByteArray(serializedData->data(), serializedData->size()), idusersocs);
}

void Server::onSlowResponse(std::shared_ptr<const TestTask::Messages::WrapperMessage> msg, int idusersocs)
{
    if (timers[idusersocs] != nullptr)
    {
        if (timers[idusersocs]->isActive())
        {
            connect(&*timers[idusersocs], &QTimer::timeout, [this, msg, idusersocs]()
                    { onSlowResponse(msg, idusersocs); });
            return;
        }else
        {
            timers[idusersocs]->deleteLater();
        }
    }
    timers[idusersocs] = new QTimer();
    timers[idusersocs]->setSingleShot(true);
    timers[idusersocs]->setInterval(msg->request_for_slow_response().time_in_seconds_to_sleep());
    int connectionCount = this->connectionCount; 
    connect(&*timers[idusersocs], &QTimer::timeout, [this, msg, idusersocs, connectionCount]()
    {
        //qDebug() << "Таймер отработал " << idusersocs;
        TestTask::Messages::WrapperMessage response;
        response.mutable_slow_response()->set_connected_client_count(connectionCount);
        auto serializedData = serializeDelimited<TestTask::Messages::WrapperMessage>(response);
        emit readyToWrite(QByteArray(serializedData->data(), serializedData->size()), idusersocs);
    });
    timers[idusersocs]->start();
}

void Server::write(QByteArray msg, int idusersocs)
{
    //qDebug() << "send";
    //qDebug() << msg;
    Sockets[idusersocs]->write(msg);
    //qDebug() << "sended";
}