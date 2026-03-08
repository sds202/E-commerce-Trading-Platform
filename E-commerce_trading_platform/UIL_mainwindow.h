#ifndef UIL_MAINWINDOW_H
#define UIL_MAINWINDOW_H

#include "All.h"
#include "BLL_user.h"
#include "BLL_commodity.h"
#include "UIL_loginwindow.h"
#include "UIL_commoditydelegate.h"
#include "BLL_shoppingcart.h"
#include "changepassword.h"

extern HandleMessage *m_handleMessage; // 全局消息处理器

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

/**
 * @brief 主窗口类，电商平台的主界面控制器
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr); // 构造函数
    ~MainWindow();                         // 析构函数

private slots:
    // 基础交互槽函数
    void onCommoditySelected(const QModelIndex &index);                                                                          // 主页商品选择
    void onLoginButtonClicked();                                                                                                 // 登录按钮点击
    void onLogoutButtonClicked();                                                                                                // 登出按钮点击
    void onLoginWindowClosed(const QString &username, const QString &password, const QString &type, unsigned long long balance); // 登录窗口关闭
    void onCommoditySelectedPage3(const QModelIndex &index);                                                                     // 商品管理页商品选择
    void onBuyNowClicked();                                                                                                      // 立即购买按钮点击
    void onAddToCartClicked();                                                                                                   // 加入购物车按钮点击
    void updateTotalPrice(int quantity);                                                                                         // 更新总价显示

    // 购物车相关槽函数
    void updateCartDisplay();                         // 更新购物车显示
    void onCartQuantityChanged(int id, int quantity); // 购物车数量改变
    void onRemoveCartItem(int id);                    // 移除购物车项目
    void onClearCartClicked();                        // 清空购物车按钮点击
    void onCreateOrderClicked();                      // 创建订单按钮点击

    // 订单相关槽函数
    void onOrderSelected(const QModelIndex &index); // 订单选择
    void onPayOrderClicked();                       // 支付订单按钮点击
    void onDeleteOrderClicked();                    // 删除订单按钮点击

    // 用户账户相关槽函数
    void onChangePasswordClicked(); // 修改密码按钮点击
    void onChargeButtonClicked();   // 充值按钮点击

    // 商品管理相关槽函数(商家专用)
    void onNewGoodsButtonClicked();    // 新增商品按钮点击
    void onDeleteGoodsButtonClicked(); // 删除商品按钮点击
    void onChangeGoodsButtonClicked(); // 修改商品按钮点击
    void onClearGoodsButtonClicked();  // 清空商品表格按钮点击
    void onUniDiscoutButtonClicked();  // 统一折扣设置按钮点击

    // 网络响应处理槽函数
    void handleFetchAllInfoOfCommodityResponse(bool success, const QString &message, const QVariantMap &commodityData);                              // 主页商品信息获取响应
    void handleFetchAllInfoOfCommodityResponse_page3(bool success, const QString &message, const QVariantMap &commodityData);                        // 商品管理页商品信息获取响应
    void handlePurchaseResponse(bool success, const QString &message, unsigned long long newBuyerBalance, unsigned int newCommodityStock);           // 购买响应
    void handleCommodityUpdateResponse(bool success, const QString &message);                                                                        // 商品更新响应
    void handleCommodityNewResponse(bool success, const QString &message);                                                                           // 新增商品响应
    void handleDeleteCommodityResponse(bool success, const QString &message, unsigned int deletedCommodityId);                                       // 删除商品响应
    void handleSetUniDiscountResponse(bool success, const QString &message, float uniDiscount, const QString &sourceStoreName, const QString &type); // 设置统一折扣响应
    void handleChangePasswordResponse(bool success, const QString &message);                                                                         // 修改密码响应
    void handleRechargeResponse(bool success, const QString &message, unsigned long long newBalance);                                                // 充值响应

    // 购物车操作响应处理
    void onCartUpdated(bool success, const QString &message, const QList<CartItem> &items, const QJsonArray &cartItemsJson);                    // 购物车更新信号处理
    void handleUpdateCartItemQuantityResponse(bool success, const QString &message, const QString &action, const QJsonArray &updatedCartItems); // 更新购物车项目数量响应
    void handleAddToCartResponse(bool success, const QString &message, const QJsonArray &updatedCartItems);                                     // 加入购物车响应
    void handleRemoveCartItemResponse(bool success, const QString &message, const QString &action, const QJsonArray &updatedCartItems);         // 移除购物车项目响应
    void handleClearCartResponse(bool success, const QString &message, const QString &action, const QJsonArray &updatedCartItems);              // 清空购物车响应

    // 订单操作响应处理
    void handleFetchOrdersResponse(bool success, const QString &message, const QJsonArray &orders);                                                                                // 获取订单列表响应
    void handleDeleteOrderResponse(bool success, const QString &message, unsigned int deletedOrderId);                                                                             // 删除订单响应
    void handleValidateOrderItemsResponse(const QString &status, const QString &message, const QJsonArray &validatedItems, unsigned long long totalPrice, const QString &changes); // 订单验证响应
    void handleCreateOrderResponse(bool success, const QString &message, unsigned long long newBalance, unsigned long long totalPrice);                                            // 创建订单响应
    void handlePayOrderResponse(bool success, const QString &message, unsigned int orderId, unsigned long long paidAmount, unsigned long long newBalance);                         // 支付订单响应

private:
    Ui::MainWindow *ui; // UI对象指针

    // 状态标志位
    bool isLogin = false;    // 登录状态标志
    bool isMerchant = false; // 商家身份标志

    // 子窗口
    loginwindow *lwindow; // 登录窗口指针

    // 数据库相关
    QSqlTableModel *model; // 数据库表模型

    // 业务对象
    User *currentUser = nullptr;           // 当前用户对象指针
    Commodity *currentCommodity = nullptr; // 当前选中商品对象指针

    // 主界面布局组件
    QListWidget *menuBar;  // 左侧菜单栏
    QStackedWidget *pages; // 页面堆栈容器

    // 左侧个人信息区域组件
    QLabel *l_profile_welcome;             // 欢迎标签
    QLabel *l_profile_userName;            // 用户名标签
    QLabel *l_profile_balance;             // 余额标签
    QPushButton *l_profile_login;          // 登录按钮
    QPushButton *l_profile_logout;         // 登出按钮
    QPushButton *l_profile_charge;         // 充值按钮
    QPushButton *l_profile_changePassword; // 修改密码按钮

    // 左侧菜单项组件
    QListWidgetItem *item1;                 // 主页菜单项
    QListWidgetItem *item2;                 // 购物车菜单项
    QListWidgetItem *item3;                 // 商品管理菜单项
    QListWidgetItem *item4;                 // 其他功能菜单项
    QListWidgetItem *fakeitem2;             // 购物车占位项(未登录)
    QListWidgetItem *fakeitem3;             // 商品管理占位项(未登录)
    QListWidgetItem *fakeitem3_notMerchant; // 商品管理占位项(非商家)
    QListWidgetItem *fakeitem4;             // 其他功能占位项(未登录)
    QListWidgetItem *item5;                 // 我的订单菜单项
    QListWidgetItem *fakeitem5;             // 我的订单占位项(未登录)

    // 页面1 - 主页商品浏览
    QLineEdit *filterEdit;           // 搜索筛选输入框
    QComboBox *filterCombo;          // 筛选条件下拉框
    QStandardItemModel *itemModel;   // 商品列表数据模型
    QListView *listView;             // 商品列表视图
    QTableWidget *r_commodity_table; // 右侧商品详情表格
    QLabel *unloggedLabel4;          // 未登录提示标签
    QSpinBox *buyQuantitySpinBox;    // 购买数量选择器
    QLabel *totalPriceLabel;         // 总价格标签
    QPushButton *addToCartButton;    // 加入购物车按钮
    QPushButton *buyNowButton;       // 直接购买按钮

    // 页面2 - 购物车
    QTableWidget *cartTable;      // 购物车商品表格
    QPushButton *clearCartButton; // 清空购物车按钮
    QPushButton *createOrder;     // 创建订单按钮
    QLabel *emptyCartLabel;       // 购物车为空提示标签

    // 页面3 - 商品管理(商家专用)
    QLineEdit *filterEdit_page3;           // 商品管理页搜索框
    QComboBox *filterCombo_page3;          // 商品管理页筛选框
    QListView *listView_page3;             // 商品管理页商品列表
    QTableWidget *r_commodity_table_page3; // 商品管理页详情表格
    QLabel *originalPrice;                 // 原价标签
    QLabel *nowPrice;                      // 现价标签
    QPushButton *rsb_b_buy;                // 购买按钮
    QPushButton *rsb_b_change;             // 修改商品按钮
    QPushButton *rsb_b_new;                // 新增商品按钮
    QPushButton *rsb_b_clear;              // 清空表格按钮
    QPushButton *rsb_b_delete;             // 删除商品按钮
    QLabel *tip;                           // 操作提示标签
    QLineEdit *uniDiscoutEdit;             // 统一折扣输入框
    QComboBox *uniDiscoutCombo;            // 统一折扣类型选择框
    QPushButton *uniDiscoutButton;         // 统一折扣设置按钮

    // 页面5 - 我的订单
    QListView *orderListView;           // 订单列表视图
    QTableWidget *r_order_table;        // 订单基本信息表格
    QTableWidget *r_order_items_table;  // 订单商品详情表格
    QLabel *r_order_total_label;        // 订单总价标签
    QPushButton *r_order_pay_button;    // 支付订单按钮
    QPushButton *r_order_delete_button; // 删除订单按钮

    // 私有初始化方法
    void initWidget();                   // 初始化组件
    void initLayout();                   // 初始化布局
    void initMerchantTablePermissions(); // 初始化商家表格权限
    void clearMerchantTableInfo();       // 清空商家表格信息
    void updateOrderDisplay();           // 更新订单显示
    void updateOrderDisplayFromMemory(); // 从内存更新订单显示

    // 商品数据请求相关方法
    void refreshCommodityListViews();                                                                   // 刷新主页商品列表
    void refreshMerchantCommodityListViews();                                                           // 刷新商家商品列表
    void requestCommodities(const QString &storeName, const QString &context);                          // 请求商品数据
    void handleCommoditiesResponse(const QJsonDocument &responseDoc, const QString &context);           // 处理商品数据响应
    void populateModel(QStandardItemModel *model, const QList<QVariantMap> &commodities, bool isPage3); // 填充商品数据模型
};

#endif // UIL_MAINWINDOW_H
