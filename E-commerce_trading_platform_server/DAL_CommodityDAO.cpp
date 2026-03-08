#include "DAL_CommodityDAO.h"

bool CommodityDAO::FetchAllCommodities(const QString &storeName, QJsonArray &commoditiesArray)
{
    QSqlQuery query;
    QString queryString;

    if (storeName.isEmpty())
    {
        queryString = "SELECT id, name, type, description, originalPrice, stock, sourceStoreName, discount, uniDiscount FROM commodity_table";
        query.prepare(queryString);
    }
    else
    {
        // 请求特定商家的商品 (商品管理页)
        queryString = "SELECT id, name, type, description, originalPrice, stock, sourceStoreName, discount, uniDiscount FROM commodity_table WHERE sourceStoreName = :storeName";
        query.prepare(queryString);
        query.bindValue(":storeName", storeName);
    }

    // 3. 执行查询
    if (!query.exec())
    {
        qDebug() << "Server: Query failed for fetch commodities:" << query.lastError().text();
        return false;
    }

    // 4. 处理查询结果
    while (query.next())
    {
        QJsonObject commodity;
        commodity["id"] = static_cast<qint64>(query.value("id").toUInt());
        commodity["name"] = query.value("name").toString();
        commodity["type"] = query.value("type").toString();
        commodity["description"] = query.value("description").toString();
        commodity["originalPrice"] = query.value("originalPrice").toLongLong(); // 对应客户端的 toLongLong
        commodity["stock"] = static_cast<qint64>(query.value("stock").toUInt());
        commodity["sourceStoreName"] = query.value("sourceStoreName").toString();
        commodity["discount"] = query.value("discount").toFloat();       // 对应客户端的 toFloat
        commodity["uniDiscount"] = query.value("uniDiscount").toFloat(); // 假设也是 float

        commoditiesArray.append(commodity);
    }

    return true;
}
QVariantMap CommodityDAO::fetchCommodityDetailsById(unsigned int commodityId)
{
    QVariantMap commodityData;
    QSqlQuery query;

    // 假设你的商品表名为 commodity_table
    // 并且包含 id, name, type, description, originalPrice, stock, sourceStoreName, discount, uniDiscount 等字段
    QString queryString = "SELECT id, name, type, description, originalPrice, stock, sourceStoreName, discount, uniDiscount FROM commodity_table WHERE id = :id";
    query.prepare(queryString);
    query.bindValue(":id", static_cast<qint64>(commodityId)); // 绑定参数时也用 qint64

    if (!query.exec())
    {
        qDebug() << "DAO: Failed to fetch commodity details for ID" << commodityId << ":" << query.lastError().text();
        return commodityData; // 返回空 map 表示失败
    }

    if (query.next())
    { // 如果找到记录
        commodityData["id"] = query.value("id").toUInt();
        commodityData["name"] = query.value("name").toString();
        commodityData["type"] = query.value("type").toString();
        commodityData["description"] = query.value("description").toString();
        commodityData["originalPrice"] = query.value("originalPrice").toLongLong();
        commodityData["stock"] = query.value("stock").toUInt();
        commodityData["sourceStoreName"] = query.value("sourceStoreName").toString();
        commodityData["discount"] = query.value("discount").toFloat();
        commodityData["uniDiscount"] = query.value("uniDiscount").toFloat();
        // 添加其他你需要的字段
    }
    else
    {
        qDebug() << "DAO: Commodity with ID" << commodityId << "not found.";
    }

    return commodityData;
}
bool CommodityDAO::getCommodityStock(unsigned int commodityId, unsigned int *outStock)
{
    QSqlQuery query;
    query.prepare("SELECT stock FROM commodity_table WHERE id = :commodityId");
    query.bindValue(":commodityId", static_cast<qint64>(commodityId));

    if (!query.exec())
    {
        qDebug() << "CommodityDAO: Failed to get commodity stock for ID" << commodityId << ":" << query.lastError().text();
        return false;
    }

    if (query.next())
    {
        if (outStock)
            *outStock = query.value("stock").toUInt();
        return true;
    }
    qDebug() << "CommodityDAO: Commodity with ID" << commodityId << "not found for stock query.";
    return false;
}

bool CommodityDAO::updateCommodityStock(unsigned int commodityId, qint64 quantityChange, unsigned int *outNewStock)
{
    QSqlQuery query;
    query.prepare("UPDATE commodity_table SET stock = stock + :quantityChange WHERE id = :commodityId");
    query.bindValue(":quantityChange", quantityChange);
    query.bindValue(":commodityId", static_cast<qint64>(commodityId));

    if (!query.exec())
    {
        qDebug() << "CommodityDAO: Failed to update stock for commodity" << commodityId << ", change" << quantityChange << ":" << query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() == 0)
    {
        qDebug() << "CommodityDAO: Commodity with ID" << commodityId << "not found for stock update.";
        return false;
    }

    // 如果需要返回新的库存值
    if (outNewStock)
    {
        QSqlQuery selectQuery;
        selectQuery.prepare("SELECT stock FROM commodity_table WHERE id = :commodityId");
        selectQuery.bindValue(":commodityId", static_cast<qint64>(commodityId));
        if (selectQuery.exec() && selectQuery.next())
        {
            *outNewStock = selectQuery.value("stock").toUInt();
        }
        else
        {
            qDebug() << "CommodityDAO: Failed to fetch new stock after update.";
            // 即使更新成功，获取新库存失败也算失败，或者根据需求决定是否返回true
            return false;
        }
    }
    return true;
}
bool CommodityDAO::updateCommodity(unsigned int id, const QString &name, const QString &type, const QString &description,
                                   unsigned long long originalPrice, unsigned int stock, const QString &sourceStoreName,
                                   float discount, float uniDiscount)
{
    QSqlQuery query;
    query.prepare("UPDATE commodity_table SET name = :name, type = :type, description = :description, "
                  "originalPrice = :originalPrice, stock = :stock, sourceStoreName = :sourceStoreName, "
                  "discount = :discount, uniDiscount = :uniDiscount WHERE id = :id");

    query.bindValue(":name", name);
    query.bindValue(":type", type);
    query.bindValue(":description", description);
    query.bindValue(":originalPrice", static_cast<qint64>(originalPrice));
    query.bindValue(":stock", static_cast<qint64>(stock));
    query.bindValue(":sourceStoreName", sourceStoreName);
    query.bindValue(":discount", discount);
    query.bindValue(":uniDiscount", uniDiscount);
    query.bindValue(":id", static_cast<qint64>(id));

    if (!query.exec())
    {
        qDebug() << "CommodityDAO: Failed to update commodity" << id << ":" << query.lastError().text();
        return false;
    }

    // 检查是否真的有行被更新，如果没有可能是 id 不存在
    if (query.numRowsAffected() == 0)
    {
        qDebug() << "CommodityDAO: Commodity with ID" << id << "not found or no changes made.";
        return false;
    }
    return true;
}

bool CommodityDAO::newCommodity(const QString &name, const QString &type, const QString &description,
                                unsigned long long originalPrice, unsigned int stock, const QString &sourceStoreName,
                                float discount, float uniDiscount)
{
    QSqlQuery query;
    // 完整的 INSERT 语句，使用 SET 语法
    query.prepare("INSERT INTO commodity_table SET "
                  "name = :name, "
                  "type = :type, "
                  "description = :description, "
                  "originalPrice = :originalPrice, "
                  "stock = :stock, "
                  "sourceStoreName = :sourceStoreName, "
                  "discount = :discount, "
                  "uniDiscount = :uniDiscount");

    // 绑定参数值
    // 注意：Qt 的 QVariant 转换，通常建议将 C++ 的整数类型转换为 qint64
    // 即使是 unsigned int 或 unsigned long long，为了兼容性和避免溢出问题，
    // 转换为 qint64 (如果值在 qint64 范围内) 是安全的做法。
    query.bindValue(":name", name);
    query.bindValue(":type", type);
    query.bindValue(":description", description);
    query.bindValue(":originalPrice", static_cast<qint64>(originalPrice));
    query.bindValue(":stock", static_cast<qint64>(stock));
    query.bindValue(":sourceStoreName", sourceStoreName);
    query.bindValue(":discount", discount);
    query.bindValue(":uniDiscount", uniDiscount);

    // 执行查询
    if (!query.exec())
    {
        qDebug() << "CommodityDAO: Failed to insert new commodity:" << query.lastError().text();
        return false; // 执行失败，返回 false
    }

    return true; // 执行成功，返回 true
}
bool CommodityDAO::updateCommodityUniDiscount(const QString &sourceStoreName, const QString &type, float uniDiscount)
{
    QSqlQuery query;
    query.prepare("UPDATE commodity_table SET uniDiscount = :uniDiscount "
                  "WHERE sourceStoreName = :sourceStoreName AND type = :type");

    query.bindValue(":uniDiscount", uniDiscount);
    query.bindValue(":sourceStoreName", sourceStoreName);
    query.bindValue(":type", type);

    if (!query.exec())
    {
        qDebug() << "CommodityDAO: Failed to update uniDiscount for store" << sourceStoreName
                 << "type" << type << ":" << query.lastError().text();
        return false;
    }

    return true;
}
bool CommodityDAO::deleteCommodity(unsigned int commodityId, const QString &sourceStoreName)
{
    QSqlDatabase db = QSqlDatabase::database();

    try
    {
        // 开始事务
        if (!db.transaction())
        {
            qDebug() << "CommodityDAO: Failed to start transaction for delete commodity";
            return false;
        }

        // 1. 验证商品是否存在且属于指定商家
        QSqlQuery checkQuery(db);
        checkQuery.prepare("SELECT name, sourceStoreName FROM commodity_table WHERE id = :commodityId");
        checkQuery.bindValue(":commodityId", commodityId);

        if (!checkQuery.exec() || !checkQuery.next())
        {
            db.rollback();
            qDebug() << "CommodityDAO: Commodity not found, ID:" << commodityId;
            return false;
        }

        QString commodityName = checkQuery.value("name").toString();
        QString commodityStore = checkQuery.value("sourceStoreName").toString();

        if (commodityStore != sourceStoreName)
        {
            db.rollback();
            qDebug() << "CommodityDAO: Permission denied. Commodity belongs to" << commodityStore
                     << "but request from" << sourceStoreName;
            return false;
        }

        // 2. 删除相关的购物车项目
        QSqlQuery deleteCartQuery(db);
        deleteCartQuery.prepare("DELETE FROM shoppingcart_table WHERE commodityId = :commodityId");
        deleteCartQuery.bindValue(":commodityId", commodityId);

        if (!deleteCartQuery.exec())
        {
            db.rollback();
            qDebug() << "CommodityDAO: Failed to delete cart items for commodity" << commodityId;
            return false;
        }

        int cartItemsDeleted = deleteCartQuery.numRowsAffected();
        qDebug() << "CommodityDAO: Deleted" << cartItemsDeleted << "cart items for commodity" << commodityId;

        // 4. 删除商品本身
        QSqlQuery deleteCommodityQuery(db);
        deleteCommodityQuery.prepare("DELETE FROM commodity_table WHERE id = :commodityId");
        deleteCommodityQuery.bindValue(":commodityId", commodityId);

        if (!deleteCommodityQuery.exec())
        {
            db.rollback();
            qDebug() << "CommodityDAO: Failed to delete commodity" << commodityId;
            return false;
        }

        // 5. 提交事务
        if (!db.commit())
        {
            db.rollback();
            qDebug() << "CommodityDAO: Failed to commit transaction for delete commodity" << commodityId;
            return false;
        }

        qDebug() << "CommodityDAO: Successfully deleted commodity" << commodityId
                 << "with" << cartItemsDeleted << "cart items ";
        return true;
    }
    catch (const std::exception &e)
    {
        db.rollback();
        qDebug() << "CommodityDAO: Exception in deleteCommodity:" << e.what();
        return false;
    }
}
