#ifndef BLL_USER_H
#define BLL_USER_H

#include "All.h"
#include "DAL_UserDAO.h"
#include "DAL_CommodityDAO.h"

/**
 * @brief 服务器端用户业务逻辑类
 */
class User
{
public:
    User(); // 默认构造函数

    // 用户数据库操作
    static User *createFromDatabaseByName(const QString &name);                                                                                                                     // 从数据库根据用户名创建用户对象
    bool saveToDatabase() const;                                                                                                                                                    // 保存用户信息到数据库
    static bool transferToMerchant(const QString &merchantName, unsigned long long amount, QString &errorMessage);                                                                  // 向商家转账
    QVariantMap makePurchase(const QString &buyerUsername, unsigned int commodityId, unsigned int purchaseQuantity, const QString &sellerStoreName, unsigned long long totalPrice); // 执行购买操作
    QVariantMap rechargeBalance(const QString &username, unsigned long long amount);                                                                                                // 用户充值操作

    // 用户获取方法
    unsigned long long getBalance() const; // 获取用户余额
    QString getType() const;               // 获取用户类型
    QString getName() const;               // 获取用户名
    QString getPassword() const;           // 获取用户密码

    // 用户设置方法
    void setBalance(double balance);                                                 // 设置用户余额
    QVariantMap changePassword(const QString &username, const QString &newPassword); // 修改用户密码

    // 登录注册
    LoginErrorType userLogin(const QString &username, const QString &password);                            // 用户登录验证
    RegisterErrorType userRegister(const QString &username, const QString &password, const QString &type); // 用户注册

    // 购买
    bool purchase(unsigned long long totalPrice, QString &errorMessage); // 购买商品(扣除余额)

    // 购物车操作
    // ShoppingCart* getShoppingCart() const; // 获取购物车(已注释)

    // 订单操作
    // QList<Order*> getOrders() const; // 获取订单列表(已注释)
    // void addOrder(Order* order); // 添加订单(已注释)
    void removeOrder(unsigned int orderId); // 移除订单

protected:
    QString name;                // 用户名
    QString password;            // 用户密码
    unsigned long long balance;  // 用户余额(分为单位)
    QString type;                // 用户类型(Customer/Merchant)
    UserDAO m_userDAO;           // 用户数据访问对象
    CommodityDAO m_commodityDAO; // 商品数据访问对象
};

/**
 * @brief 客户类，继承自User
 */
class Customer : public User
{
    // 目前无特殊功能，保留用于扩展
};

/**
 * @brief 商家类，继承自User
 */
class Merchant : public User
{
    // 目前无特殊功能，保留用于扩展
};

#endif // BLL_USER_H
