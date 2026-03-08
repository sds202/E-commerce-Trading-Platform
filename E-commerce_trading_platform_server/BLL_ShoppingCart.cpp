#include "BLL_ShoppingCart.h"
#include "DAL_ShoppingCartDAO.h"

ShoppingCart::ShoppingCart(const QString &username) : username(username) {}

bool ShoppingCart::fetchCartItems(QJsonArray &cartItems, QString &errorMessage)
{
    ShoppingCartDAO cartDAO;
    return cartDAO.fetchCartItems(username, cartItems, errorMessage);
}

void ShoppingCart::setUsername(const QString &username)
{
    this->username = username;
}

bool ShoppingCart::removeItem(unsigned int cartItemId, QString &errorMessage)
{
    ShoppingCartDAO cartDAO;
    return cartDAO.removeCartItem(username, cartItemId, errorMessage);
}

bool ShoppingCart::updateItemQuantity(unsigned int cartItemId, int quantity, QString &errorMessage)
{
    if (quantity < 0)
    {
        errorMessage = "数量不能为负数";
        return false;
    }

    ShoppingCartDAO cartDAO;
    return cartDAO.updateCartItemQuantity(username, cartItemId, quantity, errorMessage);
}

bool ShoppingCart::addItem(unsigned int commodityId, int quantity, QString &errorMessage)
{
    if (quantity <= 0)
    {
        errorMessage = "数量必须大于0";
        return false;
    }

    ShoppingCartDAO cartDAO;
    return cartDAO.addCartItem(username, commodityId, quantity, errorMessage);
}

bool ShoppingCart::clear(QString &errorMessage)
{
    ShoppingCartDAO cartDAO;
    return cartDAO.clearCart(username, errorMessage);
}
