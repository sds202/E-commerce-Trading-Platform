#include "DAL_OrderDAO.h"
#include "BLL_Order.h"

OrderDAO::OrderDAO()
{
}

OrderDAO::~OrderDAO()
{
}

bool OrderDAO::saveOrder(const Order &order)
{
    QSqlDatabase db = QSqlDatabase::database();

    // 开始事务
    if (!db.transaction())
    {
        qDebug() << "OrderDAO: Failed to start transaction:" << db.lastError().text();
        return false;
    }

    QSqlQuery query(db);

    // 插入订单基本信息
    query.prepare("INSERT INTO order_table (user, totalPrice, time, state) VALUES (:user, :totalPrice, :time, :state)");
    query.bindValue(":user", order.getUserName());
    query.bindValue(":totalPrice", static_cast<qint64>(order.getTotalPrice()));
    query.bindValue(":time", order.getOrderTime());
    query.bindValue(":state", order.getState());

    if (!query.exec())
    {
        qDebug() << "OrderDAO: Failed to insert order:" << query.lastError().text();
        db.rollback();
        return false;
    }

    // 获取自动生成的订单ID
    unsigned int orderId = query.lastInsertId().toUInt();
    qDebug() << "OrderDAO: Created order with ID:" << orderId;

    // 保存订单项
    if (!saveOrderItems(orderId, order.getItems()))
    {
        qDebug() << "OrderDAO: Failed to save order items";
        db.rollback();
        return false;
    }

    // 提交事务
    if (!db.commit())
    {
        qDebug() << "OrderDAO: Failed to commit transaction:" << db.lastError().text();
        db.rollback();
        return false;
    }

    qDebug() << "OrderDAO: Successfully saved order with ID:" << orderId;
    return true;
}

bool OrderDAO::saveOrderItems(unsigned int orderId, const QList<QVariantMap> &items)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);

    query.prepare("INSERT INTO orderitem_table (id, commodityId, commodityName, quantity, originalPrice, discount, uniDiscount) "
                  "VALUES (:id, :commodityId, :commodityName, :quantity, :originalPrice, :discount, :uniDiscount)");

    for (const QVariantMap &item : items)
    {
        query.bindValue(":id", orderId);
        query.bindValue(":commodityId", item["commodityId"].toUInt());
        query.bindValue(":commodityName", item["commodityName"].toString()); // 新增
        query.bindValue(":quantity", item["quantity"].toUInt());
        query.bindValue(":originalPrice", static_cast<qint64>(item["originalPrice"].toDouble() * 100)); // 转换为分
        query.bindValue(":discount", item["discount"].toDouble());
        query.bindValue(":uniDiscount", item["uniDiscount"].toDouble());

        if (!query.exec())
        {
            qDebug() << "OrderDAO: Failed to insert order item:" << query.lastError().text();
            return false;
        }
    }

    return true;
}

QList<Order> OrderDAO::fetchOrdersByUser(const QString &userName)
{
    QList<Order> orders;
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);

    // 查询用户的所有订单
    query.prepare("SELECT id, totalPrice, time, state FROM order_table WHERE user = :userName ORDER BY time DESC");
    query.bindValue(":userName", userName);

    if (!query.exec())
    {
        qDebug() << "OrderDAO: Failed to fetch orders:" << query.lastError().text();
        return orders;
    }

    while (query.next())
    {
        unsigned int orderId = query.value("id").toUInt();
        unsigned long long totalPrice = query.value("totalPrice").toULongLong();
        QString orderTime = query.value("time").toString();
        QString state = query.value("state").toString();

        // 获取订单项
        QList<QVariantMap> items = fetchOrderItems(orderId);

        // 创建订单对象
        Order order(userName, items, totalPrice, state);
        order.setOrderId(orderId);
        order.setOrderTime(orderTime);

        orders.append(order);
    }

    qDebug() << "OrderDAO: Fetched" << orders.size() << "orders for user:" << userName;
    return orders;
}

QList<QVariantMap> OrderDAO::fetchOrderItems(unsigned int orderId)
{
    QList<QVariantMap> items;
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);

    query.prepare("SELECT commodityId, commodityName, quantity, originalPrice, discount, uniDiscount FROM orderitem_table WHERE id = :orderId");
    query.bindValue(":orderId", orderId);

    if (!query.exec())
    {
        qDebug() << "OrderDAO: Failed to fetch order items:" << query.lastError().text();
        return items;
    }

    while (query.next())
    {
        QVariantMap item;
        item["commodityId"] = query.value("commodityId").toUInt();
        item["commodityName"] = query.value("commodityName").toString(); // 新增
        item["quantity"] = query.value("quantity").toUInt();
        item["originalPrice"] = static_cast<double>(query.value("originalPrice").toULongLong()) / 100.0; // 转换回元
        item["discount"] = query.value("discount").toDouble();
        item["uniDiscount"] = query.value("uniDiscount").toDouble();
        items.append(item);
    }

    return items;
}

bool OrderDAO::updateOrderState(unsigned int orderId, const QString &state)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);

    query.prepare("UPDATE order_table SET state = :state WHERE id = :orderId");
    query.bindValue(":state", state);
    query.bindValue(":orderId", orderId);

    if (!query.exec())
    {
        qDebug() << "OrderDAO: Failed to update order state:" << query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() == 0)
    {
        qDebug() << "OrderDAO: No order found with ID:" << orderId;
        return false;
    }

    qDebug() << "OrderDAO: Updated order" << orderId << "state to:" << state;
    return true;
}

Order OrderDAO::getOrderById(unsigned int orderId)
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);

    query.prepare("SELECT user, totalPrice, time, state FROM order_table WHERE id = :orderId");
    query.bindValue(":orderId", orderId);

    if (!query.exec() || !query.next())
    {
        qDebug() << "OrderDAO: Failed to get order by ID:" << orderId;
        return Order(); // 返回空订单
    }

    QString userName = query.value("user").toString();
    unsigned long long totalPrice = query.value("totalPrice").toULongLong();
    QString orderTime = query.value("time").toString();
    QString state = query.value("state").toString();

    // 获取订单项
    QList<QVariantMap> items = fetchOrderItems(orderId);

    // 创建订单对象
    Order order(userName, items, totalPrice, state);
    order.setOrderId(orderId);
    order.setOrderTime(orderTime);

    return order;
}

bool OrderDAO::deleteOrder(unsigned int orderId)
{
    QSqlDatabase db = QSqlDatabase::database();

    // 开始事务
    if (!db.transaction())
    {
        qDebug() << "OrderDAO: Failed to start transaction for delete:" << db.lastError().text();
        return false;
    }

    QSqlQuery query(db);

    // 由于外键约束，删除订单时会自动删除相关的订单项
    query.prepare("DELETE FROM order_table WHERE id = :orderId");
    query.bindValue(":orderId", orderId);

    if (!query.exec())
    {
        qDebug() << "OrderDAO: Failed to delete order:" << query.lastError().text();
        db.rollback();
        return false;
    }

    if (query.numRowsAffected() == 0)
    {
        qDebug() << "OrderDAO: No order found with ID:" << orderId;
        db.rollback();
        return false;
    }

    // 提交事务
    if (!db.commit())
    {
        qDebug() << "OrderDAO: Failed to commit delete transaction:" << db.lastError().text();
        db.rollback();
        return false;
    }

    qDebug() << "OrderDAO: Successfully deleted order with ID:" << orderId;
    return true;
}

bool OrderDAO::deleteOrderAndRestoreStock(unsigned int orderId, const QString &userName)
{
    QSqlDatabase db = QSqlDatabase::database();

    // 开始事务
    if (!db.transaction())
    {
        qDebug() << "OrderDAO: Failed to start transaction for delete order:" << db.lastError().text();
        return false;
    }

    QSqlQuery query(db);

    // 1. 验证订单是否属于指定用户并获取订单状态
    query.prepare("SELECT user, state FROM order_table WHERE id = :orderId");
    query.bindValue(":orderId", orderId);

    if (!query.exec())
    {
        qDebug() << "OrderDAO: Failed to verify order owner and state:" << query.lastError().text();
        db.rollback();
        return false;
    }

    if (!query.next())
    {
        qDebug() << "OrderDAO: Order not found:" << orderId;
        db.rollback();
        return false;
    }

    QString orderOwner = query.value("user").toString();
    QString orderState = query.value("state").toString();

    if (orderOwner != userName)
    {
        qDebug() << "OrderDAO: User" << userName << "is not the owner of order" << orderId;
        db.rollback();
        return false;
    }

    // 2. 根据订单状态决定是否需要恢复库存
    bool shouldRestoreStock = (orderState == "unPaid");
    qDebug() << "OrderDAO: Order" << orderId << "state is" << orderState << ", should restore stock:" << shouldRestoreStock;

    if (shouldRestoreStock)
    {
        // 3. 获取订单项以便恢复库存
        query.prepare("SELECT commodityId, quantity FROM orderitem_table WHERE id = :orderId");
        query.bindValue(":orderId", orderId);

        if (!query.exec())
        {
            qDebug() << "OrderDAO: Failed to fetch order items for stock restoration:" << query.lastError().text();
            db.rollback();
            return false;
        }

        QList<QPair<unsigned int, unsigned int>> itemsToRestore; // commodityId, quantity
        while (query.next())
        {
            unsigned int commodityId = query.value("commodityId").toUInt();
            unsigned int quantity = query.value("quantity").toUInt();
            itemsToRestore.append(qMakePair(commodityId, quantity));
        }

        // 4. 恢复商品库存（只有商品仍然存在时才恢复）
        for (const auto &item : itemsToRestore)
        {
            unsigned int commodityId = item.first;
            unsigned int quantity = item.second;

            // 检查商品是否仍然存在
            query.prepare("SELECT stock FROM commodity_table WHERE id = :commodityId");
            query.bindValue(":commodityId", commodityId);

            if (query.exec() && query.next())
            {
                // 商品存在，恢复库存
                unsigned int currentStock = query.value("stock").toUInt();
                unsigned int newStock = currentStock + quantity;

                query.prepare("UPDATE commodity_table SET stock = :newStock WHERE id = :commodityId");
                query.bindValue(":newStock", newStock);
                query.bindValue(":commodityId", commodityId);

                if (!query.exec())
                {
                    qDebug() << "OrderDAO: Failed to restore stock for commodity" << commodityId << ":" << query.lastError().text();
                    db.rollback();
                    return false;
                }

                qDebug() << "OrderDAO: Restored stock for commodity" << commodityId << ": +" << quantity << "(now" << newStock << ")";
            }
            else
            {
                // 商品不存在，跳过库存恢复
                qDebug() << "OrderDAO: Commodity" << commodityId << "no longer exists, skipping stock restoration";
            }
        }

        qDebug() << "OrderDAO: Stock restoration completed for" << itemsToRestore.size() << "items";
    }
    else
    {
        qDebug() << "OrderDAO: Order is paid/shipped/completed, skipping stock restoration";
    }

    // 5. 删除订单（外键约束会自动删除订单项）
    query.prepare("DELETE FROM order_table WHERE id = :orderId");
    query.bindValue(":orderId", orderId);

    if (!query.exec())
    {
        qDebug() << "OrderDAO: Failed to delete order:" << query.lastError().text();
        db.rollback();
        return false;
    }

    if (query.numRowsAffected() == 0)
    {
        qDebug() << "OrderDAO: No order found with ID:" << orderId;
        db.rollback();
        return false;
    }

    // 提交事务
    if (!db.commit())
    {
        qDebug() << "OrderDAO: Failed to commit delete order transaction:" << db.lastError().text();
        db.rollback();
        return false;
    }

    QString resultMessage = shouldRestoreStock ? QString("Successfully deleted order %1 and restored stock").arg(orderId) : QString("Successfully deleted order %1 (no stock restoration needed for paid order)").arg(orderId);

    qDebug() << "OrderDAO:" << resultMessage;
    return true;
}
