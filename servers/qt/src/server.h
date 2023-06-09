#include <QtNetwork>
#include <QTcpSocket>
#include <QObject>
#include "parser/parsTools.h"
#include "../proto/message.pb.h"

class Server : public QObject
{
    Q_OBJECT
public:
    Server(QObject *parent = nullptr);

    ~Server();

public slots:
    void newUser();

    void read();

    void write(QByteArray msg, int idusersocs);

signals:
    void readyToWrite(QByteArray msg, int idusersocs);

private:
    QTcpServer *server;
    int server_status;
    QMap<int, QTcpSocket *> Sockets;
};