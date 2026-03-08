#include "BLL_Order.h"

Order::Order()
    : orderId(0), userName(""), totalPrice(0), state("unPaid") {}

Order::Order(const QString &userName, const QList<QVariantMap> &items, unsigned long long totalPrice, const QString &state)
    : orderId(0),
      userName(userName),
      items(items),
      totalPrice(totalPrice),
      state(state),
      orderTime(QDateTime::currentDateTime().toString()) {}

Order::~Order() {}

unsigned int Order::getOrderId() const
{
    return orderId;
}

QString Order::getUserName() const
{
    return userName;
}

QList<QVariantMap> Order::getItems() const
{
    return items;
}

unsigned long long Order::getTotalPrice() const
{
    return totalPrice;
}

QString Order::getState() const
{
    return state;
}

QString Order::getOrderTime() const
{
    return orderTime;
}

void Order::setState(const QString &state)
{
    this->state = state;
}

void Order::setOrderId(unsigned int id)
{
    this->orderId = id;
}

void Order::setOrderTime(const QString &time)
{
    this->orderTime = time;
}

void Order::fetchOrdersFromServer(const QString &userName)
{
    QJsonObject request;
    request["mtype"] = "fetch_orders";
    request["username"] = userName;

    qDebug() << "Order: Sending fetch orders request for user:" << userName;
    HandleMessage::sendJsonData(&tcpSocket, request);
}

void Order::deleteOrderFromServer(const QString &userName, unsigned int orderId)
{
    QJsonObject request;
    request["mtype"] = "delete_order";
    request["username"] = userName;
    request["orderId"] = static_cast<qint64>(orderId);

    qDebug() << "Order: Sending delete order request for user:" << userName << ", orderId:" << orderId;
    HandleMessage::sendJsonData(&tcpSocket, request);
}

void Order::createOrderFromServer(const QString &userName, const QList<QVariantMap> &orderItems, unsigned long long totalPrice)
{
    QJsonObject request;
    request["mtype"] = "create_order";
    request["username"] = userName;
    request["totalPrice"] = static_cast<qint64>(totalPrice);

    QJsonArray itemsArray;
    for (const QVariantMap &item : orderItems)
    {
        QJsonObject itemObj;
        itemObj["commodityId"] = static_cast<qint64>(item["commodityId"].toUInt());
        itemObj["commodityName"] = item["commodityName"].toString();
        itemObj["quantity"] = static_cast<qint64>(item["quantity"].toUInt());
        itemObj["originalPrice"] = item["originalPrice"].toDouble();
        itemObj["discount"] = item["discount"].toDouble();
        itemObj["uniDiscount"] = item["uniDiscount"].toDouble();
        itemsArray.append(itemObj);
    }
    request["items"] = itemsArray;

    qDebug() << "----Order: Sending create order request for user:" << userName << ", totalPrice:" << totalPrice;
    HandleMessage::sendJsonData(&tcpSocket, request);
}

void Order::payOrderFromServer(const QString &userName, unsigned int orderId)
{
    QJsonObject request;
    request["mtype"] = "pay_order";
    request["username"] = userName;
    request["orderId"] = static_cast<qint64>(orderId);

    qDebug() << "Order: Sending pay order request for user:" << userName << ", orderId:" << orderId;
    HandleMessage::sendJsonData(&tcpSocket, request);
}
