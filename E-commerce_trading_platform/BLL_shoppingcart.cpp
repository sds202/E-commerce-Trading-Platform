#include "BLL_ShoppingCart.h"
#include "BLL_commodity.h"

ShoppingCart::ShoppingCart(const QString &userName, QObject *parent)
    : QObject(parent), userName(userName)
{
    connect(m_handleMessage, &HandleMessage::cartFetched, this, &ShoppingCart::onCartFetched);
}
ShoppingCart::~ShoppingCart()
{
}

bool ShoppingCart::addItem(unsigned int commodityId, int quantity, QString &errorMessage)
{
    QJsonObject request;
    request["mtype"] = "add_to_cart";
    request["username"] = userName;
    request["commodityId"] = static_cast<qint64>(commodityId);
    request["quantity"] = quantity;

    HandleMessage::sendJsonData(&tcpSocket, request);

    return true;
}

bool ShoppingCart::removeItem(unsigned int cartItemId, QString &errorMessage)
{
    QJsonObject request;
    request["mtype"] = "remove_cart_item";
    request["username"] = userName;
    request["cartItemId"] = static_cast<qint64>(cartItemId);

    HandleMessage::sendJsonData(&tcpSocket, request);

    return true;
}

void ShoppingCart::updateItemQuantity(unsigned int id, int quantity, QString &errorMessage)
{
    if (quantity < 0)
    {
        errorMessage = "数量无效，必须大于等于 0！";
        return;
    }

    QJsonObject jsonObj;
    jsonObj["mtype"] = "update_cart_item_quantity";
    jsonObj["username"] = userName;
    jsonObj["cartItemId"] = static_cast<qint64>(id);
    jsonObj["quantity"] = quantity;

    qDebug() << "ShoppingCart: Sending update_cart_item_quantity request for user" << userName
             << "cartItemId:" << id << "quantity:" << quantity;

    HandleMessage::sendJsonData(&tcpSocket, jsonObj);
}

QList<CartItem> ShoppingCart::getItems() const
{
    return items;
}

bool ShoppingCart::clear(QString &errorMessage)
{
    QJsonObject request;
    request["mtype"] = "clear_cart";
    request["username"] = userName;

    HandleMessage::sendJsonData(&tcpSocket, request);

    return true;
}

void ShoppingCart::loadFromDatabase()
{
    QJsonObject jsonObj;
    jsonObj["mtype"] = "fetch_cart";
    jsonObj["username"] = userName;

    HandleMessage::sendJsonData(&tcpSocket, jsonObj);
}

void ShoppingCart::onCartFetched(bool success, const QString &message, const QJsonArray &cartItems)
{
    if (!success)
    {
        items.clear(); // 清空本地购物车数据
        emit cartUpdated(success, message, items, cartItems);
        return;
    }

    // 更新本地 items
    items.clear();
    for (const QJsonValue &value : cartItems)
    {
        QJsonObject item = value.toObject();
        CartItem cartItem;
        cartItem.id = item["cartItemId"].toInt();
        cartItem.commodityId = item["commodityId"].toInt();
        cartItem.quantity = item["quantity"].toInt();
        items.append(cartItem);
    }

    emit cartUpdated(success, message, items, cartItems);
}

void ShoppingCart::removeItemByCommodityId(unsigned int commodityId)
{
    for (auto it = items.begin(); it != items.end(); ++it)
    {
        if (it->commodityId == commodityId)
        {
            items.erase(it);
            qDebug() << "ShoppingCart: Removed item with commodity ID:" << commodityId;
            return;
        }
    }
    qDebug() << "ShoppingCart: No item found with commodity ID:" << commodityId;
}

void ShoppingCart::updateFromServerResponse(const QJsonArray &updatedCartItems)
{
    items.clear(); // 清空现有数据

    for (const QJsonValue &val : updatedCartItems)
    {
        if (val.isObject())
        {
            QJsonObject itemObj = val.toObject();
            CartItem item;
            item.id = itemObj["cartItemId"].toInt();
            item.commodityId = itemObj["commodityId"].toInt();
            item.quantity = itemObj["quantity"].toInt();
            items.append(item);
        }
    }
}
