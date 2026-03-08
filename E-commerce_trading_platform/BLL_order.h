#ifndef BLL_ORDER_H
#define BLL_ORDER_H

#include "All.h"
#include "NL_HandleMessage.h"

/**
 * @brief 订单类，处理订单相关的所有业务逻辑
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
    QString getState() const;                 // 获取订单状态(unPaid/paid/cancelled)
    QString getOrderTime() const;             // 获取订单创建时间

    // 设置方法
    void setState(const QString &state);    // 设置订单状态
    void setOrderId(unsigned int id);       // 设置订单ID
    void setOrderTime(const QString &time); // 设置订单时间

    // 网络操作
    static void fetchOrdersFromServer(const QString &userName);                                                                      // 从服务器获取用户订单列表
    static void deleteOrderFromServer(const QString &userName, unsigned int orderId);                                                // 从服务器删除订单
    static void createOrderFromServer(const QString &userName, const QList<QVariantMap> &orderItems, unsigned long long totalPrice); // 在服务器创建订单
    static void payOrderFromServer(const QString &userName, unsigned int orderId);                                                   // 向服务器发送支付请求

private:
    unsigned int orderId;          // 订单唯一标识符
    QString userName;              // 订单所属用户名
    QList<QVariantMap> items;      // 订单商品列表(包含商品ID、数量、价格等信息)
    unsigned long long totalPrice; // 订单总价(分为单位)
    QString state;                 // 订单状态(unPaid未支付/paid已支付/cancelled已取消)
    QString orderTime;             // 订单创建时间
};

#endif // BLL_ORDER_H
