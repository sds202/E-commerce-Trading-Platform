#ifndef NL_HANDLEMESSAGE_H
#define NL_HANDLEMESSAGE_H

#include "All.h"

extern QTcpSocket tcpSocket; // 全局TCP套接字

/**
 * @brief 网络消息处理类，负责客户端与服务器的通信
 */
class HandleMessage : public QObject
{
    Q_OBJECT
public:
    explicit HandleMessage(QObject *parent = nullptr); // 构造函数

    // 静态方法：发送带长度前缀的JSON数据
    static bool sendJsonData(QTcpSocket *socket, const QJsonObject &jsonObject); // 发送JSON数据到服务器

signals:
    // 基础通信信号
    void serverResponse(const QJsonObject &response); // 通用服务器响应信号

    // 用户认证相关信号
    void registerResponse(bool success, QString message);                                                                                                        // 注册响应信号
    void loginResponse(bool success, const QString &message, const QString &username, const QString &password, const QString &type, unsigned long long balance); // 登录响应信号
    void changePasswordResponse(bool success, const QString &message);                                                                                           // 修改密码响应信号
    void rechargeResponse(bool success, const QString &message, unsigned long long newBalance = 0);                                                              // 充值响应信号

    // 商品相关信号
    void commoditiesResponseReceived(const QJsonDocument &responseDoc, const QString &context);                                                // 商品列表响应信号
    void fetchAllInfoOfCommodityResponse(bool success, const QString &message, const QVariantMap &commodityData);                              // 主页商品详情响应信号
    void fetchAllInfoOfCommodityResponse_page3(bool success, const QString &message, const QVariantMap &commodityData);                        // 商品管理页详情响应信号
    void commodityUpdateResponse(bool success, const QString &message);                                                                        // 商品更新响应信号
    void commodityNewResponse(bool success, const QString &message);                                                                           // 新增商品响应信号
    void deleteCommodityResponse(bool success, const QString &message, unsigned int deletedCommodityId);                                       // 删除商品响应信号
    void setUniDiscountResponse(bool success, const QString &message, float uniDiscount, const QString &sourceStoreName, const QString &type); // 设置统一折扣响应信号

    // 购买相关信号
    void purchaseResponse(bool success, const QString &message, unsigned long long newBuyerBalance = 0, unsigned int newCommodityStock = 0); // 购买响应信号

    // 购物车相关信号
    void cartFetched(bool success, const QString &message, const QJsonArray &cartItems);                                                  // 获取购物车响应信号
    void updateCartItemQuantityResponse(bool success, const QString &message, const QString &action, const QJsonArray &updatedCartItems); // 更新购物车数量响应信号
    void addToCartResponse(bool success, const QString &message, const QJsonArray &updatedCartItems);                                     // 加入购物车响应信号
    void removeCartItemResponse(bool success, const QString &message, const QString &action, const QJsonArray &updatedCartItems);         // 移除购物车项目响应信号
    void clearCartResponse(bool success, const QString &message, const QString &action, const QJsonArray &updatedCartItems);              // 清空购物车响应信号

    // 订单相关信号
    void fetchOrdersResponse(bool success, const QString &message, const QJsonArray &orders);                                                                                // 获取订单列表响应信号
    void deleteOrderResponse(bool success, const QString &message, unsigned int deletedOrderId);                                                                             // 删除订单响应信号
    void validateOrderItemsResponse(const QString &status, const QString &message, const QJsonArray &validatedItems, unsigned long long totalPrice, const QString &changes); // 订单验证响应信号
    void createOrderResponse(bool success, const QString &message, unsigned long long newBalance, unsigned long long totalPrice);                                            // 创建订单响应信号
    void payOrderResponse(bool success, const QString &message, unsigned int orderId, unsigned long long paidAmount, unsigned long long newBalance);                         // 支付订单响应信号

private slots:
    void onReadyRead(); // 处理套接字可读数据槽函数

private:
    QByteArray m_buffer;         // 接收缓冲区，用于存储不完整的消息
    quint32 m_nextBlockSize = 0; // 下一个消息的预期长度，用于消息分包处理
};

#endif // NL_HANDLEMESSAGE_H
