#include "BLL_user.h"

User::User()
    : name(""), password(""), balance(0), type(""), shoppingCart(nullptr), orders()
{
}

User::User(const QString &name, double balance, const QString &type, const QString &password)
    : name(name), password(password), balance(static_cast<unsigned long long>(balance * 100)),
      type(type), shoppingCart(new ShoppingCart(name, this)), orders()
{
}

User::User(const QString &name, const QString &password, const QString &type, unsigned long long balance)
    : name(name), password(password), balance(balance),
      type(type)
{
}

User::~User()
{
    delete shoppingCart;
    qDeleteAll(orders);
    orders.clear();
}

unsigned long long User::getBalance() const
{
    return balance;
}

QString User::getType() const
{
    return type;
}

QString User::getName() const
{
    return name;
}

QString User::getPassword() const
{
    return password;
}

ShoppingCart *User::getShoppingCart() const
{
    return shoppingCart;
}

void User::setBalance(double balance)
{
    this->balance = static_cast<unsigned long long>(balance * 100);
}

void User::setBalanceWithout100(double balance)
{
    this->balance = static_cast<unsigned long long>(balance);
}

void User::changePassword(const QString &newPassword)
{

    if (name.isEmpty())
    {
        qDebug() << "User::changePassword: Current user name is empty, cannot change password.";
        return;
    }

    QJsonObject jsonObj;
    jsonObj["mtype"] = "change_password";
    jsonObj["username"] = name; // 使用当前用户的用户名
    jsonObj["newPassword"] = newPassword;

    qDebug() << "User::changePassword: Sending change password request for user:" << name;
    HandleMessage::sendJsonData(&tcpSocket, jsonObj);
}

void User::addOrder(Order *order)
{
    if (order && !orders.contains(order))
    {
        orders.append(order);
        qDebug() << "User: Added order" << order->getOrderId() << "to user" << name;
    }
}

void User::userLogin(const QString &username, const QString &password) // m
{
    QJsonObject jsonObj;
    jsonObj["mtype"] = "login";
    jsonObj["username"] = username;
    jsonObj["password"] = password;

    HandleMessage::sendJsonData(&tcpSocket, jsonObj);
}

void User::userRegister(const QString &username, const QString &password, const QString &type) // m
{
    QJsonObject jsonObj;
    jsonObj["mtype"] = "register";
    jsonObj["username"] = username;
    jsonObj["password"] = password;
    jsonObj["user_type"] = type;

    HandleMessage::sendJsonData(&tcpSocket, jsonObj);
}

void User::purchase(unsigned long long totalPrice, const QString &username, unsigned int id, unsigned int purchaseQuantity, const QString &sourceStoreName) // m
{
    QJsonObject jsonObj;
    jsonObj["mtype"] = "purchase";
    jsonObj["buyerUsername"] = username;
    jsonObj["commodityId"] = static_cast<qint64>(id);
    jsonObj["purchaseQuantity"] = static_cast<qint64>(purchaseQuantity);
    jsonObj["sellerStoreName"] = sourceStoreName;
    jsonObj["totalPrice"] = static_cast<qint64>(totalPrice);

    qDebug() << "User::purchase: Sending purchase request for buyer" << username
             << ", commodity ID:" << id << ", quantity:" << purchaseQuantity
             << ", seller store:" << sourceStoreName << ", total price:" << totalPrice;
    HandleMessage::sendJsonData(&tcpSocket, jsonObj);
}

void User::removeOrder(unsigned int orderId)
{
    for (int i = 0; i < orders.size(); ++i)
    {
        if (orders[i]->getOrderId() == orderId)
        {
            delete orders[i];
            orders.removeAt(i);
            qDebug() << "User: Removed order" << orderId << "from user" << name;
            break;
        }
    }
}

void User::recharge(unsigned long long amount)
{
    if (name.isEmpty())
    {
        qDebug() << "User::recharge: Current user name is empty, cannot recharge.";
        return;
    }
    if (amount <= 0)
    {
        qDebug() << "User::recharge: Recharge amount must be positive.";
        return;
    }

    QJsonObject jsonObj;
    jsonObj["mtype"] = "recharge";
    jsonObj["username"] = name;
    jsonObj["amount"] = static_cast<qint64>(amount);

    qDebug() << "User::recharge: Sending recharge request for user:" << name << ", amount:" << amount;
    HandleMessage::sendJsonData(&tcpSocket, jsonObj);
}

QList<Order *> User::getOrders() const
{
    return orders;
}

Order *User::getOrderById(unsigned int orderId) const
{
    for (Order *order : orders)
    {
        if (order->getOrderId() == orderId)
        {
            return order;
        }
    }
    return nullptr;
}

void User::clearOrders()
{
    qDeleteAll(orders);
    orders.clear();
    qDebug() << "User: Cleared all orders for user" << name;
}
