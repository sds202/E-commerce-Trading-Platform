#ifndef BLL_SHOPPINGCART_H
#define BLL_SHOPPINGCART_H

#include "All.h"
#include "NL_HandleMessage.h"

extern HandleMessage *m_handleMessage; // 全局消息处理器

/**
 * @brief 购物车项目结构体
 */
struct CartItem
{
    unsigned int id;          // 购物车项目ID
    unsigned int commodityId; // 商品ID
    int quantity;             // 商品数量
};

/**
 * @brief 购物车类，处理购物车相关的所有业务逻辑
 */
class ShoppingCart : public QObject
{
    Q_OBJECT

public:
    // 构造析构
    ShoppingCart(const QString &userName, QObject *parent); // 构造函数
    ~ShoppingCart();                                        // 析构函数

    // 购物车操作(带错误信息版本)
    bool addItem(unsigned int commodityId, int quantity, QString &errorMessage);   // 添加商品到购物车
    bool removeItem(unsigned int id, QString &errorMessage);                       // 从购物车移除项目(按项目ID)
    void updateItemQuantity(unsigned int id, int quantity, QString &errorMessage); // 更新购物车项目数量
    bool clear(QString &errorMessage);                                             // 清空购物车(带错误信息)

    // 购物车操作(简化版本)
    void removeItem(unsigned int commodityId);              // 移除项目(按商品ID)
    void removeItemByCommodityId(unsigned int commodityId); // 按商品ID删除项目
    void clear();                                           // 清空购物车

    // 数据操作
    QList<CartItem> getItems() const;                                  // 获取所有购物车项目
    bool saveToDatabase() const;                                       // 保存到数据库
    void loadFromDatabase();                                           // 从数据库加载
    void updateFromServerResponse(const QJsonArray &updatedCartItems); // 从服务器响应更新购物车

    // 获取方法
    QString getUserName() const { return userName; } // 获取用户名

private slots:
    void onCartFetched(bool success, const QString &message, const QJsonArray &cartItems); // 购物车数据获取完成槽函数

signals:
    void cartUpdated(bool success, const QString &message, const QList<CartItem> &items, const QJsonArray &cartItemsJson); // 购物车更新信号

private:
    QString userName;      // 购物车所属用户名
    QList<CartItem> items; // 购物车项目列表
};

#endif // BLL_SHOPPINGCART_H
