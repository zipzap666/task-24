#include <QtNetwork>
#include <QTcpSocket>
#include <QObject>
#include <QDateTime>
#include <QTimer>

#include "parser/parsTools.h"
#include "proto/message.pb.h"

class Server : public QObject
{
    Q_OBJECT
public:
    Server(QObject *parent = nullptr);

    ~Server();

public slots:
    void newConnection();

    void read();

    void write(QByteArray msg, int idusersocs);

    void fastResponse(std::shared_ptr<const TestTask::Messages::WrapperMessage> msg, int idusersocs);
    void slowResponse(std::shared_ptr<const TestTask::Messages::WrapperMessage> msg, int idusersocs);

signals:
    void readyToWrite(QByteArray msg, int idusersocs);
    void readyFastResponse(std::shared_ptr<const TestTask::Messages::WrapperMessage> msg, int idusersocs);
    void readySlowResponse(std::shared_ptr<const TestTask::Messages::WrapperMessage> msg, int idusersocs);

private:
    QTcpServer *server;
    int server_status;
    //void resetTimer(int idusersocs);
    QMap<int, QTcpSocket *> Sockets;
    QMap<int, QTimer *> timers;
    QMap<int, DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> *> parsers; 
};