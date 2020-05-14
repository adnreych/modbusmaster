#include <util.h>
#include <iostream>

#include <QCoreApplication>
#include <QModbusRtuSerialMaster>
using namespace std;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    int action;
    int startAddress;
    quint16 numberOfEntries;

    Util u;
    u.connectDevice();

    cout << "Введите 0 если хотите считать данные и 1 если записать: ";
    cin >> action;

    if (action == 0) {
        cout << "Введите адрес: ";
        cin >> startAddress;
        Q_ASSERT(startAddress >= 0 && startAddress < 10);

        cout << "Введите количество считываемых адресов: ";
        cin >> numberOfEntries;

        u.read(startAddress, numberOfEntries);
    } else {
        cout << "Введите адрес: ";
        cin >> startAddress;
        Q_ASSERT(startAddress >= 0 && startAddress < 10);

        cout << "Введите количество записываемых адресов: ";
        cin >> numberOfEntries;

        QVector<quint16> values;

        for (int i = 0; i < numberOfEntries; i++) {
            quint16 curr;
            cout << "Введите значение №: " + i;
            cin >> curr;
            values.push_back(curr);
        }

        u.write(startAddress, numberOfEntries, values);
    }



    return a.exec();
}
