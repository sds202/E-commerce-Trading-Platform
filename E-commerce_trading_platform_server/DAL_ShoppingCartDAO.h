#ifndef DAL_SHOPPINGCARTDAO_H
#define DAL_SHOPPINGCARTDAO_H

#include "All.h"

/**
 * @brief 购物车数据访问对象，负责购物车数据的持久化操作
 */
class ShoppingCartDAO
{
public:
    // 购物车数据操作
    bool fetchCartItems(const QString &username, QJsonArray &cartItems, QString &errorMessage);                         // 获取用户购物车商品列表
    bool addCartItem(const QString &username, unsigned int commodityId, int quantity, QString &errorMessage);           // 添加商品到购物车
    bool removeCartItem(const QString &username, unsigned int cartItemId, QString &errorMessage);                       // 移除购物车商品项
    bool updateCartItemQuantity(const QString &username, unsigned int cartItemId, int quantity, QString &errorMessage); // 更新购物车商品数量
    bool clearCart(const QString &username, QString &errorMessage);                                                     // 清空用户购物车
};

#endif // DAL_SHOPPINGCARTDAO_H
