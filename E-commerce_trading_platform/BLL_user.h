#ifndef BLL_USER_H
#define BLL_USER_H

#include "All.h"
#include "BLL_shoppingcart.h"
#include "BLL_order.h"
#include "NL_HandleMessage.h"

extern QTcpSocket tcpSocket; // 全局TCP套接字

/**
 * @brief 用户类，处理用户相关的所有业务逻辑
 */
class User : public QObject
{
    Q_OBJECT
public:
    // 构造析构
    User();                                                                                              // 默认构造函数
    User(const QString &name, double balance, const QString &type, const QString &password = "");        // 兼容旧版本的构造函数
    User(const QString &name, const QString &password, const QString &type, unsigned long long balance); // 推荐使用的构造函数
    ~User();                                                                                             // 析构函数

    // 需要向服务器发送数据的操作
    void userLogin(const QString &username, const QString &password);                                                                                      // 用户登录
    void userRegister(const QString &username, const QString &password, const QString &type);                                                              // 用户注册
    void purchase(unsigned long long totalPrice, const QString &username, unsigned int id, unsigned int purchaseQuantity, const QString &sourceStoreName); // 立即购买商品
    void changePassword(const QString &newPassword);                                                                                                       // 修改密码
    void recharge(unsigned long long amount);                                                                                                              // 用户充值

    // 用户获取方法
    unsigned long long getBalance() const; // 获取余额(分为单位)
    QString getType() const;               // 获取用户类型
    QString getName() const;               // 获取用户名
    QString getPassword() const;           // 获取密码

    // 用户设置方法
    void setBalance(double balance);           // 设置余额(元为单位)
    void setBalanceWithout100(double balance); // 设置余额(分为单位)

    // 购物车操作
    ShoppingCart *getShoppingCart() const;                            // 获取购物车对象
    void setShoppingCart(ShoppingCart *cart) { shoppingCart = cart; } // 设置购物车对象

    // 订单操作
    void addOrder(Order *order);                     // 添加订单
    void removeOrder(unsigned int orderId);          // 移除订单
    QList<Order *> getOrders() const;                // 获取所有订单
    Order *getOrderById(unsigned int orderId) const; // 根据ID获取订单
    void clearOrders();                              // 清空所有订单

protected:
    QString name;               // 用户名
    QString password;           // 密码
    unsigned long long balance; // 余额(分为单位)
    QString type;               // 用户类型(Customer/Merchant)

    ShoppingCart *shoppingCart = nullptr; // 购物车对象指针
    QList<Order *> orders;                // 订单列表
};

/**
 * @brief 客户类，继承自User
 */
class Customer : public User
{
};

/**
 * @brief 商家类，继承自User
 */
class Merchant : public User
{
};

#endif // BLL_USER_H
