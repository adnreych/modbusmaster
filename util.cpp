#include "util.h"
#include "writeregistermodel.h"

#include <QStandardItemModel>
#include <QDebug>
#include <QModbusRtuSerialMaster>

QTextStream cout(stdout);
QTextStream cin(stdin);
using namespace std;

Util::Util()
{
    writeModel = new WriteRegisterModel(this);
    writeModel->setStartAddress(1);
    writeModel->setNumberOfValues("2");
}

Util::~Util()
{
    if (modbusDevice)
        modbusDevice->disconnectDevice();
    delete modbusDevice;
}

void Util::connectDevice()
{
 /*   if (!modbusDevice)
        return; */
    qDebug() << "on connect METH";
    modbusDevice = new QModbusRtuSerialMaster(this);

    if (modbusDevice->state() != QModbusDevice::ConnectedState) {
        qDebug() << "on connect";
            modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, "/dev/ttyUSB0");
            modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter, 0);
            modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, 19200);
            modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, 8);
            modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, 1);
            modbusDevice->setTimeout(5000);
            modbusDevice->setNumberOfRetries(3);

        if (!modbusDevice->connectDevice()) {
            qDebug() << "Connect FAIL";
            qDebug() << modbusDevice->errorString();
        } else {
            qDebug() << "Connect OK";
        }
    } else {
        qDebug() << "on disconnect";
        modbusDevice->disconnectDevice();
    }
}

void Util::read(int pStartAddress, quint16 pNumberOfEntries)

{
    startAddress = pStartAddress;
    numberOfEntries = pNumberOfEntries;
    if (auto *reply = modbusDevice->sendReadRequest(readWriteRequest(), 247)) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &Util::onReadReady);
        else {
            qDebug() << "Not Finished";
            delete reply;
        }
            // broadcast replies return immediately
    } else {
        qDebug() << "Read error" + modbusDevice->errorString();
    }
}

void Util::write(int pStartAddress, quint16 pNumberOfEntries, QVector<quint16> values)
{
    startAddress = pStartAddress;
    numberOfEntries = pNumberOfEntries;

    QModbusDataUnit writeUnit = readWriteRequest();
    QModbusDataUnit::RegisterType table = writeUnit.registerType();
    for (int i = 0, total = int(writeUnit.valueCount()); i < total; ++i) {
        if (table == QModbusDataUnit::Coils)
            writeUnit.setValue(i, values[i]);
        else
            writeUnit.setValue(i, values[i]);
    }

    if (auto *reply = modbusDevice->sendWriteRequest(writeUnit, 247)) {
        if (!reply->isFinished()) {
            connect(reply, &QModbusReply::finished, this, [this, reply]() {
                if (reply->error() == QModbusDevice::ProtocolError) {
                   qDebug() << "Write response error0: " + reply->errorString() + reply->rawResult().exceptionCode();
                } else if (reply->error() != QModbusDevice::NoError) {
                   qDebug() << "Write response error1: " + reply->errorString() + reply->rawResult().exceptionCode();
                }
                reply->deleteLater();
            });
        } else {
            // broadcast replies return immediately
            reply->deleteLater();
        }
    } else {
         qDebug() << "Write response error1: " + modbusDevice->errorString();
    }
}


void Util::onReadReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply) {
        qDebug() << "Reply == null";
        return;
    }

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        qDebug() << "NO ERROR";
        for (int i = 0, total = int(unit.valueCount()); i < total; ++i) {
            const QString entry = tr("Address: %1, Value: %2").arg(unit.startAddress() + i)
                                     .arg(QString::number(unit.value(i),
                                          unit.registerType() <= QModbusDataUnit::Coils ? 10 : 16));
            qDebug() <<"Entry: " + entry;
        }
    } else if (reply->error() == QModbusDevice::ProtocolError) {
        qDebug() <<"Read response error0 " + reply->errorString() +" code: " + reply->rawResult().exceptionCode();
    } else {
        qDebug() <<"Read response error1 " + reply->errorString() +" code: " + reply->error();

    }

    reply->deleteLater();
}

QModbusDataUnit Util::readWriteRequest() const
{
    const auto table =
        static_cast<QModbusDataUnit::RegisterType>(QModbusDataUnit::RegisterType::HoldingRegisters);

    return QModbusDataUnit(table, startAddress, numberOfEntries);
}



