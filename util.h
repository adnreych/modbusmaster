#ifndef UTIL_H
#define UTIL_H

#include <writeregistermodel.h>

#include <QModbusDataUnit>
#include <QModbusReply>
#include <QModbusClient>


class Util : public QObject
{
    Q_OBJECT

private:
    QModbusDataUnit readWriteRequest() const;
    int startAddress;
    quint16 numberOfEntries;

public:
    Util();
    ~Util();
    void connectDevice();
    void read(int startAddress, quint16 numberOfEntries);
    void write(int startAddress, quint16 numberOfEntries, QVector<quint16> values);
    void onReadReady();

private:
    QModbusReply *lastRequest = nullptr;
    QModbusClient *modbusDevice = nullptr;
    WriteRegisterModel *writeModel = nullptr;
};



#endif // UTIL_H
