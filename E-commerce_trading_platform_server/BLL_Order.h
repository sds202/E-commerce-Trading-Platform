#ifndef BLL_ORDER_H
#define BLL_ORDER_H

#include "All.h"
#include "DAL_OrderDAO.h"

/**
 * @brief 服务器端订单业务逻辑类
 */
class Order
{
public:
    // 构造析构
    Order();                                                                                                                         // 默认构造函数
    Order(const QString &userName, const QList<QVariantMap> &items, unsigned long long totalPrice, const QString &state = "unPaid"); // 参数构造函数
    ~Order();                                                                                                                        // 析构函数

    // 获取方法
    unsigned int getOrderId() const;          // 获取订单ID
    QString getUserName() const;              // 获取用户名
    QList<QVariantMap> getItems() const;      // 获取订单商品列表
    unsigned long long getTotalPrice() const; // 获取订单总价(分为单位)
    QString getState() const;                 // 获取订单状态
    QString getOrderTime() const;             // 获取订单创建时间

    // 设置方法
    void setState(const QString &state);    // 设置订单状态
    void setOrderId(unsigned int id);       // 设置订单ID
    void setOrderTime(const QString &time); // 设置订单时间

    // 数据库操作（服务端）
    bool saveToDatabase() const;                                    // 保存订单到数据库
    static QList<Order> fetchOrdersByUser(const QString &userName); // 根据用户名获取订单列表
    bool updateStateInDatabase() const;                             // 更新订单状态到数据库

private:
    unsigned int orderId;          // 订单ID，自增INT
    QString userName;              // 关联的用户名
    QList<QVariantMap> items;      // 订单中的商品列表
    unsigned long long totalPrice; // 总价（以分为单位）
    QString state;                 // 订单状态：unPaid, paid, shipped, completed, canceled
    QString orderTime;             // 订单创建时间
    OrderDAO orderdao;             // 订单数据访问对象
};

#endif // BLL_ORDER_H
