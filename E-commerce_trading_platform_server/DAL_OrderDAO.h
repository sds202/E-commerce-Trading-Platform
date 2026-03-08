#ifndef DAL_ORDERDAO_H
#define DAL_ORDERDAO_H

#include "All.h"

class Order; // 前向声明

/**
 * @brief 订单数据访问对象，负责订单数据的持久化操作
 */
class OrderDAO
{
public:
    OrderDAO();  // 构造函数
    ~OrderDAO(); // 析构函数

    // 订单基本操作
    bool saveOrder(const Order &order);                                             // 保存订单到数据库
    QList<Order> fetchOrdersByUser(const QString &userName);                        // 根据用户名获取订单列表
    Order getOrderById(unsigned int orderId);                                       // 根据订单ID获取订单
    bool deleteOrder(unsigned int orderId);                                         // 删除订单
    bool deleteOrderAndRestoreStock(unsigned int orderId, const QString &userName); // 删除订单并恢复库存

    // 订单状态管理
    bool updateOrderState(unsigned int orderId, const QString &state); // 更新订单状态

private:
    // 订单项管理
    bool saveOrderItems(unsigned int orderId, const QList<QVariantMap> &items); // 保存订单项到数据库
    QList<QVariantMap> fetchOrderItems(unsigned int orderId);                   // 获取订单项
};

#endif // DAL_ORDERDAO_H
