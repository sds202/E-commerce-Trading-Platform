#include "BLL_commodity.h"

Commodity::Commodity()
    : id(0), name(""), type(""), description(""),
      originalPrice(0), stock(0), sourceStoreName(""),
      discount(1.0f), uniDiscount(1.0f)
{
}

Commodity::Commodity(unsigned int id, const QString &name, const QString &type, const QString &description,
                     unsigned long long originalPrice, unsigned int stock, const QString &sourceStoreName,
                     float discount, float uniDiscount)
    : id(id), name(name), type(type), description(description),
      originalPrice(originalPrice), stock(stock), sourceStoreName(sourceStoreName),
      discount(discount), uniDiscount(uniDiscount)
{
}

void Commodity::send_fetchAllInformation(unsigned int id, int page)
{
    QJsonObject jsonObj;
    jsonObj["mtype"] = "fetch_all_info_of_commodity";
    jsonObj["id"] = static_cast<qint64>(id);
    jsonObj["page"] = page;

    HandleMessage::sendJsonData(&tcpSocket, jsonObj);
}

void Commodity::updateCommodity(unsigned int id, const QString &name, const QString &type, const QString &description,
                                unsigned long long originalPrice, unsigned int stock, const QString &sourceStoreName,
                                double discount, double uniDiscount)
{
    QJsonObject jsonObj;
    jsonObj["mtype"] = "update_commodity";
    jsonObj["id"] = static_cast<qint64>(id);
    jsonObj["name"] = name;
    jsonObj["type"] = type;
    jsonObj["description"] = description;
    jsonObj["originalPrice"] = static_cast<qint64>(originalPrice);
    jsonObj["stock"] = static_cast<qint64>(stock);
    jsonObj["sourceStoreName"] = sourceStoreName;
    jsonObj["discount"] = discount;
    jsonObj["uniDiscount"] = uniDiscount;

    qDebug() << "User::updateCommodity: Sending update commodity request for ID:" << id;
    HandleMessage::sendJsonData(&tcpSocket, jsonObj);
}

void Commodity::newCommodity(const QString &name, const QString &type, const QString &description,
                             unsigned long long originalPrice, unsigned int stock, const QString &sourceStoreName,
                             double discount, double uniDiscount)
{
    QJsonObject jsonObj;
    jsonObj["mtype"] = "new_commodity";
    jsonObj["name"] = name;
    jsonObj["type"] = type;
    jsonObj["description"] = description;
    jsonObj["originalPrice"] = static_cast<qint64>(originalPrice);
    jsonObj["stock"] = static_cast<qint64>(stock);
    jsonObj["sourceStoreName"] = sourceStoreName;
    jsonObj["discount"] = discount;
    jsonObj["uniDiscount"] = uniDiscount;

    qDebug() << "User::newCommodity: Sending new commodity request for ID:" << id;
    HandleMessage::sendJsonData(&tcpSocket, jsonObj);
}

unsigned long long Commodity::Getprice() const
{
    return static_cast<unsigned long long>(originalPrice * discount * uniDiscount);
}

unsigned long long Book::Getprice() const
{
    return static_cast<unsigned long long>(originalPrice * discount * uniDiscount);
}

unsigned long long Food::Getprice() const
{
    return static_cast<unsigned long long>(originalPrice * discount * uniDiscount);
}

unsigned long long Clothing::Getprice() const
{
    return static_cast<unsigned long long>(originalPrice * discount * uniDiscount);
}

void Commodity::setStock(unsigned int newCommodityStock)
{
    stock = newCommodityStock;
}

void Commodity::setUniDiscount(float uniDiscount, const QString &sourceStoreName, const QString &type)
{
    if (uniDiscount <= 0 || uniDiscount > 1)
    {
        qDebug() << "无效折扣值：" << uniDiscount << "，必须在(0,1]区间";
        return;
    }

    QJsonObject jsonObj;
    jsonObj["mtype"] = "set_uni_discount";
    jsonObj["uniDiscount"] = uniDiscount;
    jsonObj["sourceStoreName"] = sourceStoreName;
    jsonObj["type"] = type; // 商品的种类

    qDebug() << "User::setUniDiscount: Sending set uniDiscount request for store:" << sourceStoreName
             << ", type:" << type << ", uniDiscount:" << uniDiscount;
    HandleMessage::sendJsonData(&tcpSocket, jsonObj);
}

void Commodity::deleteCommodityFromServer()
{
    QJsonObject request;
    request["mtype"] = "delete_commodity";
    request["commodityId"] = static_cast<qint64>(this->id);
    request["sourceStoreName"] = this->sourceStoreName;

    qDebug() << "Commodity: Sending delete commodity request. ID:" << this->id
             << ", Store:" << this->sourceStoreName;

    HandleMessage::sendJsonData(&tcpSocket, request);
}
