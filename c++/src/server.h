#include <QtNetwork>
#include <QTcpSocket>
#include <QObject>
#include <QDateTime>
#include <QTimer>

#include "common/Logger.h"
#include "common/parser/parsTools.h"
#include "common/proto/message.pb.h"

class Server : public QObject
{
    Q_OBJECT
public:
    Server(int port = 33333, std::shared_ptr<Logger> logger = nullptr, QObject *parent = nullptr);

    ~Server();

public slots:
    void newConnection();

    void read();

    void write(QByteArray msg, int idusersocs);

    void onFastResponse(std::shared_ptr<const TestTask::Messages::WrapperMessage> msg, int idusersocs);
    void onSlowResponse(std::shared_ptr<const TestTask::Messages::WrapperMessage> msg, int idusersocs);

signals:
    void readyToWrite(QByteArray msg, int idusersocs);
    void fastResponse(std::shared_ptr<const TestTask::Messages::WrapperMessage> msg, int idusersocs);
    void slowResponse(std::shared_ptr<const TestTask::Messages::WrapperMessage> msg, int idusersocs);

private:
    int port;
    std::shared_ptr<Logger> logger;
    QTcpServer *server;
    int server_status;
    int connectionCount;
    QMap<int, QTcpSocket *> Sockets;
    QMap<int, QTimer *> timers;
    QMap<int, DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> *> parsers; 
};