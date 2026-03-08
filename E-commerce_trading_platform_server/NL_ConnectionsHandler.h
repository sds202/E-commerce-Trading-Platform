#ifndef NL_CONNECTIONSHANDLER_H
#define NL_CONNECTIONSHANDLER_H

#include "All.h"
#include "NL_MessageSent.h"
#include "BLL_user.h"
#include "DAL_CommodityDAO.h"
#include "BLL_commodity.h"
#include "BLL_ShoppingCart.h"
#include "BLL_order.h"
#include "DAL_OrderDAO.h"

/**
 * @brief 客户端连接处理器类，负责处理单个客户端的所有业务请求
 */
class ConnectionsHandler : public QObject
{
    Q_OBJECT
public:
    // 构造析构
    explicit ConnectionsHandler(QTcpSocket *socket, QObject *parent = nullptr); // 构造函数，传入已连接的套接字
    ~ConnectionsHandler();                                                      // 析构函数

signals:
    void clientDisconnected(ConnectionsHandler *handler); // 客户端断开连接信号

private slots:
    // 网络事件处理
    void onReadyRead();                                             // 处理套接字可读数据
    void onDisconnected();                                          // 处理套接字断开连接
    void onErrorOccurred(QAbstractSocket::SocketError socketError); // 处理套接字错误

private:
    // 网络通信相关
    QTcpSocket *m_socket;    // 该处理器管理的套接字
    QByteArray m_buffer;     // 接收缓冲区，用于处理分包数据
    quint32 m_nextBlockSize; // 下一个消息的预期长度

    // 业务对象
    Commodity m_commodity; // 商品业务对象
    User m_user;           // 用户业务对象
    ShoppingCart cart;     // 购物车业务对象
    Order order;           // 订单类业务对象

    // 核心业务处理
    QJsonObject processRequest(const QJsonObject &request); // 处理客户端请求的主入口

    // 用户认证相关处理函数
    QJsonObject handleRegisterRequest(const QJsonObject &request);       // 处理用户注册请求
    QJsonObject handleLoginRequest(const QJsonObject &request);          // 处理用户登录请求
    QJsonObject handleChangePasswordRequest(const QJsonObject &request); // 处理修改密码请求
    QJsonObject handleRechargeRequest(const QJsonObject &request);       // 处理用户充值请求

    // 商品相关处理函数
    QJsonObject handleQueryProductRequest(const QJsonObject &request);            // 处理商品查询请求
    QJsonObject handleFetchCommoditiesRequest(const QJsonObject &request);        // 处理获取商品列表请求
    QJsonObject handleFetchAllInfoOfCommodityRequest(const QJsonObject &request); // 处理获取商品详细信息请求
    QJsonObject handleUpdateCommodityRequest(const QJsonObject &request);         // 处理更新商品信息请求
    QJsonObject handleNewCommodityRequest(const QJsonObject &request);            // 处理新增商品请求
    QJsonObject handleDeleteCommodityRequest(const QJsonObject &request);         // 处理删除商品请求
    QJsonObject handleSetUniDiscountRequest(const QJsonObject &request);          // 处理设置统一折扣请求
    QJsonObject handleGetUserUniDiscountRequest(const QJsonObject &request);      // 处理获取用户统一折扣请求

    // 购买相关处理函数
    QJsonObject handlePurchaseRequest(const QJsonObject &request); // 处理直接购买请求

    // 购物车相关处理函数
    QJsonObject handleFetchCartRequest(const QJsonObject &request);              // 处理获取购物车请求
    QJsonObject handleAddToCartRequest(const QJsonObject &request);              // 处理添加商品到购物车请求
    QJsonObject handleUpdateCartItemQuantityRequest(const QJsonObject &request); // 处理更新购物车商品数量请求
    QJsonObject handleRemoveCartItemRequest(const QJsonObject &request);         // 处理移除购物车商品请求
    QJsonObject handleClearCartRequest(const QJsonObject &request);              // 处理清空购物车请求

    // 订单相关处理函数
    QJsonObject handleFetchOrdersRequest(const QJsonObject &request);        // 处理获取订单列表请求
    QJsonObject handleValidateOrderItemsRequest(const QJsonObject &request); // 处理订单项目验证请求
    QJsonObject handleCreateOrderRequest(const QJsonObject &request);        // 处理创建订单请求
    QJsonObject handlePayOrderRequest(const QJsonObject &request);           // 处理支付订单请求
    QJsonObject handleDeleteOrderRequest(const QJsonObject &request);        // 处理删除订单请求
};

#endif // NL_CONNECTIONSHANDLER_H
