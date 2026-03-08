#ifndef NL_CONNECTIONSMANAGER_H
#define NL_CONNECTIONSMANAGER_H

#include "All.h"
#include "NL_ConnectionsHandler.h" // 引入客户端连接处理器

/**
 * @brief 连接管理器类，负责管理TCP服务器和客户端连接
 */
class ConnectionsManager : public QTcpServer
{
    Q_OBJECT
public:
    explicit ConnectionsManager(QObject *parent = nullptr); // 构造函数
    void startServer(quint16 port);                         // 启动TCP服务器

protected:
    void incomingConnection(qintptr socketDescriptor) override; // 处理新客户端连接

private slots:
    void onClientHandlerDisconnected(ConnectionsHandler *handler); // 处理客户端断开连接

private:
    QList<ConnectionsHandler *> m_clientHandlers; // 存储所有活跃的客户端连接处理器
};

#endif // NL_CONNECTIONSMANAGER_H
