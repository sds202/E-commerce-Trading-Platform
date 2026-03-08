#ifndef BLL_SHOPPINGCART_H
#define BLL_SHOPPINGCART_H

#include "All.h"
#include "DAL_ShoppingCartDAO.h"

/**
 * @brief 服务器端购物车业务逻辑类
 */
class ShoppingCart : public QObject
{
    Q_OBJECT

public:
    ShoppingCart(const QString &username); // 构造函数，指定用户名

    // 购物车数据操作
    bool fetchCartItems(QJsonArray &cartItems, QString &errorMessage); // 获取购物车商品列表
    void setUsername(const QString &username);                         // 设置购物车所属用户名

    // 购物车商品操作
    bool removeItem(unsigned int cartItemId, QString &errorMessage);                       // 移除购物车商品项
    bool updateItemQuantity(unsigned int cartItemId, int quantity, QString &errorMessage); // 更新商品数量
    bool addItem(unsigned int commodityId, int quantity, QString &errorMessage);           // 添加商品到购物车
    bool clear(QString &errorMessage);                                                     // 清空购物车

private:
    QString username;                // 购物车所属用户名
    ShoppingCartDAO shoppingcartdao; // 购物车数据访问对象
};

#endif // BLL_SHOPPINGCART_H
