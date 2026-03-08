#include "NL_ConnectionsHandler.h"

ConnectionsHandler::ConnectionsHandler(QTcpSocket *socket, QObject *parent)
    : QObject(parent),
      m_socket(socket),
      m_nextBlockSize(0), // 初始化为0，表示等待读取消息长度
      cart("")
{
    // 将套接字所有权转移给当前对象，以便在析构时自动清理
    m_socket->setParent(this);

    // 连接套接字信号到本类的槽
    connect(m_socket, &QTcpSocket::readyRead, this, &ConnectionsHandler::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &ConnectionsHandler::onDisconnected);
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
            this, &ConnectionsHandler::onErrorOccurred);

    qDebug() << "ConnectionsHandler created for socket" << m_socket->socketDescriptor();
}

ConnectionsHandler::~ConnectionsHandler()
{
    qDebug() << "ConnectionsHandler destroyed for socket" << m_socket->socketDescriptor();
    // m_socket 会随着 this 对象的销毁而被 deleteLater()
}

void ConnectionsHandler::onReadyRead()
{
    // 将所有新数据追加到缓冲区
    m_buffer.append(m_socket->readAll());

    while (true)
    {
        if (m_nextBlockSize == 0)
        { // 正在等待消息长度
            if (m_buffer.size() < (int)sizeof(quint32))
            {
                break; // 数据不足，等待更多数据
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
                break; // 数据不足，等待更多数据
            }
            QByteArray jsonData = m_buffer.left(m_nextBlockSize);
            m_buffer = m_buffer.mid(m_nextBlockSize); // 移除已处理的消息体字节
            m_nextBlockSize = 0;                      // 重置，准备接收下一个消息

            QJsonParseError parseError;
            QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);

            if (parseError.error != QJsonParseError::NoError)
            {
                qWarning() << "ConnectionsHandler: JSON parse error for socket" << m_socket->socketDescriptor() << ":" << parseError.errorString();
                QJsonObject errorResponse;
                errorResponse["status"] = "error";
                errorResponse["code"] = 400;
                errorResponse["message"] = "Invalid JSON format.";
                MessageSent::sendJsonData(m_socket, errorResponse);
                continue;
            }

            if (doc.isObject())
            {
                QJsonObject request = doc.object();
                qDebug() << "ConnectionsHandler: Received JSON from socket" << m_socket->socketDescriptor() << ":" << request["mtype"].toString();

                QJsonObject response = processRequest(request); // 处理请求

                MessageSent::sendJsonData(m_socket, response); // 发送响应
            }
            else
            {
                qWarning() << "ConnectionsHandler: Received non-object JSON from socket" << m_socket->socketDescriptor();
                QJsonObject errorResponse;
                errorResponse["status"] = "error";
                errorResponse["message"] = "Received non-object JSON.";
                MessageSent::sendJsonData(m_socket, errorResponse);
            }
        }
    }
}

void ConnectionsHandler::onDisconnected()
{
    qDebug() << "ConnectionsHandler: Client disconnected from socket" << m_socket->socketDescriptor();
    emit clientDisconnected(this); // 通知服务器该客户端已断开
    this->deleteLater();           // 安全删除本处理器对象
}

void ConnectionsHandler::onErrorOccurred(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    qWarning() << "ConnectionsHandler: Socket error for socket" << m_socket->socketDescriptor() << ":" << m_socket->errorString();
}

// 业务逻辑处理函数
QJsonObject ConnectionsHandler::processRequest(const QJsonObject &request)
{
    QString requestType = request.value("mtype").toString();

    if (requestType == "register")
    {
        return handleRegisterRequest(request);
    }
    else if (requestType == "login")
    {
        return handleLoginRequest(request);
    }
    else if (requestType == "fetch_commodities_request")
    {
        return handleFetchCommoditiesRequest(request);
    }
    else if (requestType == "fetch_all_info_of_commodity")
    {
        return handleFetchAllInfoOfCommodityRequest(request);
    }
    else if (requestType == "purchase")
    {
        return handlePurchaseRequest(request);
    }
    else if (requestType == "update_commodity")
    {
        return handleUpdateCommodityRequest(request);
    }
    else if (requestType == "new_commodity")
    {
        return handleNewCommodityRequest(request);
    }
    else if (requestType == "set_uni_discount")
    {
        return handleSetUniDiscountRequest(request);
    }
    else if (requestType == "change_password")
    {
        return handleChangePasswordRequest(request);
    }
    else if (requestType == "recharge")
    {
        return handleRechargeRequest(request);
    }
    else if (requestType == "fetch_cart")
    {
        return handleFetchCartRequest(request);
    }
    else if (requestType == "update_cart_item_quantity")
    {
        return handleUpdateCartItemQuantityRequest(request);
    }
    else if (requestType == "add_to_cart")
    {
        return handleAddToCartRequest(request);
    }
    else if (requestType == "remove_cart_item")
    {
        return handleRemoveCartItemRequest(request);
    }
    else if (requestType == "clear_cart")
    {
        return handleClearCartRequest(request);
    }
    else if (requestType == "fetch_orders")
    {
        return handleFetchOrdersRequest(request);
    }
    else if (requestType == "delete_order")
    { // 新增处理
        return handleDeleteOrderRequest(request);
    }
    else if (requestType == "validate_order_items")
    { // 新增
        return handleValidateOrderItemsRequest(request);
    }
    else if (requestType == "create_order")
    { // 新增
        return handleCreateOrderRequest(request);
    }
    else if (requestType == "pay_order")
    { // 新增
        return handlePayOrderRequest(request);
    }
    else if (requestType == "delete_commodity")
    { // 新增
        return handleDeleteCommodityRequest(request);
    }
    else
    {
        QJsonObject response;
        response["status"] = "error";
        response["code"] = 400;
        response["message"] = "Unknown request type.";
        return response;
    }
}

QJsonObject ConnectionsHandler::handleRegisterRequest(const QJsonObject &request)
{
    QString username = request.value("username").toString();
    QString password = request.value("password").toString();
    QString type = request.value("user_type").toString();

    User user;
    RegisterErrorType rtype = user.userRegister(username, password, type);

    QJsonObject response;
    response["mtype"] = "register";
    switch (rtype)
    {
    case RegisterErrorType::NoError:
        response["status"] = "success";
        response["message"] = "成功注册";
        break;
    case RegisterErrorType::UsernameAlreadyExists:
        response["status"] = "error";
        response["message"] = "用户名已存在";
        break;
    case RegisterErrorType::FileError:
        response["status"] = "error";
        response["message"] = "文件系统出错了";
        break;
    default:
        response["status"] = "error";
        response["message"] = "Unknown error.";
        break;
    }

    return response;
}
QJsonObject ConnectionsHandler::handleLoginRequest(const QJsonObject &request)
{
    QString username = request.value("username").toString();
    QString password = request.value("password").toString();

    User user;
    LoginErrorType rtype = user.userLogin(username, password);

    QJsonObject response;
    response["mtype"] = "login";
    switch (rtype)
    {
    case LoginErrorType::NoError:
        response["status"] = "success";
        response["message"] = "Register successful.";
        response["username"] = user.getName();
        response["password"] = user.getPassword();
        response["type"] = user.getType();
        response["balance"] = static_cast<qint64>(user.getBalance());
        break;
    case LoginErrorType::UsernameInexists:
        response["status"] = "error";
        response["message"] = "用户名不存在";
        break;
    case LoginErrorType::FileError:
        response["status"] = "error";
        response["message"] = "文件系统出错了";
        break;
    case LoginErrorType::WrongPassword:
        response["status"] = "error";
        response["message"] = "密码错误";
        break;
    default:
        response["status"] = "error";
        response["message"] = "Unknown error.";
        break;
    }

    return response;
}
QJsonObject ConnectionsHandler::handleFetchCommoditiesRequest(const QJsonObject &request)
{
    QJsonObject response;
    response["mtype"] = "commodities_response"; // 响应类型，与客户端预期一致

    QString storeName = request["storeName"].toString();
    QString context = request["context"].toString(); // 用于客户端区分是哪个页面的数据
    response["context"] = context;                   // 将上下文信息也返回给客户端

    QJsonArray commoditiesArray;

    CommodityDAO commoditydao;

    if (!commoditydao.FetchAllCommodities(storeName, commoditiesArray))
    {
        response["status"] = "error";
        response["message"] = "Database query failed";
        return response;
    }

    response["status"] = "success";
    response["message"] = "Commodities fetched successfully.";
    response["commodities"] = commoditiesArray; // 将商品数组添加到响应中

    return response;
}
QJsonObject ConnectionsHandler::handleFetchAllInfoOfCommodityRequest(const QJsonObject &request)
{
    QJsonObject response;
    response["mtype"] = "fetch_all_info_of_commodity_response"; // 响应类型

    unsigned int commodityId = static_cast<unsigned int>(request["id"].toInteger()); // 获取商品ID

    Commodity commodity;

    // 调用业务逻辑层
    QVariantMap commodityData = commodity.getCommodityDetails(commodityId);

    if (!commodityData.isEmpty())
    { // 如果成功获取到数据
        response["status"] = "success";
        response["page"] = request["page"];
        response["message"] = "Commodity details fetched successfully.";
        // 将 QVariantMap 转换为 QJsonObject 放入响应
        QJsonObject commodityObj;
        for (auto it = commodityData.constBegin(); it != commodityData.constEnd(); ++it)
        {
            commodityObj[it.key()] = QJsonValue::fromVariant(it.value());
        }
        response["commodity"] = commodityObj;
    }
    else
    {
        response["status"] = "error";
        response["message"] = "Commodity not found or database error.";
    }

    return response;
}
QJsonObject ConnectionsHandler::handlePurchaseRequest(const QJsonObject &request)
{
    QJsonObject response;
    response["mtype"] = "purchase_response"; // 响应类型

    QString buyerUsername = request["buyerUsername"].toString();
    unsigned int commodityId = static_cast<unsigned int>(request["commodityId"].toInteger());
    unsigned int purchaseQuantity = static_cast<unsigned int>(request["purchaseQuantity"].toInteger());
    QString sellerStoreName = request["sellerStoreName"].toString();                                    // 对应 user_table.storeName
    unsigned long long totalPrice = static_cast<unsigned long long>(request["totalPrice"].toInteger()); // 客户端提供的总价

    qDebug() << "ConnectionsHandler: Handling purchase request from" << buyerUsername
             << "for commodity ID:" << commodityId << ", quantity:" << purchaseQuantity
             << ", seller store:" << sellerStoreName << ", total price:" << totalPrice;

    // 调用业务逻辑层
    User user;
    QVariantMap purchaseResult = user.makePurchase(
        buyerUsername, commodityId, purchaseQuantity, sellerStoreName, totalPrice);

    if (purchaseResult["status"].toString() == "success")
    {
        response["status"] = "success";
        response["message"] = purchaseResult["message"].toString();
        response["newBuyerBalance"] = static_cast<qint64>(purchaseResult["newBuyerBalance"].toULongLong());
        response["newCommodityStock"] = static_cast<qint64>(purchaseResult["newCommodityStock"].toUInt()); // 新增：将新库存添加到响应中
    }
    else
    {
        response["status"] = "error";
        response["message"] = purchaseResult["message"].toString();
    }

    return response;
}
QJsonObject ConnectionsHandler::handleUpdateCommodityRequest(const QJsonObject &request)
{
    QJsonObject response;
    response["mtype"] = "commodity_update_response";

    unsigned int id = static_cast<unsigned int>(request["id"].toInteger());
    QString name = request["name"].toString();
    QString type = request["type"].toString();
    QString description = request["description"].toString();
    unsigned long long originalPrice = static_cast<unsigned long long>(request["originalPrice"].toInteger());
    unsigned int stock = static_cast<unsigned int>(request["stock"].toInteger());
    QString sourceStoreName = request["sourceStoreName"].toString();
    double discount = request["discount"].toDouble();
    double uniDiscount = request["uniDiscount"].toDouble();

    qDebug() << "ConnectionsHandler: Handling update commodity request for ID:" << id;

    // 调用 BLL (简化版 BLL)
    QVariantMap updateResult = m_commodity.updateCommodity(
        id, name, type, description, originalPrice, stock, sourceStoreName, discount, uniDiscount);

    response["status"] = updateResult["status"].toString();
    response["message"] = updateResult["message"].toString();
    return response;
}
QJsonObject ConnectionsHandler::handleNewCommodityRequest(const QJsonObject &request)
{
    QJsonObject response;
    response["mtype"] = "commodity_new_response";

    QString name = request["name"].toString();
    QString type = request["type"].toString();
    QString description = request["description"].toString();
    unsigned long long originalPrice = static_cast<unsigned long long>(request["originalPrice"].toInteger());
    unsigned int stock = static_cast<unsigned int>(request["stock"].toInteger());
    QString sourceStoreName = request["sourceStoreName"].toString();
    double discount = request["discount"].toDouble();
    double uniDiscount = request["uniDiscount"].toDouble();

    // 调用 BLL (简化版 BLL)
    QVariantMap updateResult = m_commodity.newCommodity(
        name, type, description, originalPrice, stock, sourceStoreName, discount, uniDiscount);

    response["status"] = updateResult["status"].toString();
    response["message"] = updateResult["message"].toString();
    return response;
}
QJsonObject ConnectionsHandler::handleSetUniDiscountRequest(const QJsonObject &request)
{
    QJsonObject response;
    response["mtype"] = "set_uni_discount_response";

    QString sourceStoreName = request["sourceStoreName"].toString();
    QString type = request["type"].toString();
    float uniDiscount = static_cast<float>(request["uniDiscount"].toDouble());

    qDebug() << "ConnectionsHandler: Handling set uniDiscount request for store:" << sourceStoreName
             << ", type:" << type << ", uniDiscount:" << uniDiscount;

    bool success = false;
    QString message;

    try
    {
        // 功能1：更新商家的统一折扣设置（存储在用户文件中）
        if (UserDAO::setUserUniDiscount(sourceStoreName, type, uniDiscount))
        {
            qDebug() << "ConnectionsHandler: Successfully updated user uniDiscount for" << sourceStoreName;

            // 功能2：同时更新该商家现有商品的统一折扣
            QVariantMap updateResult = m_commodity.setCommodityUniDiscount(sourceStoreName, type, uniDiscount);

            if (updateResult["status"].toString() == "success")
            {
                success = true;
                message = QString("统一折扣设置成功！已更新商家设置和现有商品。\n%1").arg(updateResult["message"].toString());
            }
            else
            {
                success = true; // 商家设置成功了，但商品更新可能部分失败
                message = QString("商家统一折扣设置成功，但更新现有商品时出现问题：\n%1").arg(updateResult["message"].toString());
            }
        }
        else
        {
            success = false;
            message = "更新商家统一折扣设置失败";
        }
    }
    catch (const std::exception &e)
    {
        success = false;
        message = QString("设置统一折扣时发生错误: %1").arg(e.what());
        qDebug() << "ConnectionsHandler: Exception in handleSetUniDiscountRequest:" << e.what();
    }

    response["status"] = success ? "success" : "error";
    response["message"] = message;
    response["uniDiscount"] = uniDiscount;
    response["sourceStoreName"] = sourceStoreName;
    response["type"] = type;

    return response;
}
QJsonObject ConnectionsHandler::handleChangePasswordRequest(const QJsonObject &request)
{
    QJsonObject response;
    response["mtype"] = "change_password_response";

    QString username = request["username"].toString();
    QString newPassword = request["newPassword"].toString();

    qDebug() << "ConnectionsHandler: Handling change password request for user:" << username;

    // 调用 BLL
    QVariantMap changeResult = m_user.changePassword(username, newPassword);

    response["status"] = changeResult["status"].toString();
    response["message"] = changeResult["message"].toString();
    return response;
}
QJsonObject ConnectionsHandler::handleRechargeRequest(const QJsonObject &request)
{
    QJsonObject response;
    response["mtype"] = "recharge_response";

    QString username = request["username"].toString();
    // 假设客户端发送的金额是 unsigned long long (转换为 qint64)
    unsigned long long amount = static_cast<unsigned long long>(request["amount"].toInteger());

    qDebug() << "ConnectionsHandler: Handling recharge request for user:" << username << ", amount:" << amount;

    // 调用 BLL
    QVariantMap rechargeResult = m_user.rechargeBalance(username, amount);

    response["status"] = rechargeResult["status"].toString();
    response["message"] = rechargeResult["message"].toString();
    if (rechargeResult.contains("newBalance"))
    {
        response["newBalance"] = rechargeResult["newBalance"].toLongLong();
    }
    return response;
}
QJsonObject ConnectionsHandler::handleFetchCartRequest(const QJsonObject &request)
{
    QJsonObject response;
    response["mtype"] = "fetch_cart_response";

    QString username = request["username"].toString();

    cart.setUsername(username);

    QJsonArray cartItems;
    QString errorMessage;
    if (!cart.fetchCartItems(cartItems, errorMessage))
    {
        response["status"] = "error";
        response["message"] = errorMessage;
        return response;
    }

    response["status"] = "success";
    response["message"] = "Cart fetched successfully";
    response["cartItems"] = cartItems;

    return response;
}
QJsonObject ConnectionsHandler::handleUpdateCartItemQuantityRequest(const QJsonObject &request)
{
    QJsonObject response;
    response["mtype"] = "update_cart_item_quantity_response";

    QString username = request["username"].toString();
    unsigned int cartItemId = static_cast<unsigned int>(request["cartItemId"].toInteger());
    int quantity = request["quantity"].toInt();

    qDebug() << "ConnectionsHandler: Handling update cart item quantity request for user:" << username
             << ", cartItemId:" << cartItemId << ", quantity:" << quantity;

    // 设置购物车用户名
    cart.setUsername(username);

    QString errorMessage;
    bool success = false;

    if (quantity == 0)
    {
        // 数量为0，删除该购物车项目
        success = cart.removeItem(cartItemId, errorMessage);
        if (success)
        {
            response["status"] = "success";
            response["message"] = "购物车项目已删除";
            response["action"] = "removed"; // 告知客户端该项目已被删除
        }
        else
        {
            response["status"] = "error";
            response["message"] = errorMessage;
        }
    }
    else
    {
        // 更新数量
        success = cart.updateItemQuantity(cartItemId, quantity, errorMessage);
        if (success)
        {
            response["status"] = "success";
            response["message"] = "购物车数量更新成功";
            response["action"] = "updated"; // 告知客户端该项目已被更新
            response["newQuantity"] = quantity;
        }
        else
        {
            response["status"] = "error";
            response["message"] = errorMessage;
        }
    }

    return response;
}
QJsonObject ConnectionsHandler::handleAddToCartRequest(const QJsonObject &request)
{
    QJsonObject response;
    response["mtype"] = "add_to_cart_response";

    QString username = request["username"].toString();
    unsigned int commodityId = static_cast<unsigned int>(request["commodityId"].toInteger());
    int quantity = request["quantity"].toInt();

    qDebug() << "ConnectionsHandler: Handling add to cart request for user:" << username
             << ", commodityId:" << commodityId << ", quantity:" << quantity;

    // 设置购物车用户名
    cart.setUsername(username);

    QString errorMessage;
    bool success = cart.addItem(commodityId, quantity, errorMessage);

    if (success)
    {
        response["status"] = "success";
        response["message"] = "商品已成功添加到购物车";

        // 获取更新后的购物车信息
        QJsonArray updatedCartItems;
        QString fetchError;
        if (cart.fetchCartItems(updatedCartItems, fetchError))
        {
            response["cartItems"] = updatedCartItems;
        }
    }
    else
    {
        response["status"] = "error";
        response["message"] = errorMessage;
    }

    return response;
}
QJsonObject ConnectionsHandler::handleRemoveCartItemRequest(const QJsonObject &request)
{
    QJsonObject response;
    response["mtype"] = "remove_cart_item_response";

    QString username = request["username"].toString();
    unsigned int cartItemId = static_cast<unsigned int>(request["cartItemId"].toInteger());

    qDebug() << "ConnectionsHandler: Handling remove cart item request for user:" << username
             << ", cartItemId:" << cartItemId;

    // 设置购物车用户名
    cart.setUsername(username);

    QString errorMessage;
    bool success = cart.removeItem(cartItemId, errorMessage);

    if (success)
    {
        response["status"] = "success";
        response["message"] = "购物车项目删除成功";
        response["action"] = "removed";

        // 获取更新后的购物车信息
        QJsonArray updatedCartItems;
        QString fetchError;
        if (cart.fetchCartItems(updatedCartItems, fetchError))
        {
            response["cartItems"] = updatedCartItems;
        }
        else
        {
            qWarning() << "ConnectionsHandler: Failed to fetch updated cart items:" << fetchError;
        }
    }
    else
    {
        response["status"] = "error";
        response["message"] = errorMessage;
    }

    return response;
}
QJsonObject ConnectionsHandler::handleClearCartRequest(const QJsonObject &request)
{
    QJsonObject response;
    response["mtype"] = "clear_cart_response";

    QString username = request["username"].toString();

    qDebug() << "ConnectionsHandler: Handling clear cart request for user:" << username;

    // 设置购物车用户名
    cart.setUsername(username);

    QString errorMessage;
    bool success = cart.clear(errorMessage);

    if (success)
    {
        response["status"] = "success";
        response["message"] = "购物车已清空";
        response["action"] = "cleared";

        // 返回空的购物车数据
        QJsonArray emptyCartItems;
        response["cartItems"] = emptyCartItems;
    }
    else
    {
        response["status"] = "error";
        response["message"] = errorMessage;
    }

    return response;
}
QJsonObject ConnectionsHandler::handleFetchOrdersRequest(const QJsonObject &request)
{
    QJsonObject response;
    response["mtype"] = "fetch_orders_response";

    QString username = request["username"].toString();

    qDebug() << "ConnectionsHandler: Handling fetch orders request for user:" << username;

    try
    {
        // 使用Order类的静态方法从数据库获取订单
        QList<Order> orders = Order::fetchOrdersByUser(username);

        QJsonArray ordersArray;
        for (const Order &order : orders)
        {
            QJsonObject orderObj;
            orderObj["orderId"] = static_cast<qint64>(order.getOrderId());
            orderObj["userName"] = order.getUserName();
            orderObj["totalPrice"] = static_cast<qint64>(order.getTotalPrice());
            orderObj["state"] = order.getState();
            orderObj["orderTime"] = order.getOrderTime();

            // 转换订单项
            QJsonArray itemsArray;
            for (const QVariantMap &item : order.getItems())
            {
                QJsonObject itemObj;
                itemObj["commodityId"] = static_cast<qint64>(item["commodityId"].toUInt());
                itemObj["commodityName"] = item["commodityName"].toString(); // 新增
                itemObj["quantity"] = static_cast<qint64>(item["quantity"].toUInt());
                itemObj["originalPrice"] = item["originalPrice"].toDouble();
                itemObj["discount"] = item["discount"].toDouble();
                itemObj["uniDiscount"] = item["uniDiscount"].toDouble();
                itemsArray.append(itemObj);
            }
            orderObj["items"] = itemsArray;
            ordersArray.append(orderObj);
        }

        response["status"] = "success";
        response["orders"] = ordersArray;
        response["message"] = QString("成功获取 %1 个订单").arg(orders.size());
    }
    catch (const std::exception &e)
    {
        response["status"] = "error";
        response["message"] = QString("获取订单失败: %1").arg(e.what());
        qDebug() << "ConnectionsHandler: Exception in handleFetchOrdersRequest:" << e.what();
    }
    catch (...)
    {
        response["status"] = "error";
        response["message"] = "获取订单时发生未知错误";
        qDebug() << "ConnectionsHandler: Unknown exception in handleFetchOrdersRequest";
    }

    return response;
}
QJsonObject ConnectionsHandler::handleDeleteOrderRequest(const QJsonObject &request)
{
    QJsonObject response;
    response["mtype"] = "delete_order_response";

    QString username = request["username"].toString();
    unsigned int orderId = static_cast<unsigned int>(request["orderId"].toInteger());

    qDebug() << "ConnectionsHandler: Handling delete order request for user:" << username
             << ", orderId:" << orderId;

    try
    {
        // 先获取订单状态以便生成相应的提示消息
        OrderDAO orderDAO;
        Order orderInfo = orderDAO.getOrderById(orderId);
        QString orderState = orderInfo.getState();

        bool success = orderDAO.deleteOrderAndRestoreStock(orderId, username);

        if (success)
        {
            response["status"] = "success";
            response["deletedOrderId"] = static_cast<qint64>(orderId);

            // 根据订单状态生成不同的提示消息
            if (orderState == "unPaid")
            {
                response["message"] = "订单删除成功，商品库存已恢复";
            }
            else
            {
                response["message"] = QString("订单删除成功（订单状态：%1，无需恢复库存）").arg(orderState);
            }
        }
        else
        {
            response["status"] = "error";
            response["message"] = "删除订单失败";
        }
    }
    catch (const std::exception &e)
    {
        response["status"] = "error";
        response["message"] = QString("删除订单时发生错误: %1").arg(e.what());
        qDebug() << "ConnectionsHandler: Exception in handleDeleteOrderRequest:" << e.what();
    }
    catch (...)
    {
        response["status"] = "error";
        response["message"] = "删除订单时发生未知错误";
        qDebug() << "ConnectionsHandler: Unknown exception in handleDeleteOrderRequest";
    }

    return response;
}
QJsonObject ConnectionsHandler::handleValidateOrderItemsRequest(const QJsonObject &request)
{
    QJsonObject response;
    response["mtype"] = "validate_order_items_response";

    QString username = request["username"].toString();
    QJsonArray itemsArray = request["items"].toArray();

    qDebug() << "ConnectionsHandler: Handling validate order items request for user:" << username;

    try
    {
        QSqlDatabase db = QSqlDatabase::database();

        QJsonArray validatedItems;
        QStringList issues;
        bool hasStockIssues = false;
        bool hasPriceChanges = false;
        unsigned long long totalPrice = 0;

        for (const QJsonValue &itemValue : itemsArray)
        {
            if (!itemValue.isObject())
                continue;

            QJsonObject itemObj = itemValue.toObject();
            unsigned int commodityId = static_cast<unsigned int>(itemObj["commodityId"].toInteger());
            unsigned int requestedQuantity = static_cast<unsigned int>(itemObj["quantity"].toInteger());
            double uiCurrentPrice = itemObj["uiCurrentPrice"].toDouble(); // UI显示的现价

            // 查询商品当前信息
            QSqlQuery query(db);
            query.prepare("SELECT name, originalPrice, discount, uniDiscount, stock, sourceStoreName FROM commodity_table WHERE id = :id");
            query.bindValue(":id", commodityId);

            if (!query.exec() || !query.next())
            {
                issues << QString("商品ID %1 不存在").arg(commodityId);
                continue;
            }

            QString name = query.value("name").toString();
            unsigned long long currentOriginalPrice = query.value("originalPrice").toULongLong();
            double currentDiscount = query.value("discount").toDouble();
            double currentUniDiscount = query.value("uniDiscount").toDouble();
            unsigned int currentStock = query.value("stock").toUInt();
            QString sourceStoreName = query.value("sourceStoreName").toString();

            // 检查库存
            if (requestedQuantity > currentStock)
            {
                hasStockIssues = true;
                issues << QString("商品 %1 库存不足（需要 %2，现有 %3）").arg(name).arg(requestedQuantity).arg(currentStock);
                continue;
            }

            // 计算当前数据库中的现价
            double currentPriceInYuan = (static_cast<double>(currentOriginalPrice) / 100.0) * currentDiscount * currentUniDiscount;

            // 检查现价是否发生变化（与用户UI上看到的现价比较）
            bool priceChanged = false;
            const double PRICE_TOLERANCE = 0.01; // 1分的容差

            if (qAbs(currentPriceInYuan - uiCurrentPrice) > PRICE_TOLERANCE)
            {
                priceChanged = true;
                hasPriceChanges = true;
                issues << QString("商品 %1 现价已变更：%2 元 -> %3 元")
                              .arg(name)
                              .arg(uiCurrentPrice, 0, 'f', 2)
                              .arg(currentPriceInYuan, 0, 'f', 2);
            }

            // 使用当前数据库价格计算总价
            unsigned long long itemPriceInFen = static_cast<unsigned long long>(currentOriginalPrice * currentDiscount * currentUniDiscount);
            totalPrice += itemPriceInFen * requestedQuantity;

            // 构建验证后的商品信息
            QJsonObject validatedItem;
            validatedItem["commodityId"] = static_cast<qint64>(commodityId);
            validatedItem["commodityName"] = name;
            validatedItem["quantity"] = static_cast<qint64>(requestedQuantity);
            validatedItem["originalPrice"] = static_cast<double>(currentOriginalPrice) / 100.0;
            validatedItem["discount"] = currentDiscount;
            validatedItem["uniDiscount"] = currentUniDiscount;
            validatedItem["currentStock"] = static_cast<qint64>(currentStock);
            validatedItem["sourceStoreName"] = sourceStoreName;
            validatedItem["priceChanged"] = priceChanged;
            validatedItems.append(validatedItem);
        }

        if (hasStockIssues)
        {
            response["status"] = "error";
            response["message"] = issues.join("\n");
        }
        else if (hasPriceChanges)
        {
            response["status"] = "price_changed";
            response["message"] = "商品现价发生变化，请确认是否继续";
            response["changes"] = issues.join("\n");
            response["validatedItems"] = validatedItems;
            response["totalPrice"] = static_cast<qint64>(totalPrice);
        }
        else
        {
            response["status"] = "success";
            response["message"] = "验证通过，可以创建订单";
            response["validatedItems"] = validatedItems;
            response["totalPrice"] = static_cast<qint64>(totalPrice);
        }
    }
    catch (const std::exception &e)
    {
        response["status"] = "error";
        response["message"] = QString("验证订单项时发生错误: %1").arg(e.what());
        qDebug() << "ConnectionsHandler: Exception in handleValidateOrderItemsRequest:" << e.what();
    }

    return response;
}
QJsonObject ConnectionsHandler::handleCreateOrderRequest(const QJsonObject &request)
{
    QJsonObject response;
    response["mtype"] = "create_order_response";

    QString username = request["username"].toString();
    unsigned long long totalPrice = static_cast<unsigned long long>(request["totalPrice"].toInteger());
    QJsonArray itemsArray = request["items"].toArray();

    qDebug() << "ConnectionsHandler: Handling create order request for user:" << username;

    try
    {
        QSqlDatabase db = QSqlDatabase::database();

        // 开始事务
        if (!db.transaction())
        {
            response["status"] = "error";
            response["message"] = "无法开始数据库事务";
            return response;
        }

        // 验证所有商品库存
        QList<QVariantMap> validatedItems;

        for (const QJsonValue &itemValue : itemsArray)
        {
            QJsonObject itemObj = itemValue.toObject();
            unsigned int commodityId = static_cast<unsigned int>(itemObj["commodityId"].toInteger());
            unsigned int quantity = static_cast<unsigned int>(itemObj["quantity"].toInteger());

            QSqlQuery stockQuery(db);
            stockQuery.prepare("SELECT stock FROM commodity_table WHERE id = :id");
            stockQuery.bindValue(":id", commodityId);

            if (!stockQuery.exec() || !stockQuery.next())
            {
                db.rollback();
                response["status"] = "error";
                response["message"] = QString("商品 %1 不存在").arg(commodityId);
                return response;
            }

            unsigned int currentStock = stockQuery.value("stock").toUInt();
            if (quantity > currentStock)
            {
                db.rollback();
                response["status"] = "error";
                response["message"] = QString("商品 %1 库存不足").arg(commodityId);
                return response;
            }

            QVariantMap validatedItem;
            validatedItem["commodityId"] = commodityId;
            validatedItem["commodityName"] = itemObj["commodityName"].toString();
            validatedItem["quantity"] = quantity;
            validatedItem["originalPrice"] = itemObj["originalPrice"].toDouble();
            validatedItem["discount"] = itemObj["discount"].toDouble();
            validatedItem["uniDiscount"] = itemObj["uniDiscount"].toDouble();
            validatedItems.append(validatedItem);
        }

        // 创建订单（状态为未支付）
        Order order(username, validatedItems, totalPrice, "unPaid");
        OrderDAO orderDAO;
        if (!orderDAO.saveOrder(order))
        {
            db.rollback();
            response["status"] = "error";
            response["message"] = "保存订单失败";
            return response;
        }

        // 更新商品库存
        for (const QJsonValue &itemValue : itemsArray)
        {
            QJsonObject itemObj = itemValue.toObject();
            unsigned int commodityId = static_cast<unsigned int>(itemObj["commodityId"].toInteger());
            unsigned int quantity = static_cast<unsigned int>(itemObj["quantity"].toInteger());

            QSqlQuery updateStockQuery(db);
            updateStockQuery.prepare("UPDATE commodity_table SET stock = stock - :quantity WHERE id = :commodityId");
            updateStockQuery.bindValue(":quantity", quantity);
            updateStockQuery.bindValue(":commodityId", commodityId);

            if (!updateStockQuery.exec())
            {
                db.rollback();
                response["status"] = "error";
                response["message"] = QString("更新商品 %1 库存失败").arg(commodityId);
                return response;
            }
        }

        // 清空用户购物车
        QSqlQuery clearCartQuery(db);
        clearCartQuery.prepare("DELETE FROM shoppingcart_table WHERE user = :username");
        clearCartQuery.bindValue(":username", username);

        if (!clearCartQuery.exec())
        {
            db.rollback();
            response["status"] = "error";
            response["message"] = "清空购物车失败";
            return response;
        }

        // 提交事务
        if (!db.commit())
        {
            db.rollback();
            response["status"] = "error";
            response["message"] = "提交事务失败";
            return response;
        }

        response["status"] = "success";
        response["message"] = "订单创建成功（未支付状态）";
        response["totalPrice"] = static_cast<qint64>(totalPrice);
        // 注意：不返回余额信息，因为没有扣款
    }
    catch (const std::exception &e)
    {
        response["status"] = "error";
        response["message"] = QString("创建订单时发生错误: %1").arg(e.what());
        qDebug() << "ConnectionsHandler: Exception in handleCreateOrderRequest:" << e.what();
    }

    return response;
}
QJsonObject ConnectionsHandler::handlePayOrderRequest(const QJsonObject &request)
{
    QJsonObject response;
    response["mtype"] = "pay_order_response";

    QString username = request["username"].toString();
    unsigned int orderId = static_cast<unsigned int>(request["orderId"].toInteger());

    qDebug() << "ConnectionsHandler: Handling pay order request for user:" << username << ", orderId:" << orderId;

    try
    {
        QSqlDatabase db = QSqlDatabase::database();

        // 开始事务
        if (!db.transaction())
        {
            response["status"] = "error";
            response["message"] = "无法开始数据库事务";
            return response;
        }

        // 1. 验证订单是否存在且属于指定用户
        QSqlQuery orderQuery(db);
        orderQuery.prepare("SELECT user, totalPrice, state FROM order_table WHERE id = :orderId");
        orderQuery.bindValue(":orderId", orderId);

        if (!orderQuery.exec() || !orderQuery.next())
        {
            db.rollback();
            response["status"] = "error";
            response["message"] = "订单不存在";
            return response;
        }

        QString orderOwner = orderQuery.value("user").toString();
        unsigned long long orderTotal = orderQuery.value("totalPrice").toULongLong();
        QString orderState = orderQuery.value("state").toString();

        if (orderOwner != username)
        {
            db.rollback();
            response["status"] = "error";
            response["message"] = "您无权支付此订单";
            return response;
        }

        if (orderState != "unPaid")
        {
            db.rollback();
            response["status"] = "error";
            response["message"] = QString("订单状态为 %1，无法支付").arg(orderState);
            return response;
        }

        // 2. 使用UserDAO检查用户余额
        unsigned int userId = 0;
        unsigned long long userBalance = 0;
        if (!UserDAO::getUserIdAndBalanceByUsername(username, &userId, &userBalance))
        {
            db.rollback();
            response["status"] = "error";
            response["message"] = "无法获取用户余额信息";
            return response;
        }

        if (userBalance < orderTotal)
        {
            db.rollback();
            response["status"] = "error";
            response["message"] = QString("余额不足：需要 %1 元，现有 %2 元")
                                      .arg(orderTotal / 100.0, 0, 'f', 2)
                                      .arg(userBalance / 100.0, 0, 'f', 2);
            return response;
        }

        // 3. 获取订单项目以计算商家收入
        QSqlQuery itemsQuery(db);
        itemsQuery.prepare("SELECT commodityId, quantity FROM orderitem_table WHERE id = :orderId");
        itemsQuery.bindValue(":orderId", orderId);

        if (!itemsQuery.exec())
        {
            db.rollback();
            response["status"] = "error";
            response["message"] = "无法获取订单项目信息";
            return response;
        }

        QMap<QString, unsigned long long> merchantPayments; // 商家名 -> 应收金额

        while (itemsQuery.next())
        {
            unsigned int commodityId = itemsQuery.value("commodityId").toUInt();
            unsigned int quantity = itemsQuery.value("quantity").toUInt();

            // 查询商品信息
            QSqlQuery commodityQuery(db);
            commodityQuery.prepare("SELECT sourceStoreName, originalPrice, discount, uniDiscount FROM commodity_table WHERE id = :commodityId");
            commodityQuery.bindValue(":commodityId", commodityId);

            if (!commodityQuery.exec() || !commodityQuery.next())
            {
                db.rollback();
                response["status"] = "error";
                response["message"] = QString("无法获取商品 %1 的信息").arg(commodityId);
                return response;
            }

            QString merchantName = commodityQuery.value("sourceStoreName").toString();
            unsigned long long originalPrice = commodityQuery.value("originalPrice").toULongLong();
            double discount = commodityQuery.value("discount").toDouble();
            double uniDiscount = commodityQuery.value("uniDiscount").toDouble();

            // 计算该商品的总金额
            unsigned long long itemTotal = static_cast<unsigned long long>(originalPrice * discount * uniDiscount * quantity);
            merchantPayments[merchantName] += itemTotal;
        }

        // 4. 使用UserDAO扣除买家余额
        unsigned long long newBuyerBalance = 0;
        if (!UserDAO::updateUserBalance(userId, -static_cast<qint64>(orderTotal), &newBuyerBalance))
        {
            db.rollback();
            response["status"] = "error";
            response["message"] = "扣除买家余额失败";
            return response;
        }

        // 5. 使用UserDAO增加各商家余额
        for (auto it = merchantPayments.constBegin(); it != merchantPayments.constEnd(); ++it)
        {
            QString merchantName = it.key();
            unsigned long long amount = it.value();

            // 获取商家的userId
            unsigned int merchantUserId = UserDAO::getUserIdByUsername(merchantName);
            if (merchantUserId == 0)
            {
                db.rollback();
                response["status"] = "error";
                response["message"] = QString("商家 %1 不存在").arg(merchantName);
                return response;
            }

            // 使用UserDAO更新商家余额
            if (!UserDAO::updateUserBalance(merchantUserId, static_cast<qint64>(amount), nullptr))
            {
                db.rollback();
                response["status"] = "error";
                response["message"] = QString("更新商家 %1 余额失败").arg(merchantName);
                return response;
            }

            qDebug() << "ConnectionsHandler: Updated merchant" << merchantName << "balance by +" << amount;
        }

        // 6. 更新订单状态为已支付
        QSqlQuery updateOrderQuery(db);
        updateOrderQuery.prepare("UPDATE order_table SET state = 'paid' WHERE id = :orderId");
        updateOrderQuery.bindValue(":orderId", orderId);

        if (!updateOrderQuery.exec())
        {
            db.rollback();
            response["status"] = "error";
            response["message"] = "更新订单状态失败";
            return response;
        }

        // 7. 提交事务
        if (!db.commit())
        {
            db.rollback();
            response["status"] = "error";
            response["message"] = "提交事务失败";
            return response;
        }

        // 8. 返回成功响应
        response["status"] = "success";
        response["message"] = "订单支付成功";
        response["orderId"] = static_cast<qint64>(orderId);
        response["paidAmount"] = static_cast<qint64>(orderTotal);
        response["newBalance"] = static_cast<qint64>(newBuyerBalance);

        qDebug() << "ConnectionsHandler: Order" << orderId << "paid successfully. Amount:" << orderTotal << ", New balance:" << newBuyerBalance;
    }
    catch (const std::exception &e)
    {
        response["status"] = "error";
        response["message"] = QString("支付订单时发生错误: %1").arg(e.what());
        qDebug() << "ConnectionsHandler: Exception in handlePayOrderRequest:" << e.what();
    }

    return response;
}
QJsonObject ConnectionsHandler::handleDeleteCommodityRequest(const QJsonObject &request)
{
    QJsonObject response;
    response["mtype"] = "delete_commodity_response";

    unsigned int commodityId = static_cast<unsigned int>(request["commodityId"].toInteger());
    QString sourceStoreName = request["sourceStoreName"].toString();

    qDebug() << "ConnectionsHandler: Handling delete commodity request. ID:" << commodityId
             << ", Store:" << sourceStoreName;

    // 参数验证
    if (commodityId == 0 || sourceStoreName.isEmpty())
    {
        response["status"] = "error";
        response["message"] = "无效的请求参数";
        return response;
    }

    // 调用业务逻辑层处理删除商品
    bool success = Commodity::deleteCommodity(commodityId, sourceStoreName);

    if (success)
    {
        response["status"] = "success";
        response["message"] = "商品删除成功！已清理相关的购物车和订单项目";
        response["commodityId"] = static_cast<qint64>(commodityId);

        qDebug() << "ConnectionsHandler: Commodity deletion successful. ID:" << commodityId;
    }
    else
    {
        response["status"] = "error";
        response["message"] = "商品删除失败！请检查商品是否存在或您是否有删除权限";

        qDebug() << "ConnectionsHandler: Commodity deletion failed. ID:" << commodityId;
    }

    return response;
}
QJsonObject ConnectionsHandler::handleGetUserUniDiscountRequest(const QJsonObject &request)
{
    QJsonObject response;
    response["mtype"] = "get_user_uni_discount_response";

    QString username = request["username"].toString();

    QMap<QString, float> uniDiscounts = UserDAO::getUserAllUniDiscounts(username);

    QJsonObject discountObj;
    for (auto it = uniDiscounts.constBegin(); it != uniDiscounts.constEnd(); ++it)
    {
        discountObj[it.key()] = static_cast<double>(it.value());
    }

    response["status"] = "success";
    response["username"] = username;
    response["uniDiscounts"] = discountObj;

    return response;
}
