#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QDebug>
#include <QTextStream>

enum Level
{
    LOG_ERR,
    LOG_WARN,
    LOG_INFO,
    LOG_DEBUG
};

class Logger : public QObject
{
    Q_OBJECT
public:
    Logger(){};
    Logger(QString filePath, int level) : filePath(filePath), curentLevel(level){};

    void setPath(QString s) { filePath = s; };
    void setLevel(int level) { curentLevel = level; }

    QString getPath() { return filePath; };
    int getLevel() { return curentLevel; };

    inline void log(int level, QString msg)
    {
        if (level > curentLevel)
            return;

        if (!fileIsOk())
            return;

        QTextStream out(&logFile);
        std::string std_msg = "ERROR: " + msg.toStdString() + "\n";

        switch (level)
        {
        case LOG_ERR:
            out << "ERROR: " << msg << "\n";
            break;

        case LOG_WARN:
            out << "WARNING: " << msg << "\n";
            break;

        case LOG_INFO:
            out << "INFO: " << msg << "\n";
            break;

        case LOG_DEBUG:
            out << "DEBUG: " << msg << "\n";
            break;

        default:
            out << "ANY: " << msg << "\n";
            break;
        }
    }

private:
    bool fileIsOk()
    {
        if (!logFile.isOpen())
        {
            logFile.setFileName(filePath);
            if (logFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
                return true;
        }
        else if (logFile.fileName() != filePath)
        {
            logFile.close();
            logFile.setFileName(filePath);
            if (logFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
                return true;
        }
        else
        {
            return true;
        }
        return false;
    }

    int curentLevel;
    QFile logFile;
    QString filePath;
};

#endif // LOGER_H