#include "NL_ConnectionsManager.h"

ConnectionsManager::ConnectionsManager(QObject *parent) : QTcpServer(parent)
{
}

void ConnectionsManager::startServer(quint16 port)
{
    if (!listen(QHostAddress::Any, port))
    {
        qCritical() << "ConnectionsManager: Unable to start the server:" << errorString();
        return;
    }
    qDebug() << "ConnectionsManager: Server listening on port" << port << "\n";
}

void ConnectionsManager::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket *clientSocket = new QTcpSocket(this); // QTcpSocket 的父对象是 ConnectionsManager
    if (!clientSocket->setSocketDescriptor(socketDescriptor))
    {
        qWarning() << "ConnectionsManager: Failed to set socket descriptor for new client:" << clientSocket->errorString();
        delete clientSocket;
        return;
    }

    qDebug() << "ConnectionsManager: New client connected from" << clientSocket->peerAddress().toString() << "on socket" << socketDescriptor;

    // 为每个新连接创建一个 ConnectionHandler 实例
    ConnectionsHandler *handler = new ConnectionsHandler(clientSocket, this); // Handler 的父对象也是 ConnectionsManager
    m_clientHandlers.append(handler);                                         // 将处理器添加到列表中

    // 连接 ConnectionHandler 的 disconnected 信号到 ConnectionsManager 的槽，以便清理
    connect(handler, &ConnectionsHandler::clientDisconnected, this, &ConnectionsManager::onClientHandlerDisconnected);
}

void ConnectionsManager::onClientHandlerDisconnected(ConnectionsHandler *handler)
{
    qDebug() << "ConnectionsManager: Client handler for socket" << handler->findChild<QTcpSocket *>()->socketDescriptor() << "disconnected, removing from list.";
    m_clientHandlers.removeOne(handler); // 从列表中移除处理器
    // handler 对象会在其 onDisconnected 槽中调用 deleteLater() 进行自我清理
}
