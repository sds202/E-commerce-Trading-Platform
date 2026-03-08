#include "DAL_ShoppingCartDAO.h"

#include "All.h"

bool ShoppingCartDAO::fetchCartItems(const QString &username, QJsonArray &cartItems, QString &errorMessage)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);

    query.prepare(
        "SELECT sc.id AS cart_item_id, sc.commodityId, sc.quantity, "
        "c.name AS commodityName, c.type, c.description, "
        "c.originalPrice, c.stock, c.sourceStoreName, "
        "c.discount, c.uniDiscount "
        "FROM shoppingcart_table sc "
        "JOIN commodity_table c ON sc.commodityId = c.id "
        "WHERE sc.user = :username");
    query.bindValue(":username", username);

    if (!query.exec())
    {
        errorMessage = "Database query failed: " + query.lastError().text();
        return false;
    }

    while (query.next())
    {
        QJsonObject item;
        item["cartItemId"] = query.value("cart_item_id").toInt();
        item["commodityId"] = query.value("commodityId").toInt();
        item["quantity"] = query.value("quantity").toInt();
        item["commodityName"] = query.value("commodityName").toString();

        // Add additional commodity details
        item["type"] = query.value("type").toString();
        item["description"] = query.value("description").toString();
        unsigned long long dbOriginalPrice = query.value("originalPrice").toULongLong();
        item["originalPrice"] = static_cast<qint64>(dbOriginalPrice); // Send raw original price (e.g., in cents)
        item["stock"] = query.value("stock").toInt();
        item["sourceStoreName"] = query.value("sourceStoreName").toString();
        float dbDiscount = query.value("discount").toFloat();
        item["discount"] = dbDiscount;
        float dbUniDiscount = query.value("uniDiscount").toFloat();
        item["uniDiscount"] = dbUniDiscount;

        // Calculate effective unit price (assuming originalPrice is in cents)
        double unitPrice = (static_cast<double>(dbOriginalPrice) / 100.0) * dbDiscount * dbUniDiscount;
        item["unitPrice"] = unitPrice;

        cartItems.append(item);
    }

    return true;
}

bool ShoppingCartDAO::removeCartItem(const QString &username, unsigned int cartItemId, QString &errorMessage)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);

    // 验证该购物车项目是否属于指定用户
    query.prepare("SELECT user FROM shoppingcart_table WHERE id = :cartItemId");
    query.bindValue(":cartItemId", cartItemId);

    if (!query.exec())
    {
        errorMessage = "Database query failed: " + query.lastError().text();
        return false;
    }

    if (!query.next())
    {
        errorMessage = "购物车项目不存在";
        return false;
    }

    QString itemOwner = query.value("user").toString();
    if (itemOwner != username)
    {
        errorMessage = "无权限删除此购物车项目";
        return false;
    }

    // 删除购物车项目
    query.prepare("DELETE FROM shoppingcart_table WHERE id = :cartItemId AND user = :username");
    query.bindValue(":cartItemId", cartItemId);
    query.bindValue(":username", username);

    if (!query.exec())
    {
        errorMessage = "Failed to remove cart item: " + query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() == 0)
    {
        errorMessage = "购物车项目不存在或已被删除";
        return false;
    }

    return true;
}

bool ShoppingCartDAO::updateCartItemQuantity(const QString &username, unsigned int cartItemId, int quantity, QString &errorMessage)
{
    if (quantity <= 0)
    {
        errorMessage = "数量必须大于0";
        return false;
    }

    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);

    // 首先验证该购物车项目是否属于指定用户，并获取商品ID
    query.prepare("SELECT user, commodityId FROM shoppingcart_table WHERE id = :cartItemId");
    query.bindValue(":cartItemId", cartItemId);

    if (!query.exec())
    {
        errorMessage = "Database query failed: " + query.lastError().text();
        return false;
    }

    if (!query.next())
    {
        errorMessage = "购物车项目不存在";
        return false;
    }

    QString itemOwner = query.value("user").toString();
    unsigned int commodityId = query.value("commodityId").toUInt();

    if (itemOwner != username)
    {
        errorMessage = "无权限修改此购物车项目";
        return false;
    }

    // 检查商品库存
    query.prepare("SELECT stock FROM commodity_table WHERE id = :commodityId");
    query.bindValue(":commodityId", commodityId);

    if (!query.exec())
    {
        errorMessage = "Failed to check commodity stock: " + query.lastError().text();
        return false;
    }

    if (!query.next())
    {
        errorMessage = "商品不存在";
        return false;
    }

    int stock = query.value("stock").toInt();
    if (quantity > stock)
    {
        errorMessage = QString("库存不足，当前库存为 %1 件").arg(stock);
        return false;
    }

    // 更新购物车项目数量
    query.prepare("UPDATE shoppingcart_table SET quantity = :quantity WHERE id = :cartItemId AND user = :username");
    query.bindValue(":quantity", quantity);
    query.bindValue(":cartItemId", cartItemId);
    query.bindValue(":username", username);

    if (!query.exec())
    {
        errorMessage = "Failed to update cart item quantity: " + query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() == 0)
    {
        errorMessage = "购物车项目不存在或更新失败";
        return false;
    }

    return true;
}

bool ShoppingCartDAO::addCartItem(const QString &username, unsigned int commodityId, int quantity, QString &errorMessage)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);

    // 首先检查商品是否存在且库存充足
    query.prepare("SELECT stock FROM commodity_table WHERE id = :commodityId");
    query.bindValue(":commodityId", commodityId);

    if (!query.exec())
    {
        errorMessage = "Database query failed: " + query.lastError().text();
        return false;
    }

    if (!query.next())
    {
        errorMessage = "商品不存在";
        return false;
    }

    int stock = query.value("stock").toInt();

    // 检查购物车中是否已存在该商品
    query.prepare("SELECT id, quantity FROM shoppingcart_table WHERE user = :username AND commodityId = :commodityId");
    query.bindValue(":username", username);
    query.bindValue(":commodityId", commodityId);

    if (!query.exec())
    {
        errorMessage = "Database query failed: " + query.lastError().text();
        return false;
    }

    if (query.next())
    {
        // 商品已存在，更新数量
        int currentQuantity = query.value("quantity").toInt();
        int newQuantity = currentQuantity + quantity;

        if (newQuantity > stock)
        {
            errorMessage = QString("库存不足，当前库存为 %1 件，购物车中已有 %2 件").arg(stock).arg(currentQuantity);
            return false;
        }

        unsigned int cartItemId = query.value("id").toUInt();
        query.prepare("UPDATE shoppingcart_table SET quantity = :quantity WHERE id = :id");
        query.bindValue(":quantity", newQuantity);
        query.bindValue(":id", cartItemId);

        if (!query.exec())
        {
            errorMessage = "Failed to update cart item: " + query.lastError().text();
            return false;
        }
    }
    else
    {
        // 商品不存在，添加新记录
        if (quantity > stock)
        {
            errorMessage = QString("库存不足，当前库存为 %1 件").arg(stock);
            return false;
        }

        query.prepare("INSERT INTO shoppingcart_table (user, commodityId, quantity) VALUES (:username, :commodityId, :quantity)");
        query.bindValue(":username", username);
        query.bindValue(":commodityId", commodityId);
        query.bindValue(":quantity", quantity);

        if (!query.exec())
        {
            errorMessage = "Failed to add cart item: " + query.lastError().text();
            return false;
        }
    }

    return true;
}

bool ShoppingCartDAO::clearCart(const QString &username, QString &errorMessage)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);

    // 删除该用户的所有购物车项目
    query.prepare("DELETE FROM shoppingcart_table WHERE user = :username");
    query.bindValue(":username", username);

    if (!query.exec())
    {
        errorMessage = "Failed to clear cart: " + query.lastError().text();
        return false;
    }

    qDebug() << "ShoppingCartDAO: Cleared" << query.numRowsAffected() << "items from cart for user:" << username;

    return true;
}
