#include "NL_MessageSent.h"

bool MessageSent::sendJsonData(QTcpSocket *socket, const QJsonObject &jsonObject)
{
    if (!socket || !socket->isOpen())
    {
        qWarning() << "MessageSent::sendJsonData: Socket is not open or null. Cannot send JSON data.";
        return false;
    }

    QJsonDocument doc(jsonObject);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact); // 紧凑格式，不含空白

    quint32 dataLength = jsonData.length();

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15); // 确保版本一致性

    out << qToBigEndian(dataLength); // 先写入长度（网络字节序）
    block.append(jsonData);          // 再追加JSON数据

    qint64 bytesWritten = socket->write(block);
    if (bytesWritten == -1)
    {
        qWarning() << "MessageSent::sendJsonData: Failed to write data to socket:" << socket->errorString();
        return false;
    }
    socket->flush(); // 确保数据立即发送

    qDebug() << "MessageSent::sendJsonData to socket: " << socket->socketDescriptor() << " Type: " << jsonObject["mtype"].toString();
    return true;
}
