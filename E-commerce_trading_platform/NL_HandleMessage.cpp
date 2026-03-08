#include "NL_HandleMessage.h"

HandleMessage::HandleMessage(QObject *parent) : QObject(parent), m_nextBlockSize(0)
{
    connect(&tcpSocket, &QTcpSocket::readyRead, this, &HandleMessage::onReadyRead);
}

bool HandleMessage::sendJsonData(QTcpSocket *socket, const QJsonObject &jsonObject)
{
    if (!socket || !socket->isOpen())
    {
        qWarning() << "HandleMessage::sendJsonData: Socket is not open or null. Cannot send JSON data.";
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
        qWarning() << "HandleMessage::sendJsonData: Failed to write data to socket:" << socket->errorString();
        return false;
    }
    socket->flush(); // 确保数据立即发送

    qDebug() << "NetworkProtocolUtils::sendJsonData: Sent" << bytesWritten << "bytes (length:" << dataLength << ") JSON:" << jsonData;
    return true;
}

void HandleMessage::onReadyRead()
{
    m_buffer.append(tcpSocket.readAll());

    while (true)
    {
        if (m_nextBlockSize == 0)
        { // 正在等待消息长度
            if (m_buffer.size() < (int)sizeof(quint32))
            {
                break; // 数据不足以读取长度，等待更多数据
            }
            QDataStream in(&m_buffer, QIODevice::ReadOnly);
            in.setVersion(QDataStream::Qt_5_15);
            quint32 tempSize;
            in >> tempSize;
            m_nextBlockSize = qFromBigEndian(tempSize); // 转换为本地字节序

            m_buffer = m_buffer.mid(sizeof(quint32)); // 移除已读取的长度字节
        }

        if (m_nextBlockSize > 0)
        { // 正在等待消息体
            if (m_buffer.size() < (int)m_nextBlockSize)
            {
                break; // 数据不足以读取完整消息体，等待更多数据
            }
            QByteArray jsonData = m_buffer.left(m_nextBlockSize);
            m_buffer = m_buffer.mid(m_nextBlockSize); // 移除已处理的消息体字节
            m_nextBlockSize = 0;                      // 重置，准备接收下一个消息

            QJsonParseError parseError;
            QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);

            if (parseError.error != QJsonParseError::NoError)
            {
                qDebug() << QString("HandleMessage: JSON parse error: %1").arg(parseError.errorString());
                continue;
            }

            if (doc.isObject())
            {
                QJsonObject response = doc.object();
                qDebug() << QString("HandleMessage: Received response: %1").arg(QJsonDocument(response).toJson(QJsonDocument::Compact).constData());
                QString responseType = response.value("mtype").toString();

                if (responseType == "register")
                {
                    bool success = (response.value("status").toString() == "success");
                    QString message = response.value("message").toString();
                    emit registerResponse(success, message); // 发射注册响应信号
                }
                else if (responseType == "login")
                {
                    bool success = (response.value("status").toString() == "success");
                    QString message = response.value("message").toString();
                    QString username = response.value("username").toString();
                    QString password = response.value("password").toString();
                    QString type = response.value("type").toString();
                    unsigned long long balance = response.value("balance").toInteger();
                    emit loginResponse(success, message, username, password, type, balance);
                }
                else if (responseType == "commodities_response")
                {
                    QString context = response.value("context").toString();
                    emit commoditiesResponseReceived(doc, context);
                }
                else if (responseType == "fetch_all_info_of_commodity_response")
                {
                    bool success = (response.value("status").toString() == "success");
                    QString message = response.value("message").toString();
                    QVariantMap commodityData;
                    if (success)
                    {
                        if (response.contains("commodity") && response.value("commodity").isObject())
                        {
                            commodityData = response.value("commodity").toObject().toVariantMap();
                        }
                        else
                        {
                            qDebug() << "HandleMessage: 'commodity' object not found in fetch_all_info_of_commodity_response.";
                            success = false; // 数据不完整，也算失败
                            message = "Received incomplete commodity data.";
                        }
                    }
                    if (response.value("page") == 1)
                        emit fetchAllInfoOfCommodityResponse(success, message, commodityData);
                    else
                        emit fetchAllInfoOfCommodityResponse_page3(success, message, commodityData);
                }
                else if (responseType == "purchase_response")
                {
                    bool success = (response.value("status").toString() == "success");
                    QString message = response.value("message").toString();
                    unsigned long long newBuyerBalance = 0;
                    unsigned int newCommodityStock = 0;

                    if (success)
                    {
                        newBuyerBalance = static_cast<unsigned long long>(response.value("newBuyerBalance").toInteger());
                        newCommodityStock = static_cast<unsigned int>(response.value("newCommodityStock").toInteger());
                    }
                    qDebug() << "HandleMessage: Emitting purchaseResponse. Success:" << success << ", Message:" << message << ", New Stock:" << newCommodityStock;

                    emit purchaseResponse(success, message, newBuyerBalance, newCommodityStock);
                }
                else if (responseType == "commodity_update_response")
                {
                    bool success = (response.value("status").toString() == "success");
                    QString message = response.value("message").toString();
                    qDebug() << "HandleMessage: Emitting commodityUpdateResponse. Success:" << success << ", Message:" << message;
                    emit commodityUpdateResponse(success, message);
                }
                else if (responseType == "commodity_new_response")
                {
                    bool success = (response.value("status").toString() == "success");
                    QString message = response.value("message").toString();
                    qDebug() << "HandleMessage: Emitting commodityNewResponse. Success:" << success << ", Message:" << message;
                    emit commodityNewResponse(success, message);
                }
                else if (responseType == "set_uni_discount_response")
                {
                    bool success = (response.value("status").toString() == "success");
                    QString message = response.value("message").toString();
                    float uniDiscount = response["uniDiscount"].toDouble();
                    QString sourceStoreName = response["sourceStoreName"].toString();
                    QString type = response["type"].toString();
                    qDebug() << "HandleMessage: Emitting setUniDiscountResponse. Success:" << success << ", Message:" << message;
                    emit setUniDiscountResponse(success, message, uniDiscount, sourceStoreName, type);
                }
                else if (responseType == "change_password_response")
                {
                    bool success = (response.value("status").toString() == "success");
                    QString message = response.value("message").toString();
                    qDebug() << "HandleMessage: Emitting changePasswordResponse. Success:" << success << ", Message:" << message;
                    emit changePasswordResponse(success, message);
                }
                else if (responseType == "recharge_response")
                {
                    bool success = (response.value("status").toString() == "success");
                    QString message = response.value("message").toString();
                    unsigned long long newBalance = 0;
                    if (success && response.contains("newBalance"))
                    {
                        newBalance = static_cast<unsigned long long>(response.value("newBalance").toInteger());
                    }
                    qDebug() << "HandleMessage: Emitting rechargeResponse. Success:" << success << ", Message:" << message << ", New Balance:" << newBalance;
                    emit rechargeResponse(success, message, newBalance);
                }
                else if (responseType == "fetch_cart_response")
                {
                    bool success = (response["status"].toString() == "success");
                    QString message = response["message"].toString();
                    QJsonArray cartItems = response["cartItems"].toArray();
                    emit cartFetched(success, message, cartItems);
                }
                else if (responseType == "update_cart_item_quantity_response")
                {
                    bool success = (response["status"].toString() == "success");
                    QString message = response["message"].toString();
                    QString action = response["action"].toString();
                    QJsonArray updatedCartItems;
                    if (success && response.contains("cartItems"))
                    {
                        updatedCartItems = response["cartItems"].toArray();
                    }
                    qDebug() << "HandleMessage: Emitting updateCartItemQuantityResponse. Success:" << success
                             << ", Message:" << message << ", Action:" << action;
                    emit updateCartItemQuantityResponse(success, message, action, updatedCartItems);
                }
                else if (responseType == "add_to_cart_response")
                {
                    bool success = (response["status"].toString() == "success");
                    QString message = response["message"].toString();
                    QJsonArray updatedCartItems;
                    if (success && response.contains("cartItems"))
                    {
                        updatedCartItems = response["cartItems"].toArray();
                    }
                    qDebug() << "HandleMessage: Emitting addToCartResponse. Success:" << success << ", Message:" << message;
                    emit addToCartResponse(success, message, updatedCartItems);
                }
                else if (responseType == "remove_cart_item_response")
                {
                    bool success = (response["status"].toString() == "success");
                    QString message = response["message"].toString();
                    QString action = response["action"].toString();

                    QJsonArray updatedCartItems;
                    if (success && response.contains("cartItems"))
                    {
                        updatedCartItems = response["cartItems"].toArray();
                    }

                    qDebug() << "HandleMessage: Emitting removeCartItemResponse. Success:" << success
                             << ", Message:" << message << ", Action:" << action;
                    emit removeCartItemResponse(success, message, action, updatedCartItems);
                }
                else if (responseType == "clear_cart_response")
                {
                    bool success = (response["status"].toString() == "success");
                    QString message = response["message"].toString();
                    QString action = response["action"].toString();

                    QJsonArray updatedCartItems;
                    if (success && response.contains("cartItems"))
                    {
                        updatedCartItems = response["cartItems"].toArray();
                    }

                    qDebug() << "HandleMessage: Emitting clearCartResponse. Success:" << success
                             << ", Message:" << message << ", Action:" << action;
                    emit clearCartResponse(success, message, action, updatedCartItems);
                }
                else if (responseType == "fetch_orders_response")
                {
                    bool success = (response["status"].toString() == "success");
                    QString message = response["message"].toString();

                    QJsonArray ordersArray;
                    if (success && response.contains("orders"))
                    {
                        ordersArray = response["orders"].toArray();
                    }

                    qDebug() << "HandleMessage: Emitting fetchOrdersResponse. Success:" << success
                             << ", Message:" << message << ", Orders count:" << ordersArray.size();
                    emit fetchOrdersResponse(success, message, ordersArray);
                }
                else if (responseType == "delete_order_response")
                {
                    bool success = (response["status"].toString() == "success");
                    QString message = response["message"].toString();

                    unsigned int deletedOrderId = 0;
                    if (success && response.contains("deletedOrderId"))
                    {
                        deletedOrderId = static_cast<unsigned int>(response["deletedOrderId"].toInteger());
                    }

                    qDebug() << "HandleMessage: Emitting deleteOrderResponse. Success:" << success
                             << ", Message:" << message << ", Deleted Order ID:" << deletedOrderId;
                    emit deleteOrderResponse(success, message, deletedOrderId);
                }
                else if (responseType == "validate_order_items_response")
                {
                    QString status = response["status"].toString();
                    QString message = response["message"].toString();

                    QJsonArray validatedItems;
                    unsigned long long totalPrice = 0;
                    QString changes;

                    if (response.contains("validatedItems"))
                    {
                        validatedItems = response["validatedItems"].toArray();
                    }
                    if (response.contains("totalPrice"))
                    {
                        totalPrice = static_cast<unsigned long long>(response["totalPrice"].toInteger());
                    }
                    if (response.contains("changes"))
                    {
                        changes = response["changes"].toString();
                    }

                    qDebug() << "HandleMessage: Emitting validateOrderItemsResponse. Status:" << status
                             << ", Message:" << message;
                    emit validateOrderItemsResponse(status, message, validatedItems, totalPrice, changes);
                }
                else if (responseType == "create_order_response")
                {
                    bool success = (response["status"].toString() == "success");
                    QString message = response["message"].toString();

                    unsigned long long newBalance = 0;
                    unsigned long long totalPrice = 0;

                    if (success)
                    {
                        if (response.contains("newBalance"))
                        {
                            newBalance = static_cast<unsigned long long>(response["newBalance"].toInteger());
                        }
                        if (response.contains("totalPrice"))
                        {
                            totalPrice = static_cast<unsigned long long>(response["totalPrice"].toInteger());
                        }
                    }

                    qDebug() << "HandleMessage: Emitting createOrderResponse. Success:" << success
                             << ", Message:" << message;
                    emit createOrderResponse(success, message, newBalance, totalPrice);
                }
                else if (responseType == "pay_order_response")
                {
                    bool success = (response["status"].toString() == "success");
                    QString message = response["message"].toString();

                    unsigned int orderId = 0;
                    unsigned long long paidAmount = 0;
                    unsigned long long newBalance = 0;

                    if (success)
                    {
                        if (response.contains("orderId"))
                        {
                            orderId = static_cast<unsigned int>(response["orderId"].toInteger());
                        }
                        if (response.contains("paidAmount"))
                        {
                            paidAmount = static_cast<unsigned long long>(response["paidAmount"].toInteger());
                        }
                        if (response.contains("newBalance"))
                        {
                            newBalance = static_cast<unsigned long long>(response["newBalance"].toInteger());
                        }
                    }

                    qDebug() << "HandleMessage: Emitting payOrderResponse. Success:" << success
                             << ", Message:" << message << ", Order ID:" << orderId;
                    emit payOrderResponse(success, message, orderId, paidAmount, newBalance);
                }
                else if (responseType == "delete_commodity_response")
                {
                    bool success = (response["status"].toString() == "success");
                    QString message = response["message"].toString();
                    unsigned int deletedCommodityId = 0;

                    if (success && response.contains("commodityId"))
                    {
                        deletedCommodityId = static_cast<unsigned int>(response["commodityId"].toInteger());
                    }

                    qDebug() << "HandleMessage: Emitting deleteCommodityResponse. Success:" << success
                             << ", Message:" << message << ", Commodity ID:" << deletedCommodityId;
                    emit deleteCommodityResponse(success, message, deletedCommodityId);
                }
            }
            else
            {
                qDebug() << "HandleMessage: Received non-object JSON from server.";
            }
        }
    }
}
