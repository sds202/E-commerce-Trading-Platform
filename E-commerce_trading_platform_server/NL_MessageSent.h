#ifndef NL_MESSAGESENT_H
#define NL_MESSAGESENT_H

#include "All.h"

/**
 * @brief 消息发送工具类，提供网络数据发送功能
 */
class MessageSent
{
public:
    static bool sendJsonData(QTcpSocket *socket, const QJsonObject &jsonObject); // 发送JSON数据到客户端
};

#endif // NL_MESSAGESENT_H
