#include "BLL_Order.h"
#include "DAL_OrderDAO.h"

Order::Order() : orderId(0), totalPrice(0)
{
}

Order::Order(const QString &userName, const QList<QVariantMap> &items, unsigned long long totalPrice, const QString &state)
    : orderId(0), userName(userName), items(items), totalPrice(totalPrice), state(state)
{
    orderTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
}

Order::~Order()
{
}

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

bool Order::saveToDatabase() const
{
    OrderDAO orderDAO;
    return orderDAO.saveOrder(*this);
}

QList<Order> Order::fetchOrdersByUser(const QString &userName)
{
    OrderDAO orderDAO;
    return orderDAO.fetchOrdersByUser(userName);
}

bool Order::updateStateInDatabase() const
{
    OrderDAO orderDAO;
    return orderDAO.updateOrderState(orderId, state);
}
