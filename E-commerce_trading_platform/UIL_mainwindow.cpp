#include "UIL_mainwindow.h"
#include "ui_UIL_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("电商交易平台");
    setMinimumWidth(1000);

    initWidget();
    initLayout();

    lwindow = new loginwindow(parent);

    connect(menuBar, &QListWidget::currentRowChanged, this, [this](int index)
            {
                pages->setCurrentIndex(index);
                if (index == 0)
                { // 切换到主页
                    refreshCommodityListViews();
                }
                if (index == 1)
                { // 切换到购物车页面
                    updateCartDisplay();
                }
                if (index == 3)
                { // 切换到订单页面
                    updateOrderDisplay();
                }
                if (index == 5)
                { // 切换到商品管理页面
                    refreshMerchantCommodityListViews();
                    clearMerchantTableInfo(); // 清理表格信息
                    qDebug() << "change to page index 2";
                } });
    connect(menuBar, &QListWidget::currentRowChanged, pages, &QStackedWidget::setCurrentIndex);
    connect(listView, &QListView::clicked, this, &MainWindow::onCommoditySelected);
    connect(l_profile_login, &QPushButton::clicked, this, &MainWindow::onLoginButtonClicked);
    connect(l_profile_logout, &QPushButton::clicked, this, &MainWindow::onLogoutButtonClicked);
    connect(listView_page3, &QListView::clicked, this, &MainWindow::onCommoditySelectedPage3);
    connect(buyQuantitySpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::updateTotalPrice);
    connect(addToCartButton, &QPushButton::clicked, this, &MainWindow::onAddToCartClicked);
    connect(buyNowButton, &QPushButton::clicked, this, &MainWindow::onBuyNowClicked);
    connect(clearCartButton, &QPushButton::clicked, this, &MainWindow::onClearCartClicked);
    connect(createOrder, &QPushButton::clicked, this, &MainWindow::onCreateOrderClicked);
    connect(orderListView, &QListView::clicked, this, &MainWindow::onOrderSelected);
    connect(r_order_pay_button, &QPushButton::clicked, this, &MainWindow::onPayOrderClicked);
    connect(l_profile_changePassword, &QPushButton::clicked, this, &MainWindow::onChangePasswordClicked);
    connect(l_profile_charge, &QPushButton::clicked, this, &MainWindow::onChargeButtonClicked);
    // 连接按钮信号
    connect(rsb_b_new, &QPushButton::clicked, this, &MainWindow::onNewGoodsButtonClicked);
    connect(rsb_b_change, &QPushButton::clicked, this, &MainWindow::onChangeGoodsButtonClicked);
    connect(rsb_b_clear, &QPushButton::clicked, this, &MainWindow::onClearGoodsButtonClicked);
    connect(rsb_b_delete, &QPushButton::clicked, this, &MainWindow::onDeleteGoodsButtonClicked); // 新增连接
    // connect(uniDiscoutButton, &QPushButton::clicked, this, &MainWindow::onUniDiscoutButtonClicked);

    connect(r_order_delete_button, &QPushButton::clicked, this, &MainWindow::onDeleteOrderClicked);
    connect(uniDiscoutButton, &QPushButton::clicked, this, &MainWindow::onUniDiscoutButtonClicked);

    connect(lwindow, &loginwindow::loginwindowclosed, this, &MainWindow::onLoginWindowClosed);
    connect(m_handleMessage, &HandleMessage::commoditiesResponseReceived,
            this, &MainWindow::handleCommoditiesResponse);
    connect(m_handleMessage, &HandleMessage::fetchAllInfoOfCommodityResponse,
            this, &MainWindow::handleFetchAllInfoOfCommodityResponse);
    connect(m_handleMessage, &HandleMessage::fetchAllInfoOfCommodityResponse_page3,
            this, &MainWindow::handleFetchAllInfoOfCommodityResponse_page3);
    connect(m_handleMessage, &HandleMessage::purchaseResponse,
            this, &MainWindow::handlePurchaseResponse);
    connect(m_handleMessage, &HandleMessage::commodityUpdateResponse,
            this, &MainWindow::handleCommodityUpdateResponse);
    connect(m_handleMessage, &HandleMessage::commodityNewResponse,
            this, &MainWindow::handleCommodityNewResponse);
    connect(m_handleMessage, &HandleMessage::setUniDiscountResponse,
            this, &MainWindow::handleSetUniDiscountResponse);
    connect(m_handleMessage, &HandleMessage::changePasswordResponse,
            this, &MainWindow::handleChangePasswordResponse);
    connect(m_handleMessage, &HandleMessage::rechargeResponse,
            this, &MainWindow::handleRechargeResponse);
    connect(m_handleMessage, &HandleMessage::updateCartItemQuantityResponse,
            this, &MainWindow::handleUpdateCartItemQuantityResponse);
    connect(m_handleMessage, &HandleMessage::addToCartResponse,
            this, &MainWindow::handleAddToCartResponse);
    connect(m_handleMessage, &HandleMessage::removeCartItemResponse,
            this, &MainWindow::handleRemoveCartItemResponse); // 新增连接
    connect(m_handleMessage, &HandleMessage::clearCartResponse,
            this, &MainWindow::handleClearCartResponse); // 新增连接
    connect(m_handleMessage, &HandleMessage::fetchOrdersResponse,
            this, &MainWindow::handleFetchOrdersResponse); // 新增连接
    connect(m_handleMessage, &HandleMessage::deleteOrderResponse,
            this, &MainWindow::handleDeleteOrderResponse); // 新增连接
    connect(m_handleMessage, &HandleMessage::validateOrderItemsResponse,
            this, &MainWindow::handleValidateOrderItemsResponse); // 新增连接
    connect(m_handleMessage, &HandleMessage::createOrderResponse,
            this, &MainWindow::handleCreateOrderResponse); // 新增连接
    connect(m_handleMessage, &HandleMessage::payOrderResponse,
            this, &MainWindow::handlePayOrderResponse); // 新增连接
    connect(m_handleMessage, &HandleMessage::deleteCommodityResponse,
            this, &MainWindow::handleDeleteCommodityResponse); // 新增连接

    menuBar->setCurrentRow(0); // 默认选中主页
}

MainWindow::~MainWindow()
{
    delete ui;
    delete currentUser;
    delete currentCommodity;
    delete lwindow;
}

void MainWindow::initWidget()
{
    // 1.左侧菜单栏
    menuBar = new QListWidget(this);
    menuBar->setFixedWidth(170);          // 工具栏宽度
    menuBar->setIconSize(QSize(15, 15));  // 图标大小
    menuBar->setSpacing(2);               // 设置间距
    menuBar->setFocusPolicy(Qt::NoFocus); // 禁用焦点

    item1 = new QListWidgetItem(QIcon("://style/icon/home.png"), "  主页");
    item2 = new QListWidgetItem(QIcon("://style/icon/shopping_cart.png"), "  购物车");
    item5 = new QListWidgetItem(QIcon("://style/icon/order.png"), "  我的订单");
    fakeitem5 = new QListWidgetItem(QIcon("://style/icon/order.png"), "  我的订单");
    item3 = new QListWidgetItem(QIcon("://style/icon/commodities_management.png"), "  商品管理");
    // item4 = new QListWidgetItem(QIcon("://style/icon/setting.png"), "  设置");
    fakeitem2 = new QListWidgetItem(QIcon("://style/icon/shopping_cart.png"), "  购物车");
    fakeitem3 = new QListWidgetItem(QIcon("://style/icon/commodities_management.png"), "  商品管理");
    fakeitem3_notMerchant = new QListWidgetItem(QIcon("://style/icon/commodities_management.png"), "  商品管理");
    // fakeitem4 = new QListWidgetItem(QIcon("://style/icon/setting.png"), "  设置");

    menuBar->addItem(item1);
    menuBar->addItem(item2);
    menuBar->addItem(fakeitem2);
    menuBar->addItem(item5);
    menuBar->addItem(fakeitem5);
    menuBar->addItem(item3);
    menuBar->addItem(fakeitem3);
    menuBar->addItem(fakeitem3_notMerchant);
    // menuBar->addItem(item4);
    // menuBar->addItem(fakeitem4);

    item2->setHidden(true); // 隐藏购物车
    item3->setHidden(true); // 隐藏商品管理
    // item4->setHidden(true); // 隐藏设置
    item5->setHidden(true);
    fakeitem3_notMerchant->setHidden(true);

    // 2.左侧_个人信息
    l_profile_welcome = new QLabel("你好，", this);
    l_profile_userName = new QLabel("游客", this);
    l_profile_welcome->setStyleSheet("font-size: 16pt;");
    l_profile_userName->setStyleSheet("font-size: 16pt;");
    l_profile_balance = new QLabel("登录以查看余额", this);
    l_profile_login = new QPushButton("登录/注册", this);
    l_profile_logout = new QPushButton("注销", this);
    l_profile_changePassword = new QPushButton("修改密码", this);
    l_profile_charge = new QPushButton("充值", this);

    // 页面1-主页=======================================================================================

    // 3.中间筛选控件
    filterEdit = new QLineEdit(this);
    filterEdit->setPlaceholderText("输入筛选关键词");
    filterCombo = new QComboBox(this);
    filterCombo->addItems({"名称 ", "种类 ", "商家 "});

    // 4.中间商品列表
    listView = new QListView(this);
    listView->setViewMode(QListView::IconMode);
    listView->setGridSize(QSize(150, 200));
    listView->setResizeMode(QListView::Adjust);
    listView->setMovement(QListView::Static);
    listView->setSpacing(10);
    listView->setStyleSheet("QListView {"
                            "background-color: transparent;border: none; } ");

    // 加载商品数据
    //  初始化：
    //  先创建并设置模型，但不要填充数据
    QStandardItemModel *model = new QStandardItemModel(this);
    // 注意：这里先将 QStandardItemModel 设置给 listView
    // 后面再设置 QSortFilterProxyModel 会覆盖掉
    listView->setModel(model);
    listView->setItemDelegate(new CommodityDelegate(this));

    // 设置代理模型
    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->setFilterRole(Qt::DisplayRole); // 默认筛选名称
    listView->setModel(proxyModel);             // 现在 listView 看到的是 proxyModel

    // 连接筛选控件信号 (这部分保持不变)
    connect(filterEdit, &QLineEdit::textChanged, this, [proxyModel](const QString &text)
            { proxyModel->setFilterRegularExpression(QRegularExpression(text, QRegularExpression::CaseInsensitiveOption)); });
    connect(filterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [proxyModel, model](int index)
            {
        switch (index) {
        case 0: // 名称
            proxyModel->setFilterRole(Qt::DisplayRole);
            proxyModel->setFilterKeyColumn(0);
            break;
        case 1: // 种类
            proxyModel->setFilterRole(Qt::UserRole + 1);
            proxyModel->setFilterKeyColumn(0);
            break;
        case 2: // 商家
            proxyModel->setFilterRole(Qt::UserRole + 5);
            proxyModel->setFilterKeyColumn(0);
            break;
        } });

    // 5.右侧商品详细信息
    r_commodity_table = new QTableWidget;
    r_commodity_table->setColumnCount(2); // 两列：标题和数据
    r_commodity_table->setRowCount(8);    // 八行：商品名称、种类、来源商家、库存、原价、折扣、统一折扣、描述
    r_commodity_table->setHorizontalHeaderLabels({"字段", "值"});
    r_commodity_table->setItem(0, 0, new QTableWidgetItem(" 商品名称"));
    r_commodity_table->setItem(1, 0, new QTableWidgetItem(" 种类"));
    r_commodity_table->setItem(2, 0, new QTableWidgetItem(" 来源商家"));
    r_commodity_table->setItem(3, 0, new QTableWidgetItem(" 库存"));
    r_commodity_table->setItem(4, 0, new QTableWidgetItem(" 原价"));
    r_commodity_table->setItem(5, 0, new QTableWidgetItem(" 折扣"));
    r_commodity_table->setItem(6, 0, new QTableWidgetItem(" 统一折扣"));
    r_commodity_table->setItem(7, 0, new QTableWidgetItem(" 描述"));
    r_commodity_table->setItem(0, 1, new QTableWidgetItem(""));
    r_commodity_table->setItem(1, 1, new QTableWidgetItem(""));
    r_commodity_table->setItem(2, 1, new QTableWidgetItem(""));
    r_commodity_table->setItem(3, 1, new QTableWidgetItem(""));
    r_commodity_table->setItem(4, 1, new QTableWidgetItem(""));
    r_commodity_table->setItem(5, 1, new QTableWidgetItem(""));
    r_commodity_table->setItem(6, 1, new QTableWidgetItem(""));
    r_commodity_table->setItem(7, 1, new QTableWidgetItem(""));
    for (int row = 0; row < 8; ++row)
    {
        QTableWidgetItem *item = r_commodity_table->item(row, 0);
        if (item)
        {
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        }
        item = r_commodity_table->item(row, 1);
        if (item)
        {
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        }
    }
    r_commodity_table->setShowGrid(false);
    r_commodity_table->verticalHeader()->setVisible(false);
    r_commodity_table->horizontalHeader()->setVisible(false);

    // 设置列宽和行高
    r_commodity_table->setColumnWidth(0, 100);                                            // 第一列固定宽度
    r_commodity_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch); // 第二列自动拉伸
    r_commodity_table->setRowHeight(7, 100);                                              // 描述行高度为100px

    // 6.右侧布局添加购买相关控件
    buyQuantitySpinBox = new QSpinBox(this);
    buyQuantitySpinBox->setMinimum(1); // 最小购买数量为1
    buyQuantitySpinBox->setMaximum(1); // 初始最大值设为1，动态更新
    buyQuantitySpinBox->setValue(1);   // 默认数量为1

    totalPriceLabel = new QLabel("总价格: 0.00", this);
    totalPriceLabel->setStyleSheet("font-size: 14pt;");

    addToCartButton = new QPushButton("加入购物车", this);
    buyNowButton = new QPushButton("直接购买", this);

    // 页面2-购物车=======================================================================================
    cartTable = new QTableWidget(this);
    cartTable->setColumnCount(8);
    cartTable->setHorizontalHeaderLabels({"商品名称", "种类", "单价", "数量", "总价", "库存", "操作", "ID"});
    cartTable->setShowGrid(true);
    cartTable->verticalHeader()->setVisible(false);
    cartTable->setSelectionMode(QAbstractItemView::NoSelection);
    cartTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    cartTable->setColumnWidth(0, 150);   // 商品名称
    cartTable->setColumnWidth(1, 100);   // 种类
    cartTable->setColumnWidth(2, 100);   // 单价
    cartTable->setColumnWidth(3, 80);    // 数量
    cartTable->setColumnWidth(4, 100);   // 总价
    cartTable->setColumnWidth(5, 80);    // 库存
    cartTable->setColumnWidth(6, 150);   // 操作（原来第5列，现为第6列）
    cartTable->setColumnHidden(7, true); // 隐藏 ID 列（原来第6列，现为第7列）
    cartTable->horizontalHeader()->setStretchLastSection(true);

    cartTable->verticalHeader()->setDefaultSectionSize(40); // 设置默认行高为50像素

    clearCartButton = new QPushButton("清空购物车", this);
    createOrder = new QPushButton("生成订单", this);
    emptyCartLabel = new QLabel("购物车为空", this);
    emptyCartLabel->setStyleSheet("font-size: 18pt;");
    emptyCartLabel->setAlignment(Qt::AlignCenter);

    // 页面3-商品管理=======================================================================================
    // 右侧控件
    tip = new QLabel("");
    rsb_b_change = new QPushButton("修改该商品");
    rsb_b_new = new QPushButton("添加新商品");
    rsb_b_clear = new QPushButton("清空");
    rsb_b_delete = new QPushButton("下架该商品");

    uniDiscoutEdit = new QLineEdit(this);
    uniDiscoutEdit->setPlaceholderText("输入折扣 要求(0,1]区间的小数");
    uniDiscoutCombo = new QComboBox(this);
    uniDiscoutCombo->addItems({"Book", "Clothing", "Food"});
    uniDiscoutButton = new QPushButton("使用折扣", this);

    // 页面4-我的订单=======================================================================================
    orderListView = new QListView(this);
    orderListView->setViewMode(QListView::ListMode); // 改为列表模式以更好地展示详细信息
    orderListView->setResizeMode(QListView::Adjust);
    orderListView->setMovement(QListView::Static);
    orderListView->setSpacing(5);
    orderListView->setFocusPolicy(Qt::NoFocus); // 禁用焦点
    orderListView->setStyleSheet(
        "QListView {"
        "    background-color: transparent;" // 背景透明，与 CommodityDelegate 一致
        "    border: none;"                  // 无边框
        "    padding: 5px;"                  // 内边距
        "}"
        "QListView::item {"
        "    background-color: #FBFBFD;" // 默认背景色，与 CommodityDelegate 的 fillPath 一致
        "    border: 1px solid #C9CCD1;" // 浅灰色边框，与 CommodityDelegate 的 QPen(Qt::lightGray, 1) 一致
        "    border-radius: 5px;"        // 圆角边框，与 CommodityDelegate 的 addRoundedRect 一致
        "    margin: 5px;"               // 外边距
        "    padding: 10px;"             // 内边距
        "}"
        "QListView::item:hover {"
        "    background-color: #F4F6F9;" // 悬停时的背景色，与 CommodityDelegate 的 State_MouseOver 一致
        "    border: 1px solid #C9CCD1;" // 保持浅灰色边框
        "}"
        "QListView::item:selected {"
        "    border-bottom: 2px solid #003E92;" // 仅下边框显示深蓝色，2px 宽度
        "    border-radius: 5px;"
        "    color: #000000;" // 选中时字体颜色保持黑色
        "}");

    r_order_table = new QTableWidget(this);
    r_order_table->setColumnCount(2);
    r_order_table->setRowCount(5);
    r_order_table->setHorizontalHeaderLabels({"字段", "值"});
    r_order_table->setItem(0, 0, new QTableWidgetItem(" 订单ID"));
    r_order_table->setItem(1, 0, new QTableWidgetItem(" 用户名"));
    r_order_table->setItem(2, 0, new QTableWidgetItem(" 总价"));
    r_order_table->setItem(3, 0, new QTableWidgetItem(" 状态"));
    r_order_table->setItem(4, 0, new QTableWidgetItem(" 创建时间"));
    for (int row = 0; row < 5; ++row)
    {
        r_order_table->setItem(row, 1, new QTableWidgetItem(""));
        QTableWidgetItem *item = r_order_table->item(row, 0);
        if (item)
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        item = r_order_table->item(row, 1);
        if (item)
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    }
    r_order_table->setShowGrid(false);
    r_order_table->verticalHeader()->setVisible(false);
    r_order_table->horizontalHeader()->setVisible(false);
    r_order_table->setStyleSheet(
        "QTableWidget {"
        "    background-color: #FFFFFF;"
        "    border: 1px solid #E0E0E0;"
        "    border-radius: 5px;" // 圆角边框
        "    padding: 5px;"
        "}");

    r_order_table->setColumnWidth(0, 100);                                            // 第一列固定宽度（如 100 像素）
    r_order_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch); // 第二列自动拉伸占满剩余空间

    // 新增：订单商品表格
    r_order_items_table = new QTableWidget(this);
    r_order_items_table->setColumnCount(4);
    r_order_items_table->setHorizontalHeaderLabels({"名称", "原价", "折后价", "数量"});
    r_order_items_table->setShowGrid(false);
    r_order_items_table->verticalHeader()->setVisible(false);
    r_order_items_table->setSelectionMode(QAbstractItemView::NoSelection);
    r_order_items_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    r_order_items_table->setColumnWidth(0, 80); // 商品名称
    r_order_items_table->setColumnWidth(1, 70); // 原价
    r_order_items_table->setColumnWidth(2, 70); // 折后价
    r_order_items_table->setColumnWidth(3, 50); // 数量
    r_order_items_table->horizontalHeader()->setStretchLastSection(true);
    r_order_items_table->setStyleSheet(
        "QTableWidget {"
        "    background-color: #FFFFFF;"
        "    border: 1px solid #E0E0E0;"
        "    border-radius: 8px;" // 圆角边框
        "    padding: 5px;"
        "}");

    // 新增：总价标签和支付按钮
    r_order_total_label = new QLabel("总价: 0.00", this);
    r_order_total_label->setStyleSheet("font-size: 14pt; font-weight: bold;");
    r_order_delete_button = new QPushButton("删除订单", this);
    r_order_pay_button = new QPushButton("支付订单", this);
}
void MainWindow::initLayout()
{
    // 布局===============================================================================================================

    // 0.右侧页面容器
    pages = new QStackedWidget(this);
    QWidget *page1 = new QWidget();
    QWidget *page2 = new QWidget();
    QWidget *page3 = new QWidget();
    QWidget *page4 = new QWidget();
    QWidget *fakePage2 = new QWidget();
    QWidget *fakePage3 = new QWidget();
    QWidget *fakePage3_notMerchant = new QWidget();
    QWidget *fakePage4 = new QWidget();
    QWidget *page5 = new QWidget();
    QWidget *fakePage5 = new QWidget();
    pages->addWidget(page1);
    pages->addWidget(page2);
    pages->addWidget(fakePage2);
    pages->addWidget(page5);
    pages->addWidget(fakePage5);
    pages->addWidget(page3);
    pages->addWidget(fakePage3);
    pages->addWidget(fakePage3_notMerchant);
    pages->addWidget(page4);
    pages->addWidget(fakePage4);

    // 0.总布局
    QHBoxLayout *mainLayout = new QHBoxLayout;
    QVBoxLayout *l_sideLayout = new QVBoxLayout;
    mainLayout->addLayout(l_sideLayout, 1);
    mainLayout->addWidget(pages, 10);

    // 0.总布局左侧
    QVBoxLayout *profileLayout = new QVBoxLayout;
    QHBoxLayout *welcomeNameLayout = new QHBoxLayout;
    QHBoxLayout *loginOrLogoutLayout = new QHBoxLayout;
    QHBoxLayout *changeOrChargeLayout = new QHBoxLayout;
    l_sideLayout->addLayout(profileLayout);
    l_sideLayout->addWidget(menuBar);
    profileLayout->addLayout(welcomeNameLayout);
    welcomeNameLayout->addWidget(l_profile_welcome);
    welcomeNameLayout->addWidget(l_profile_userName);
    profileLayout->addWidget(l_profile_balance);
    loginOrLogoutLayout->addWidget(l_profile_login);
    loginOrLogoutLayout->addWidget(l_profile_logout);
    profileLayout->addLayout(loginOrLogoutLayout);
    profileLayout->addLayout(changeOrChargeLayout);
    changeOrChargeLayout->addWidget(l_profile_changePassword);
    changeOrChargeLayout->addWidget(l_profile_charge);

    // 0.总布局右侧
    QHBoxLayout *page1_homeLayout = new QHBoxLayout;
    QHBoxLayout *page2_shoppingCartLayout = new QHBoxLayout;
    QHBoxLayout *page3_commoditiesManagementLayout = new QHBoxLayout;
    QHBoxLayout *page4_settingLayout = new QHBoxLayout;
    QHBoxLayout *page5_orderLayout = new QHBoxLayout;
    QHBoxLayout *page_unlogged1 = new QHBoxLayout;
    QHBoxLayout *page_unlogged2 = new QHBoxLayout;
    QHBoxLayout *page_unlogged3 = new QHBoxLayout;
    QHBoxLayout *page_unlogged4 = new QHBoxLayout;
    QHBoxLayout *page_notMerchant = new QHBoxLayout;
    page1->setLayout(page1_homeLayout);
    page2->setLayout(page2_shoppingCartLayout);
    page3->setLayout(page3_commoditiesManagementLayout);
    page4->setLayout(page4_settingLayout);
    page5->setLayout(page5_orderLayout);
    fakePage2->setLayout(page_unlogged1);
    fakePage3->setLayout(page_unlogged2);
    fakePage3_notMerchant->setLayout(page_notMerchant);
    fakePage4->setLayout(page_unlogged3);
    fakePage5->setLayout(page_unlogged4);

    // 1.页面1-主页
    QVBoxLayout *m_sideLayout = new QVBoxLayout;
    QVBoxLayout *r_sideLayout = new QVBoxLayout;
    QHBoxLayout *filterLayout = new QHBoxLayout;

    page1_homeLayout->addLayout(m_sideLayout, 6);
    page1_homeLayout->addLayout(r_sideLayout, 4);
    m_sideLayout->addLayout(filterLayout);
    m_sideLayout->addWidget(listView);
    r_sideLayout->addWidget(r_commodity_table);

    unloggedLabel4 = new QLabel("登录后可以使用购买功能", this);
    unloggedLabel4->setStyleSheet("font-size: 10.5pt;");
    unloggedLabel4->setAlignment(Qt::AlignCenter); // 居中对齐

    r_sideLayout->addWidget(unloggedLabel4);

    filterLayout->addWidget(filterEdit);
    filterLayout->addWidget(filterCombo);

    r_sideLayout->addWidget(buyQuantitySpinBox);
    r_sideLayout->addWidget(totalPriceLabel);
    r_sideLayout->addWidget(addToCartButton);
    r_sideLayout->addWidget(buyNowButton);

    // 2.页面2-购物车
    QVBoxLayout *cartLayout = new QVBoxLayout;
    cartLayout->addWidget(cartTable);
    QHBoxLayout *cartButtonLayout = new QHBoxLayout;
    cartButtonLayout->addWidget(clearCartButton);
    cartButtonLayout->addWidget(createOrder);
    cartLayout->addLayout(cartButtonLayout);
    cartLayout->addWidget(emptyCartLabel);
    page2_shoppingCartLayout->addLayout(cartLayout);

    // 3.页面3-商品管理
    QVBoxLayout *m_sideLayout_page3 = new QVBoxLayout;
    QVBoxLayout *r_sideLayout_page3 = new QVBoxLayout;
    QHBoxLayout *filterLayout_page3 = new QHBoxLayout;

    page3_commoditiesManagementLayout->addLayout(m_sideLayout_page3, 6);
    page3_commoditiesManagementLayout->addLayout(r_sideLayout_page3, 4);

    // 筛选控件
    filterEdit_page3 = new QLineEdit(this);
    filterEdit_page3->setPlaceholderText("输入筛选关键词");
    filterCombo_page3 = new QComboBox(this);
    filterCombo_page3->addItems({"名称 ", "种类 "});
    filterLayout_page3->addWidget(filterEdit_page3);
    filterLayout_page3->addWidget(filterCombo_page3);

    // 商品列表
    listView_page3 = new QListView(this);
    listView_page3->setViewMode(QListView::IconMode);
    listView_page3->setGridSize(QSize(150, 200));
    listView_page3->setResizeMode(QListView::Adjust);
    listView_page3->setMovement(QListView::Static);
    listView_page3->setSpacing(10);
    listView_page3->setStyleSheet("QListView { background-color: transparent; border: none; } ");

    // 页面3 - 商品管理：加载当前商家的商品

    // 1. 创建并设置QStandardItemModel
    QStandardItemModel *model_page3 = new QStandardItemModel(this);
    // 先将基础模型设置给listView_page3
    listView_page3->setModel(model_page3);
    // 设置自定义的ItemDelegate来绘制商品卡片
    listView_page3->setItemDelegate(new CommodityDelegate(this));

    // 2. 创建并设置QSortFilterProxyModel (代理模型)
    QSortFilterProxyModel *proxyModel_page3 = new QSortFilterProxyModel(this);
    // 将基础模型作为代理模型的源模型
    proxyModel_page3->setSourceModel(model_page3);
    // 默认筛选角色设置为DisplayRole (通常是商品名称)
    proxyModel_page3->setFilterRole(Qt::DisplayRole);
    // 将代理模型设置给listView_page3，现在listView_page3将通过代理模型显示数据
    listView_page3->setModel(proxyModel_page3);

    // 3. 连接筛选控件信号
    // 当filterEdit_page3的文本改变时，更新代理模型的正则表达式过滤器
    connect(filterEdit_page3, &QLineEdit::textChanged, this, [proxyModel_page3](const QString &text)
            {
        // 设置不区分大小写的正则表达式过滤
        proxyModel_page3->setFilterRegularExpression(QRegularExpression(text, QRegularExpression::CaseInsensitiveOption)); });
    // 当filterCombo_page3的选择改变时，更新代理模型的筛选角色和列
    connect(filterCombo_page3, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [proxyModel_page3](int index)
            {
        switch (index) {
        case 0: // 名称
            proxyModel_page3->setFilterRole(Qt::DisplayRole); // 使用Qt::DisplayRole进行筛选
            proxyModel_page3->setFilterKeyColumn(0); // 筛选第一列（通常是名称）
            break;
        case 1: // 种类
            proxyModel_page3->setFilterRole(Qt::UserRole + 1); // 使用Qt::UserRole + 1 (类型)进行筛选
            proxyModel_page3->setFilterKeyColumn(0); // 筛选第一列（因为数据都存在item的role里，key column通常是0）
            break;
            // 对于商家自己的商品管理页面，通常不需要按“商家”筛选，因为所有商品都属于当前商家。
            // 如果需要，可以添加 case 2: proxyModel_page3->setFilterRole(Qt::UserRole + 5); proxyModel_page3->setFilterKeyColumn(0); break;
        } });

    // 右侧商品详细信息
    // 保留 r_commodity_table_page3 的初始化
    r_commodity_table_page3 = new QTableWidget(this);
    r_commodity_table_page3->setColumnCount(2);
    r_commodity_table_page3->setRowCount(8);
    r_commodity_table_page3->setHorizontalHeaderLabels({"字段", "值"});
    r_commodity_table_page3->setItem(0, 0, new QTableWidgetItem(" 商品名称"));
    r_commodity_table_page3->setItem(1, 0, new QTableWidgetItem(" 种类"));
    r_commodity_table_page3->setItem(2, 0, new QTableWidgetItem(" 来源商家"));
    r_commodity_table_page3->setItem(3, 0, new QTableWidgetItem(" 库存"));
    r_commodity_table_page3->setItem(4, 0, new QTableWidgetItem(" 原价"));
    r_commodity_table_page3->setItem(5, 0, new QTableWidgetItem(" 折扣"));
    r_commodity_table_page3->setItem(6, 0, new QTableWidgetItem(" 统一折扣"));
    r_commodity_table_page3->setItem(7, 0, new QTableWidgetItem(" 描述")); // 设置第二列内容换行
    r_commodity_table_page3->setWordWrap(true);                            // 启用表格整体换行
    // 在 initLayout() 方法中的商品管理页面表格初始化部分
    for (int row = 0; row < 8; ++row)
    {
        QTableWidgetItem *item = new QTableWidgetItem("");
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        // 如果是来源商家行（第2行）或统一折扣行（第6行），设为不可编辑
        if (row == 2 && currentUser && isMerchant)
        {
            item->setText(currentUser->getName());
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        }
        else if (row == 6)
        { // 统一折扣行不可编辑
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            item->setToolTip("统一折扣由系统统一管理，不可单独修改");
        }
        else
        {
            // 其他行在商家模式下默认可编辑
            if (isMerchant)
            {
                item->setFlags(item->flags() | Qt::ItemIsEditable);
            }
            else
            {
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            }
        }

        r_commodity_table_page3->setItem(row, 1, item);

        // 第一列（字段名）始终不可编辑
        item = r_commodity_table_page3->item(row, 0);
        if (item)
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    }

    // 设置列宽
    r_commodity_table_page3->setColumnWidth(0, 100);                                            // 第一列固定宽度（如 100 像素）
    r_commodity_table_page3->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch); // 第二列自动拉伸占满剩余空间

    // 调整行高以适应内容
    r_commodity_table_page3->setRowHeight(7, 100);

    // 隐藏网格线和表头
    r_commodity_table_page3->setShowGrid(false);
    r_commodity_table_page3->verticalHeader()->setVisible(false);
    r_commodity_table_page3->horizontalHeader()->setVisible(false);

    QHBoxLayout *ManagementLayout = new QHBoxLayout;
    QHBoxLayout *uniDiscoutLayout = new QHBoxLayout;
    QHBoxLayout *modifyOrDeleteLayout = new QHBoxLayout;

    modifyOrDeleteLayout->addWidget(rsb_b_change);
    modifyOrDeleteLayout->addWidget(rsb_b_delete);
    ManagementLayout->addWidget(rsb_b_new);
    ManagementLayout->addWidget(rsb_b_clear);
    uniDiscoutLayout->addWidget(uniDiscoutEdit);
    uniDiscoutLayout->addWidget(uniDiscoutCombo);
    uniDiscoutLayout->addWidget(uniDiscoutButton);

    m_sideLayout_page3->addLayout(filterLayout_page3);
    m_sideLayout_page3->addWidget(listView_page3);
    r_sideLayout_page3->addWidget(tip);
    r_sideLayout_page3->addWidget(r_commodity_table_page3);
    r_sideLayout_page3->addLayout(uniDiscoutLayout);
    r_sideLayout_page3->addLayout(ManagementLayout);
    r_sideLayout_page3->addLayout(modifyOrDeleteLayout);

    // 4.页面4-设置

    // 5.页面5-我的订单
    QVBoxLayout *m_sideLayout_page5 = new QVBoxLayout;
    QVBoxLayout *r_sideLayout_page5 = new QVBoxLayout;

    page5_orderLayout->addLayout(m_sideLayout_page5, 6);
    page5_orderLayout->addLayout(r_sideLayout_page5, 4);

    m_sideLayout_page5->addWidget(orderListView);
    r_sideLayout_page5->addWidget(r_order_table);
    r_sideLayout_page5->addSpacing(10);
    r_sideLayout_page5->addWidget(r_order_items_table);
    r_sideLayout_page5->addWidget(r_order_total_label);
    r_sideLayout_page5->addWidget(r_order_delete_button);
    r_sideLayout_page5->addWidget(r_order_pay_button);

    // 6.其他伪页面（未登录页面 和 不是商家页面）
    QLabel *unloggedLabel1 = new QLabel("登录后可以使用此功能", this);
    QLabel *unloggedLabel2 = new QLabel("登录后可以使用此功能", this);
    QLabel *unloggedLabel3 = new QLabel("登录后可以使用此功能", this);
    QLabel *unloggedLabel4 = new QLabel("登录后可以使用此功能", this);
    QLabel *notMerchant = new QLabel("只有商家可以使用此功能", this);
    unloggedLabel1->setStyleSheet("font-size: 18pt;"); // 字体大小设为 24pt
    unloggedLabel2->setStyleSheet("font-size: 18pt;");
    unloggedLabel3->setStyleSheet("font-size: 18pt;");
    notMerchant->setStyleSheet("font-size: 18pt;");
    unloggedLabel4->setStyleSheet("font-size: 18pt;");
    unloggedLabel4->setAlignment(Qt::AlignCenter); // 居中对齐
    unloggedLabel1->setAlignment(Qt::AlignCenter); // 居中对齐
    unloggedLabel2->setAlignment(Qt::AlignCenter);
    unloggedLabel3->setAlignment(Qt::AlignCenter);
    notMerchant->setAlignment(Qt::AlignCenter);
    page_unlogged1->addWidget(unloggedLabel1);
    page_unlogged2->addWidget(unloggedLabel2);
    page_unlogged3->addWidget(unloggedLabel3);
    page_unlogged4->addWidget(unloggedLabel4);
    page_notMerchant->addWidget(notMerchant);

    // 7.应用布局
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    // 8.隐藏初始化
    l_profile_logout->hide();
    buyQuantitySpinBox->hide();
    totalPriceLabel->hide();
    addToCartButton->hide();
    buyNowButton->hide();
    l_profile_changePassword->hide();
    l_profile_charge->hide();

    cartTable->setRowCount(0);
    cartTable->hide();
    clearCartButton->hide();
    createOrder->hide();
    emptyCartLabel->setText("购物车为空");
    emptyCartLabel->show();
}

// 初始化商家表格编辑权限
void MainWindow::initMerchantTablePermissions()
{
    if (!isMerchant || !currentUser)
        return;

    // 设置商品管理页面表格的正确编辑权限
    for (int row = 0; row < 8; ++row)
    {
        QTableWidgetItem *item = r_commodity_table_page3->item(row, 1);
        if (!item)
        {
            item = new QTableWidgetItem("");
            item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            r_commodity_table_page3->setItem(row, 1, item);
        }

        if (row == 2)
        { // 来源商家行
            item->setText(currentUser->getName());
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        }
        else if (row == 6)
        { // 统一折扣行不可编辑
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            item->setToolTip("统一折扣由系统统一管理，不可单独修改");
        }
        else
        {
            // 确保其他行可编辑
            item->setFlags(item->flags() | Qt::ItemIsEditable);
        }
    }
}

// 清理商品管理页面表格信息（除了来源商家）
void MainWindow::clearMerchantTableInfo()
{
    if (!isMerchant || !currentUser)
        return;

    // 清空除来源商家外的所有字段
    for (int row = 0; row < 8; ++row)
    {
        QTableWidgetItem *item = r_commodity_table_page3->item(row, 1);
        if (!item)
        {
            item = new QTableWidgetItem("");
            item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            r_commodity_table_page3->setItem(row, 1, item);
        }

        if (row == 2)
        { // 来源商家行保留当前商家名，不可编辑
            item->setText(currentUser->getName());
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        }
        else if (row == 6)
        { // 统一折扣行不可编辑，但清空内容
            item->setText("");
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            item->setToolTip("统一折扣由系统统一管理，不可单独修改");
        }
        else
        {
            // 其他行清空内容，设为可编辑
            item->setText("");
            item->setFlags(item->flags() | Qt::ItemIsEditable);
        }
    }

    // 清空提示信息
    if (tip)
    {
        tip->setText("");
    }

    // 清理当前商品对象
    if (currentCommodity)
    {
        delete currentCommodity;
        currentCommodity = nullptr;
    }
}

// 左侧
void MainWindow::onChangePasswordClicked()
{
    if (!isLogin || !currentUser)
    {
        QMessageBox::warning(this, "提示", "请先登录！");
        return;
    }

    PasswordDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        QString oldPassword = dialog.getOldPassword();
        if (oldPassword != currentUser->getPassword())
        {
            QMessageBox::warning(this, "错误", "原密码错误！");
            return;
        }

        QString newPassword = dialog.getNewPassword();

        currentUser->changePassword(newPassword); // 更新 currentUser 的密码
    }
}
void MainWindow::handleChangePasswordResponse(bool success, const QString &message)
{

    if (success)
    {
        QMessageBox::information(this, "修改密码", "密码修改成功！\n" + message);
        qDebug() << "Change password successful:" << message;
        // 成功后，可能需要更新本地的 m_currentUser 对象的密码
        // m_currentUser.updateUserInfo(m_currentUser.getName(), newPassword, m_currentUser.getType(), m_currentUser.getBalance());
        // 或者直接将 m_currentUser.m_password 更新为 newPassword
    }
    else
    {
        QMessageBox::warning(this, "修改密码", "密码修改失败！\n" + message);
        qDebug() << "Change password failed:" << message;
    }
}

void MainWindow::onChargeButtonClicked()
{
    bool ok;
    double amount1 = QInputDialog::getDouble(this, "充值", "请输入充值金额：", 0.0, 0.0, 1000000.0, 2, &ok);
    if (!ok)
    {
        return; // 用户取消输入
    }

    if (amount1 <= 0)
    {
        QMessageBox::warning(this, "错误", "充值金额必须大于0！");
        return;
    }

    unsigned long long amount = static_cast<unsigned long long>(amount1 * 100.0);

    // 调用客户端 User 类的 recharge 方法发送请求
    // 这里的 m_currentUser 应该是当前登录的用户对象
    currentUser->recharge(amount);

    // 禁用按钮，防止重复点击
    // ui->rechargeButton->setEnabled(false);
    qDebug() << "MainWindow: Sending recharge request.";
}
void MainWindow::handleRechargeResponse(bool success, const QString &message, unsigned long long newBalance)
{
    // ui->rechargeButton->setEnabled(true); // 重新启用按钮

    if (success)
    {
        QMessageBox::information(this, "充值", "充值成功！\n" + message + "\n新余额: " + QString::number(static_cast<double>(newBalance) / 100.0, 'f', 2) + " 元");
        qDebug() << "Recharge successful:" << message << ", New Balance:" << newBalance;

        currentUser->setBalanceWithout100(newBalance);
        l_profile_balance->setText(QString("余额：%1").arg(static_cast<double>(newBalance) / 100.0, 0, 'f', 2));
    }
    else
    {
        QMessageBox::warning(this, "充值", "充值失败！\n" + message);
        qDebug() << "Recharge failed:" << message;
    }
}

void MainWindow::onLoginButtonClicked()
{
    lwindow->show();
}
void MainWindow::onLoginWindowClosed(const QString &username, const QString &password, const QString &type, unsigned long long balance)
{
    qDebug() << "test ↑ ajic:";
    delete currentUser;
    qDebug() << "test ↓ fajic:";
    currentUser = nullptr;
    currentUser = new User(username, password, type, balance);

    isLogin = true;
    isMerchant = (currentUser->getType() == "Merchant");

    l_profile_userName->setText(currentUser->getName());
    l_profile_balance->setText(QString("余额：%1").arg(static_cast<double>(currentUser->getBalance()) / 100.0, 0, 'f', 2));

    item2->setHidden(false); // 隐藏购物车
    fakeitem2->setHidden(true);

    item5->setHidden(false); // 隐藏购物车
    fakeitem5->setHidden(true);

    // item4->setHidden(false); // 隐藏设置
    // fakeitem4->setHidden(true);
    if (isMerchant)
    {
        item3->setHidden(false); // 显示商品管理
        fakeitem3->setHidden(true);
        fakeitem3_notMerchant->setHidden(true);

        // 初始化商品管理页面的表格编辑权限
        initMerchantTablePermissions();
    }
    else
    {
        item3->setHidden(true);
        fakeitem3->setHidden(true);
        fakeitem3_notMerchant->setHidden(false);
    }

    menuBar->setCurrentRow(0); // 默认选中主页

    l_profile_login->setText("切换账号");
    l_profile_logout->show();
    l_profile_changePassword->show();
    l_profile_charge->show();
    unloggedLabel4->hide();
}

void MainWindow::onLogoutButtonClicked()
{
    delete currentUser;
    currentUser = nullptr;

    isLogin = false;

    l_profile_userName->setText("游客");
    l_profile_balance->setText("登录以查看余额");

    item2->setHidden(true); // 隐藏购物车
    fakeitem2->setHidden(false);

    item5->setHidden(true); // 隐藏购物车
    fakeitem5->setHidden(false);

    item3->setHidden(true); // 显示商品管理
    fakeitem3->setHidden(false);
    fakeitem3_notMerchant->setHidden(true);

    menuBar->setCurrentRow(0); // 默认选中主页

    l_profile_login->setText("登录/注册");
    l_profile_logout->hide();
    l_profile_changePassword->hide();
    l_profile_charge->hide();
    unloggedLabel4->show();

    buyQuantitySpinBox->hide();
    totalPriceLabel->hide();
    addToCartButton->hide();
    buyNowButton->hide();
}

// 主页
void MainWindow::onCommoditySelected(const QModelIndex &index)
{
    // 获取代理模型
    QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(listView->model());
    if (!proxyModel)
        return;

    // 获取源模型中的索引
    QModelIndex sourceIndex = proxyModel->mapToSource(index);
    QStandardItemModel *sourceModel = qobject_cast<QStandardItemModel *>(proxyModel->sourceModel());
    if (!sourceModel)
        return;

    QStandardItem *item = sourceModel->itemFromIndex(sourceIndex);
    if (!item)
        return;

    // 获取商品 ID 并从数据库加载 Commodity 对象
    uint id = item->data(Qt::UserRole + 7).toUInt();
    qDebug() << "商品 ID:" << id;

    delete currentCommodity;
    currentCommodity = nullptr;
    currentCommodity = new Commodity();
    if (!currentCommodity)
    {
        QMessageBox::warning(this, tr("错误"), tr("无法加载商品信息"));
        return;
    }

    currentCommodity->send_fetchAllInformation(id, 1);
}
void MainWindow::handleFetchAllInfoOfCommodityResponse(bool success, const QString &message, const QVariantMap &commodityData)
{
    if (success)
    {
        qDebug() << "Successfully fetched all info for commodity. ID:" << commodityData["id"].toUInt()
                 << ", Name:" << commodityData["name"].toString();

        delete currentCommodity;
        currentCommodity = nullptr;
        currentCommodity = new Commodity(
            commodityData["id"].toUInt(),
            commodityData["name"].toString(),
            commodityData["type"].toString(),
            commodityData["description"].toString(),
            commodityData["originalPrice"].toULongLong(),
            commodityData["stock"].toUInt(),
            commodityData["sourceStoreName"].toString(),
            commodityData["discount"].toFloat(),
            commodityData["uniDiscount"].toFloat()); // 更新 QTableWidget - 按照新的字段顺序：商品名称、种类、来源商家、库存、原价、折扣、统一折扣、描述
        r_commodity_table->setItem(0, 1, new QTableWidgetItem(currentCommodity->getName()));
        r_commodity_table->setItem(1, 1, new QTableWidgetItem(currentCommodity->getType()));
        r_commodity_table->setItem(2, 1, new QTableWidgetItem(currentCommodity->getSourceStoreName()));
        r_commodity_table->setItem(3, 1, new QTableWidgetItem(QString::number(currentCommodity->getStock())));
        r_commodity_table->setItem(4, 1, new QTableWidgetItem(QString::number(static_cast<double>(currentCommodity->getOriginalPrice()) / 100.0, 'f', 2)));
        r_commodity_table->setItem(5, 1, new QTableWidgetItem(QString::number(currentCommodity->getDiscount())));
        r_commodity_table->setItem(6, 1, new QTableWidgetItem(QString::number(currentCommodity->getUniDiscount())));
        r_commodity_table->setItem(7, 1, new QTableWidgetItem(currentCommodity->getDescription()));

        // 设置新添加的项为只读
        for (int row = 0; row < 8; ++row)
        {
            QTableWidgetItem *tableItem = r_commodity_table->item(row, 1);
            if (tableItem)
            {
                tableItem->setFlags(tableItem->flags() & ~Qt::ItemIsEditable);
            }
        }

        if (currentCommodity && isLogin)
        {
            buyQuantitySpinBox->setMaximum(currentCommodity->getStock()); // 设置最大购买数量为库存
            buyQuantitySpinBox->setValue(1);                              // 重置为1
            updateTotalPrice(1);                                          // 更新总价格
            buyQuantitySpinBox->show();
            totalPriceLabel->show();
            addToCartButton->show();
            buyNowButton->show();
        }
        else
        {
            buyQuantitySpinBox->hide();
            totalPriceLabel->hide();
            addToCartButton->hide();
            buyNowButton->hide();
        }
    }
    else
    {
        qDebug() << "Failed to fetch all info for commodity:" << message;
        QMessageBox::warning(this, "Error", "Failed to load commodity details: " + message);
    }
}

void MainWindow::onBuyNowClicked()
{
    if (!currentCommodity || !isLogin)
    {
        QMessageBox::warning(this, "错误", "请先登录或选择商品！");
        return;
    }
    if (!currentUser)
    {
        QMessageBox::warning(this, "错误", "用户信息不可用！");
        return;
    }

    unsigned int quantity = buyQuantitySpinBox->value();
    QString errorMessage;
    unsigned long long totalPrice = currentCommodity->Getprice() * quantity;

    // 检查库存
    if (quantity > currentCommodity->getStock() || currentCommodity->getStock() == 0)
    {
        QMessageBox::warning(this, "错误", "库存不足！");
        return;
    }

    // 调用购买函数
    currentUser->purchase(totalPrice, currentUser->getName(), currentCommodity->getId(), quantity, currentCommodity->getSourceStoreName());

    buyNowButton->setEnabled(false);
}
void MainWindow::handlePurchaseResponse(bool success, const QString &message, unsigned long long newBuyerBalance, unsigned int newCommodityStock)
{
    buyNowButton->setEnabled(true); // 无论成功失败，重新启用购买按钮

    if (success)
    {
        QMessageBox::information(this, "购买成功", "购买成功\n您的新余额：" + QString::number(newBuyerBalance / 100.0, 'f', 2) + "元");
        qDebug() << "Purchase successful. New Buyer Balance:" << newBuyerBalance;
        currentUser->setBalanceWithout100(newBuyerBalance);
        currentCommodity->setStock(newCommodityStock);
        l_profile_balance->setText(QString("余额：%1").arg(static_cast<double>(currentUser->getBalance()) / 100.0, 0, 'f', 2));
        r_commodity_table->setItem(6, 1, new QTableWidgetItem(QString::number(currentCommodity->getStock())));
        // 刷新 listView（主页商品列表）
        QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(listView->model());
        if (proxyModel)
        {
            QStandardItemModel *sourceModel = qobject_cast<QStandardItemModel *>(proxyModel->sourceModel());
            if (sourceModel)
            {
                for (int row = 0; row < sourceModel->rowCount(); ++row)
                {
                    QStandardItem *item = sourceModel->item(row);
                    if (item && item->data(Qt::UserRole + 7).toUInt() == currentCommodity->getId())
                    {
                        item->setData(currentCommodity->getStock(), Qt::UserRole + 4);
                        QModelIndex sourceIndex = sourceModel->index(row, 0);
                        proxyModel->dataChanged(sourceIndex, sourceIndex);
                        break;
                    }
                }
            }
        }
    }
    else
    {
        QMessageBox::warning(this, "购买失败", "购买失败：" + message);
        qDebug() << "Purchase failed:" << message;
        refreshCommodityListViews();
    }
}

void MainWindow::updateTotalPrice(int quantity)
{
    if (currentCommodity)
    {
        unsigned long long price = currentCommodity->Getprice();
        double total = static_cast<double>(price * quantity) / 100.0;
        totalPriceLabel->setText(QString("总价格: %1").arg(total, 0, 'f', 2));
    }
}

void MainWindow::onAddToCartClicked()
{
    if (!currentCommodity || !isLogin || !currentUser)
    {
        QMessageBox::warning(this, "错误", "请先登录或选择商品！");
        return;
    }

    uint commodityId = currentCommodity->getId();
    unsigned int quantity = buyQuantitySpinBox->value();

    // 检查库存
    if (quantity > currentCommodity->getStock())
    {
        QMessageBox::warning(this, "错误", "库存不足！");
        return;
    }

    // 禁用按钮防止重复点击
    addToCartButton->setEnabled(false);

    // 调用购物车的 addItem 方法，这会发送网络请求到服务器
    if (!currentUser->getShoppingCart())
    {
        ShoppingCart *tmp = new ShoppingCart(currentUser->getName(), this);
        currentUser->setShoppingCart(tmp);
        connect(currentUser->getShoppingCart(), &ShoppingCart::cartUpdated, this, &MainWindow::onCartUpdated);
    }

    QString errorMessage;
    currentUser->getShoppingCart()->addItem(commodityId, quantity, errorMessage);
}
void MainWindow::handleAddToCartResponse(bool success, const QString &message, const QJsonArray &updatedCartItems)
{
    // 重新启用按钮
    addToCartButton->setEnabled(true);

    if (success)
    {
        QMessageBox::information(this, "成功", QString("已添加 %1 件 %2 到购物车").arg(buyQuantitySpinBox->value()).arg(currentCommodity ? currentCommodity->getName() : "商品"));

        qDebug() << "MainWindow: Add to cart successful. Message:" << message;

        // 如果服务器返回了更新后的购物车数据，可以选择性地更新购物车显示
        // 这里不强制更新，让用户切换到购物车页面时再加载
    }
    else
    {
        QMessageBox::warning(this, "错误", "添加到购物车失败：" + message);
        qDebug() << "MainWindow: Add to cart failed. Message:" << message;
    }
}

// 主页的商品管理
void MainWindow::refreshCommodityListViews()
{
    // 请求主页的所有商品
    requestCommodities("", "main_page");

    // 如果是商家，请求其自己的商品
    if (isLogin && isMerchant)
    {
        requestCommodities(currentUser->getName(), "merchant_page");
    }
}
void MainWindow::refreshMerchantCommodityListViews()
{
    if (isMerchant)
    {
        requestCommodities(currentUser->getName(), "merchant_page");
    }
}
void MainWindow::requestCommodities(const QString &storeName, const QString &context)
{
    QJsonObject jsonObj;
    jsonObj["mtype"] = "fetch_commodities_request";
    jsonObj["context"] = context; // 用于区分是哪个页面的请求 ("main_page" 或 "merchant_page")

    if (!storeName.isEmpty())
    {
        jsonObj["storeName"] = storeName;
    }
    HandleMessage::sendJsonData(&tcpSocket, jsonObj);
}
void MainWindow::populateModel(QStandardItemModel *model, const QList<QVariantMap> &commodities, bool isPage3)
{
    qDebug() << "finishaaaaaa";
    if (!model)
        return;     // defensive check
    model->clear(); // 清空现有数据

    for (const QVariantMap &commodity : commodities)
    {
        QStandardItem *item = new QStandardItem(commodity["name"].toString());
        item->setData(commodity["type"], Qt::UserRole + 1);
        item->setData(commodity["description"], Qt::UserRole + 2);
        item->setData(commodity["originalPrice"], Qt::UserRole + 3);
        item->setData(commodity["stock"], Qt::UserRole + 4);
        item->setData(commodity["sourceStoreName"], Qt::UserRole + 5);
        item->setData(commodity["discount"], Qt::UserRole + 6);
        item->setData(commodity["id"], Qt::UserRole + 7);
        if (isPage3)
        { // 只有商品管理页才需要 uniDiscount
            item->setData(commodity["uniDiscount"], Qt::UserRole + 8);
        }
        model->appendRow(item);
    }
}
void MainWindow::handleCommoditiesResponse(const QJsonDocument &responseDoc, const QString &context)
{
    if (!responseDoc.isObject())
    {
        qDebug() << "Invalid JSON response for commodities.";
        return;
    }

    QJsonObject responseObj = responseDoc.object();
    if (responseObj["status"].toString() != "success")
    { // 假设服务端会返回状态
        qDebug() << "Failed to fetch commodities:" << responseObj["message"].toString();
        return;
    }

    QJsonArray commoditiesArray = responseObj["commodities"].toArray();
    QList<QVariantMap> commodities;
    for (const QJsonValue &value : commoditiesArray)
    {
        if (value.isObject())
        {
            commodities.append(value.toObject().toVariantMap());
        }
    }

    // 根据 context 更新对应的 ListView
    if (context == "main_page")
    {
        QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(listView->model());
        if (proxyModel)
        {
            QStandardItemModel *model = qobject_cast<QStandardItemModel *>(proxyModel->sourceModel());
            populateModel(model, commodities, false);
        }
    }
    else if (context == "merchant_page")
    {
        QSortFilterProxyModel *proxyModel_page3 = qobject_cast<QSortFilterProxyModel *>(listView_page3->model());
        if (proxyModel_page3)
        {
            QStandardItemModel *model_page3 = qobject_cast<QStandardItemModel *>(proxyModel_page3->sourceModel());
            populateModel(model_page3, commodities, true); // 传递 true 表示是 page3
        }
    }
}

// 商品管理页
void MainWindow::onCommoditySelectedPage3(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(listView_page3->model());
    if (!proxyModel)
        return;

    QModelIndex sourceIndex = proxyModel->mapToSource(index);
    QStandardItemModel *sourceModel = qobject_cast<QStandardItemModel *>(proxyModel->sourceModel());
    if (!sourceModel)
        return;

    QStandardItem *item = sourceModel->itemFromIndex(sourceIndex);
    if (!item)
        return;

    uint id = item->data(Qt::UserRole + 7).toUInt();
    delete currentCommodity;
    currentCommodity = nullptr;
    currentCommodity = new Commodity();
    if (!currentCommodity)
    {
        QMessageBox::warning(this, tr("错误"), tr("无法加载商品信息"));
        return;
    }

    currentCommodity->send_fetchAllInformation(id, 3);

    // 更新提示信息
    tip->setText(QString("提示：您正在修改商品 %1，编号 %2").arg(currentCommodity->getName()).arg(id));
}
void MainWindow::handleFetchAllInfoOfCommodityResponse_page3(bool success, const QString &message, const QVariantMap &commodityData)
{
    if (success)
    {
        qDebug() << "Successfully fetched all info for commodity. ID:" << commodityData["id"].toUInt()
                 << ", Name:" << commodityData["name"].toString();

        delete currentCommodity;
        currentCommodity = nullptr;
        currentCommodity = new Commodity(
            commodityData["id"].toUInt(),
            commodityData["name"].toString(),
            commodityData["type"].toString(),
            commodityData["description"].toString(),
            commodityData["originalPrice"].toULongLong(),
            commodityData["stock"].toUInt(),
            commodityData["sourceStoreName"].toString(),
            commodityData["discount"].toFloat(),
            commodityData["uniDiscount"].toFloat()); // 更新 r_commodity_table_page3 - 按照新的字段顺序：商品名称、种类、来源商家、库存、原价、折扣、统一折扣、描述
        r_commodity_table_page3->setItem(0, 1, new QTableWidgetItem(currentCommodity->getName()));
        r_commodity_table_page3->setItem(1, 1, new QTableWidgetItem(currentCommodity->getType()));
        r_commodity_table_page3->setItem(2, 1, new QTableWidgetItem(currentCommodity->getSourceStoreName()));
        r_commodity_table_page3->setItem(3, 1, new QTableWidgetItem(QString::number(currentCommodity->getStock())));
        r_commodity_table_page3->setItem(4, 1, new QTableWidgetItem(QString::number(static_cast<double>(currentCommodity->getOriginalPrice()) / 100.0, 'f', 2)));
        r_commodity_table_page3->setItem(5, 1, new QTableWidgetItem(QString::number(currentCommodity->getDiscount())));
        r_commodity_table_page3->setItem(6, 1, new QTableWidgetItem(QString::number(currentCommodity->getUniDiscount())));
        r_commodity_table_page3->setItem(7, 1, new QTableWidgetItem(currentCommodity->getDescription()));
        r_commodity_table_page3->resizeColumnsToContents();

        // 设置表格可编辑（仅在商品管理模式下）
        for (int row = 0; row < 8; ++row)
        {
            QTableWidgetItem *tableItem = r_commodity_table_page3->item(row, 1);
            if (tableItem && isMerchant)
            {
                tableItem->setFlags(tableItem->flags() | Qt::ItemIsEditable);
            }
        }
        // 来源商家字段不可编辑（锁定为当前商家）
        if (r_commodity_table_page3->item(2, 1))
        {
            r_commodity_table_page3->item(2, 1)->setFlags(r_commodity_table_page3->item(2, 1)->flags() & ~Qt::ItemIsEditable);
        }
        if (r_commodity_table_page3->item(6, 1))
        {
            r_commodity_table_page3->item(6, 1)->setFlags(r_commodity_table_page3->item(6, 1)->flags() & ~Qt::ItemIsEditable);
        }
    }
    else
    {
        qDebug() << "Failed to fetch all info for commodity:" << message;
        QMessageBox::warning(this, "Error", "Failed to load commodity details: " + message);
    }
}

void MainWindow::onChangeGoodsButtonClicked()
{
    // 检查是否选择了商品
    if (!currentCommodity)
    {
        QMessageBox::warning(this, "错误", "没有选择要修改的商品！");
        return;
    } // 从 r_commodity_table_page3 读取数据
    QString name = r_commodity_table_page3->item(0, 1) ? r_commodity_table_page3->item(0, 1)->text() : "";
    QString kindStr = r_commodity_table_page3->item(1, 1) ? r_commodity_table_page3->item(1, 1)->text() : "";
    QString fromMerchant = currentUser ? currentUser->getName() : ""; // 强制使用当前商家
    QString surplusStr = r_commodity_table_page3->item(3, 1) ? r_commodity_table_page3->item(3, 1)->text() : "0";
    QString originalPriceStr = r_commodity_table_page3->item(4, 1) ? r_commodity_table_page3->item(4, 1)->text() : "0";
    QString discountStr = r_commodity_table_page3->item(5, 1) ? r_commodity_table_page3->item(5, 1)->text() : "1";
    QString uniDiscountStr = r_commodity_table_page3->item(6, 1) ? r_commodity_table_page3->item(6, 1)->text() : "1";
    QString description = r_commodity_table_page3->item(7, 1) ? r_commodity_table_page3->item(7, 1)->text() : "";

    // 转换数据类型
    double originalPrice_double = originalPriceStr.toDouble() * 100.0;
    unsigned long long originalPrice = static_cast<unsigned long long>(originalPrice_double);
    double discount = discountStr.toDouble();
    double uniDiscount = uniDiscountStr.toDouble();
    unsigned int surplus = surplusStr.toInt();
    int id = currentCommodity->getId();

    // 验证输入
    if (name.isEmpty() || kindStr.isEmpty() || fromMerchant.isEmpty())
    {
        QMessageBox::warning(this, "错误", "商品名称、种类和来源商家不能为空！");
        return;
    }
    if (!(kindStr == "Food" || kindStr == "Clothing" || kindStr == "Book"))
    {
        QMessageBox::warning(this, "错误", "商品种类只能是Food, Clothing或Book");
        return;
    }
    if (originalPrice <= 0 || discount <= 0 || discount > 1 || uniDiscount <= 0 || uniDiscount > 1)
    {
        QMessageBox::warning(this, "错误", "原价、折扣和统一折扣必须为有效值（折扣和统一折扣在(0,1]区间）！");
        return;
    }

    currentCommodity->updateCommodity(id, name, kindStr, description, originalPrice, surplus, fromMerchant, discount, uniDiscount);
    rsb_b_change->setEnabled(false);
}
void MainWindow::handleCommodityUpdateResponse(bool success, const QString &message)
{
    rsb_b_change->setEnabled(true);
    if (success)
    {
        QMessageBox::information(this, "商品更新", "商品信息更新成功！\n" + message);
        qDebug() << "Commodity update successful:" << message;
        refreshMerchantCommodityListViews();
    }
    else
    {
        QMessageBox::warning(this, "商品更新", "商品信息更新失败！\n" + message);
        qDebug() << "Commodity update failed:" << message;
    }
}

void MainWindow::onNewGoodsButtonClicked()
{
    // 从 r_commodity_table_page3 读取数据
    QString name = r_commodity_table_page3->item(0, 1) ? r_commodity_table_page3->item(0, 1)->text() : "";
    QString kindStr = r_commodity_table_page3->item(1, 1) ? r_commodity_table_page3->item(1, 1)->text() : "";
    QString fromMerchant = currentUser ? currentUser->getName() : ""; // 强制使用当前商家
    QString surplusStr = r_commodity_table_page3->item(3, 1) ? r_commodity_table_page3->item(3, 1)->text() : "1";
    QString originalPriceStr = r_commodity_table_page3->item(4, 1) ? r_commodity_table_page3->item(4, 1)->text() : "0";
    QString discountStr = r_commodity_table_page3->item(5, 1) ? r_commodity_table_page3->item(5, 1)->text() : "1";
    QString uniDiscountStr = r_commodity_table_page3->item(6, 1) ? r_commodity_table_page3->item(6, 1)->text() : "1";
    QString description = r_commodity_table_page3->item(7, 1) ? r_commodity_table_page3->item(7, 1)->text() : "";

    // 转换数据类型
    double originalPrice_double = originalPriceStr.toDouble();
    unsigned long long originalPrice = static_cast<unsigned long long>(originalPrice_double * 100.0);
    double discount = discountStr.toDouble();
    double uniDiscount = uniDiscountStr.toDouble();
    unsigned int surplus = surplusStr.toInt();

    // 验证输入
    if (name.isEmpty() || kindStr.isEmpty() || fromMerchant.isEmpty())
    {
        QMessageBox::warning(this, "错误", "商品名称、种类和来源商家不能为空！");
        return;
    }
    if (!(kindStr == "Food" || kindStr == "Clothing" || kindStr == "Book"))
    {
        QMessageBox::warning(this, "错误", "商品种类只能是Food, Clothing或Book");
        return;
    }
    if (originalPrice <= 0 || discount <= 0 || discount > 1)
    {
        QMessageBox::warning(this, "错误", "原价、折扣必须为有效值（折扣在(0,1]区间）！");
        return;
    }

    // 确保 currentCommodity 被正确初始化
    if (currentCommodity)
    {
        delete currentCommodity;
        currentCommodity = nullptr;
    }

    currentCommodity = new Commodity(0, name, kindStr, description, originalPrice, surplus, fromMerchant, discount, uniDiscount);
    if (!currentCommodity)
    {
        QMessageBox::warning(this, "错误", "无法创建商品对象！");
        return;
    }

    qDebug() << "Creating new commodity with data:" << name << kindStr << fromMerchant;

    // 调用新商品方法
    currentCommodity->newCommodity(name, kindStr, description, originalPrice, surplus, fromMerchant, discount, uniDiscount);

    // 禁用按钮防止重复点击
    rsb_b_new->setEnabled(false);

    // 刷新商品列表
    refreshMerchantCommodityListViews();
}
void MainWindow::handleCommodityNewResponse(bool success, const QString &message)
{
    rsb_b_new->setEnabled(true);
    if (success)
    {
        QMessageBox::information(this, "商品添加", "商品信息添加成功！\n" + message);
        qDebug() << "Commodity update successful:" << message;
        refreshMerchantCommodityListViews();
    }
    else
    {
        QMessageBox::warning(this, "商品添加", "商品信息添加失败！\n" + message);
        qDebug() << "Commodity update failed:" << message;
    }
}

void MainWindow::onClearGoodsButtonClicked()
{
    // 清空右侧表格
    for (int row = 0; row < 8; ++row)
    {
        if (row == 2)
        { // 来源商家行设置为当前商家
            QTableWidgetItem *item = new QTableWidgetItem(currentUser ? currentUser->getName() : "");
            item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            // 确保来源商家行不可编辑
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            r_commodity_table_page3->setItem(row, 1, item);
        }
        else if (row == 6)
        { // 来源商家行设置为当前商家
            QTableWidgetItem *item = new QTableWidgetItem("");
            item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            // 确保来源商家行不可编辑
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            r_commodity_table_page3->setItem(row, 1, item);
        }
        else
        {
            QTableWidgetItem *item = new QTableWidgetItem("");
            item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            // 确保其他行可编辑（在商家模式下）
            if (isMerchant)
            {
                item->setFlags(item->flags() | Qt::ItemIsEditable);
            }
            else
            {
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            }
            r_commodity_table_page3->setItem(row, 1, item);
        }
    }

    // 清理当前商品对象
    delete currentCommodity;
    currentCommodity = nullptr;
    tip->setText("提示：已清空商品信息");
}

void MainWindow::onUniDiscoutButtonClicked()
{
    if (!isMerchant || !currentUser)
    {
        QMessageBox::warning(this, "错误", "仅限商家操作，且需登录！");
        return;
    }

    QString discountStr = uniDiscoutEdit->text();
    QString type = uniDiscoutCombo->currentText();
    QString sourceStoreName = currentUser->getName();

    bool ok;
    float discount = discountStr.toFloat(&ok);
    if (!ok || discount <= 0 || discount > 1)
    {
        QMessageBox::warning(this, "错误", "请输入有效的折扣值（0,1]！");
        return;
    }

    Commodity::setUniDiscount(discount, sourceStoreName, type);
    uniDiscoutButton->setEnabled(false);
}
void MainWindow::handleSetUniDiscountResponse(bool success, const QString &message, float uniDiscount, const QString &sourceStoreName, const QString &type)
{
    uniDiscoutButton->setEnabled(true);

    if (success)
    {
        QMessageBox::information(this, "设置统一折扣", "统一折扣设置成功！\n" + message);
        qDebug() << "Set uniDiscount successful:" << message;

        refreshMerchantCommodityListViews();

        // 更新当前选中的商品信息（如果有）
        if (currentCommodity && currentCommodity->getSourceStoreName() == sourceStoreName && currentCommodity->getType() == type)
        {
            r_commodity_table_page3->setItem(6, 1, new QTableWidgetItem(QString::number(uniDiscount)));
        }
    }
    else
    {
        QMessageBox::warning(this, "设置统一折扣", "统一折扣设置失败！\n" + message);
        qDebug() << "Set uniDiscount failed:" << message;
    }
}

void MainWindow::onDeleteGoodsButtonClicked()
{
    // 检查是否选择了商品
    if (!currentCommodity)
    {
        QMessageBox::warning(this, "错误", "请先选择要删除的商品！");
        return;
    }

    // 检查是否是当前商家的商品
    if (!currentUser || currentCommodity->getSourceStoreName() != currentUser->getName())
    {
        QMessageBox::warning(this, "错误", "您只能删除自己店铺的商品！");
        return;
    }

    // 确认删除操作
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "确认删除",
        QString("确定要删除商品 \"%1\" 吗？\n\n注意：删除后无法恢复，且会清空所有用户购物车中的该商品。")
            .arg(currentCommodity->getName()),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No // 默认选择"否"
    );

    if (reply != QMessageBox::Yes)
    {
        return;
    }

    // 禁用删除按钮防止重复点击
    rsb_b_delete->setEnabled(false);

    // 发送删除请求
    currentCommodity->deleteCommodityFromServer();
}
void MainWindow::handleDeleteCommodityResponse(bool success, const QString &message, unsigned int deletedCommodityId)
{
    // 重新启用删除按钮
    rsb_b_delete->setEnabled(true);

    if (success)
    {
        qDebug() << "MainWindow: Commodity deleted successfully. ID:" << deletedCommodityId;

        // 显示成功消息
        QMessageBox::information(this, "删除成功",
                                 QString("商品删除成功！\n%1").arg(message));

        // 清空当前商品信息
        delete currentCommodity;
        currentCommodity = nullptr;

        // 清空表格显示
        clearMerchantTableInfo();

        // 刷新商品列表
        refreshMerchantCommodityListViews();

        // 如果当前在购物车页面，刷新购物车（删除相关商品）
        if (currentUser)
        {
            ShoppingCart *cart = currentUser->getShoppingCart();
            if (cart)
            {
                // 从购物车中移除已删除的商品
                cart->removeItemByCommodityId(deletedCommodityId);
                updateCartDisplay();
            }
        }

        tip->setText("提示：商品已成功删除");
    }
    else
    {
        qDebug() << "MainWindow: Commodity deletion failed:" << message;
        QMessageBox::warning(this, "删除失败", "商品删除失败：\n" + message);
    }
}

// 购物车页
void MainWindow::updateCartDisplay()
{
    if (!currentUser->getShoppingCart())
    {
        ShoppingCart *tmp = new ShoppingCart(currentUser->getName(), this);
        currentUser->setShoppingCart(tmp);

        connect(currentUser->getShoppingCart(), &ShoppingCart::cartUpdated, this, &MainWindow::onCartUpdated);
    }
    currentUser->getShoppingCart()->loadFromDatabase();
}
void MainWindow::onCartUpdated(bool success, const QString &message, const QList<CartItem> &cartListItems, const QJsonArray &cartItemsJson)
{
    if (!success || cartListItems.isEmpty())
    {
        cartTable->setRowCount(0);
        cartTable->hide();
        clearCartButton->hide();
        createOrder->hide();
        emptyCartLabel->setText(success ? "购物车为空" : message);
        emptyCartLabel->show();
        return;
    }
    else
    {
        cartTable->setRowCount(cartListItems.size());
        cartTable->show();
        clearCartButton->show();
        createOrder->show();
        emptyCartLabel->hide();
    }

    double overallTotalPrice = 0.0;
    for (int row = 0; row < cartListItems.size(); ++row)
    {
        const CartItem &cartListItem = cartListItems[row]; // From QList<CartItem>, primarily for quantity and cart_item_id
        QJsonObject jsonItemDetails;
        if (row < cartItemsJson.size() && cartItemsJson[row].isObject())
        {
            jsonItemDetails = cartItemsJson[row].toObject();
        }

        // Extract details from jsonItemDetails (enriched by server's DAL_ShoppingCartDAO)
        QString name = jsonItemDetails["commodityName"].toString();
        QString type = jsonItemDetails["type"].toString();
        double unitPrice = jsonItemDetails["unitPrice"].toDouble(); // This is the final unit price in yuan
        int stock = jsonItemDetails["stock"].toInt();
        // unsigned int cart_item_id = jsonItemDetails["cartItemId"].toInt(); // Matches cartListItem.id
        // unsigned int commodity_id = jsonItemDetails["commodityId"].toInt(); // Matches cartListItem.commodityId

        int quantityInCart = (cartListItem.quantity > stock) ? stock : cartListItem.quantity;
        double itemTotalPrice = unitPrice * quantityInCart;
        overallTotalPrice += itemTotalPrice;

        cartTable->setItem(row, 0, new QTableWidgetItem(name));
        cartTable->setItem(row, 1, new QTableWidgetItem(type));
        cartTable->setItem(row, 2, new QTableWidgetItem(QString::number(unitPrice, 'f', 2)));
        cartTable->setItem(row, 3, new QTableWidgetItem(QString::number((cartListItem.quantity > stock) ? stock : cartListItem.quantity)));
        cartTable->setItem(row, 4, new QTableWidgetItem(QString::number(itemTotalPrice, 'f', 2)));
        cartTable->setItem(row, 5, new QTableWidgetItem(QString::number(stock))); // 添加库存列
        cartTable->setItem(row, 7, new QTableWidgetItem(QString::number(cartListItem.id)));

        QSpinBox *quantitySpinBox = new QSpinBox(this);
        quantitySpinBox->setMinimum(1);
        quantitySpinBox->setMaximum(stock);
        quantitySpinBox->setValue(cartListItem.quantity);
        cartTable->setCellWidget(row, 3, quantitySpinBox);
        // Ensure item.id (cart_item_id) is correctly captured by the lambda
        unsigned int currentCartItemId = cartListItem.id;
        connect(quantitySpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                this, [this, currentCartItemId](int value)
                { onCartQuantityChanged(currentCartItemId, value); });

        QWidget *operationWidget = new QWidget(this);
        QHBoxLayout *operationLayout = new QHBoxLayout(operationWidget);
        operationLayout->setContentsMargins(0, 0, 0, 0); // Remove margins for a tighter look
        operationLayout->setSpacing(5);                  // Add some spacing between buttons if you add more

        QPushButton *removeButton = new QPushButton("删除", this);
        removeButton->setProperty("cart_item_id", currentCartItemId); // Store cart_item_id if needed directly
        operationLayout->addWidget(removeButton);
        cartTable->setCellWidget(row, 6, operationWidget);
        connect(removeButton, &QPushButton::clicked, this, [this, currentCartItemId]()
                { onRemoveCartItem(currentCartItemId); });
    }

    // Make table cells non-editable (except for widgets like QSpinBox)
    for (int row = 0; row < cartTable->rowCount(); ++row)
    {
        for (int col = 0; col < cartTable->columnCount(); ++col)
        {
            if (cartTable->cellWidget(row, col))
                continue; // Skip columns with widgets
            QTableWidgetItem *tableItem = cartTable->item(row, col);
            if (tableItem)
            {
                tableItem->setFlags(tableItem->flags() & ~Qt::ItemIsEditable);
            }
        }
    }

    // This final check is good
    if (cartListItems.isEmpty())
    {
        cartTable->hide();
        clearCartButton->hide();
        createOrder->hide();
        emptyCartLabel->show();
    }
    else
    {
        cartTable->show();
        clearCartButton->show();
        createOrder->show();
        emptyCartLabel->hide();
    }
}

void MainWindow::onCartQuantityChanged(int id, int quantity)
{
    if (!currentUser)
        return;
    QString errorMessage;
    currentUser->getShoppingCart()->updateItemQuantity(id, quantity, errorMessage);
}
void MainWindow::handleUpdateCartItemQuantityResponse(bool success, const QString &message, const QString &action, const QJsonArray &updatedCartItems)
{
    if (success)
    {
        qDebug() << "MainWindow: Cart item quantity update successful. Action:" << action << ", Message:" << message;
        updateCartDisplay();
    }
    else
    {
        qDebug() << "MainWindow: Cart item quantity update failed. Message:" << message;
        QMessageBox::warning(this, "错误", "购物车操作失败：" + message);

        // 操作失败时，重新加载购物车以确保显示的是最新状态
        updateCartDisplay();
    }
}

void MainWindow::onRemoveCartItem(int id)
{
    if (!currentUser)
        return;

    // 确认删除操作
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "确认删除",
        "确定要删除此商品吗？",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No)
    {
        return;
    }

    // 调用购物车的 removeItem 方法，这会发送网络请求到服务器
    if (!currentUser->getShoppingCart())
    {
        ShoppingCart *tmp = new ShoppingCart(currentUser->getName(), this);
        currentUser->setShoppingCart(tmp);
        connect(currentUser->getShoppingCart(), &ShoppingCart::cartUpdated, this, &MainWindow::onCartUpdated);
    }

    QString errorMessage;
    currentUser->getShoppingCart()->removeItem(id, errorMessage);
}
void MainWindow::handleRemoveCartItemResponse(bool success, const QString &message, const QString &action, const QJsonArray &updatedCartItems)
{
    if (success)
    {
        currentUser->getShoppingCart()->updateFromServerResponse(updatedCartItems);
        qDebug() << "MainWindow: Remove cart item successful. Action:" << action << ", Message:" << message;

        // 如果服务器返回了更新后的购物车数据，使用它来更新UI
        if (!updatedCartItems.isEmpty())
        {
            // 将 QJsonArray 转换为 QList<CartItem> 以保持与现有 onCartUpdated 函数的兼容性
            QList<CartItem> cartItems;
            for (const QJsonValue &val : updatedCartItems)
            {
                if (val.isObject())
                {
                    QJsonObject itemObj = val.toObject();
                    CartItem item;
                    item.id = itemObj["cartItemId"].toInt();
                    item.commodityId = itemObj["commodityId"].toInt();
                    // item.commodityName = itemObj["commodityName"].toString();
                    item.quantity = itemObj["quantity"].toInt();
                    // item.unitPrice = itemObj["unitPrice"].toDouble();
                    cartItems.append(item);
                }
            }

            // 直接调用 onCartUpdated 来更新购物车显示
            onCartUpdated(true, "购物车更新成功", cartItems, updatedCartItems);
        }
        else
        {
            qDebug() << "testaaaaaaaaaaaaaaa";
            // 如果服务器没有返回完整数据，重新加载购物车
            // updateCartDisplay();
            // 直接更新UI显示为空购物车状态
            cartTable->setRowCount(0);
            cartTable->hide();
            clearCartButton->hide();
            createOrder->hide();
            emptyCartLabel->setText("购物车为空");
            emptyCartLabel->show();
        }

        // 显示成功提示
        QMessageBox::information(this, "成功", "商品已从购物车中删除");
    }
    else
    {
        qDebug() << "MainWindow: Remove cart item failed. Message:" << message;
        QMessageBox::warning(this, "错误", "删除购物车商品失败：" + message);

        // 操作失败时，重新加载购物车以确保显示的是最新状态
        updateCartDisplay();
    }
}

void MainWindow::onClearCartClicked()
{
    if (!currentUser)
    {
        QMessageBox::warning(this, "错误", "请先登录！");
        return;
    }

    // 确认清空操作
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "确认清空购物车",
        "确定要清空购物车吗？这将删除所有商品。",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No)
    {
        return;
    }

    // 禁用按钮防止重复点击
    clearCartButton->setEnabled(false);

    // 调用购物车的 clear 方法，这会发送网络请求到服务器
    if (!currentUser->getShoppingCart())
    {
        ShoppingCart *tmp = new ShoppingCart(currentUser->getName(), this);
        currentUser->setShoppingCart(tmp);
        connect(currentUser->getShoppingCart(), &ShoppingCart::cartUpdated, this, &MainWindow::onCartUpdated);
    }

    QString errorMessage;
    currentUser->getShoppingCart()->clear(errorMessage);
}
void MainWindow::handleClearCartResponse(bool success, const QString &message, const QString &action, const QJsonArray &updatedCartItems)
{
    // 重新启用清空购物车按钮
    clearCartButton->setEnabled(true);

    if (success)
    {
        qDebug() << "MainWindow: Clear cart successful. Action:" << action << ", Message:" << message;

        // 直接更新UI显示为空购物车状态
        cartTable->setRowCount(0);
        cartTable->hide();
        clearCartButton->hide();
        createOrder->hide();
        emptyCartLabel->setText("购物车为空");
        emptyCartLabel->show();

        // 显示成功提示
        QMessageBox::information(this, "成功", "购物车已清空");

        qDebug() << "MainWindow: Cart cleared successfully, UI updated to empty state";
    }
    else
    {
        qDebug() << "MainWindow: Clear cart failed. Message:" << message;
        QMessageBox::warning(this, "错误", "清空购物车失败：" + message);

        // 操作失败时，重新加载购物车以确保显示的是最新状态
        updateCartDisplay();
    }
}

void MainWindow::onCreateOrderClicked()
{
    if (!currentUser)
    {
        QMessageBox::warning(this, "错误", "请先登录！");
        return;
    }

    ShoppingCart *cart = currentUser->getShoppingCart();
    if (!cart || cart->getItems().isEmpty())
    {
        QMessageBox::warning(this, "错误", "购物车为空，无法生成订单！");
        return;
    }

    // 禁用按钮防止重复点击
    createOrder->setEnabled(false);

    // 从购物车获取商品信息，并获取UI显示的现价
    QList<CartItem> cartItems = cart->getItems();
    QList<QVariantMap> orderItemsForValidation;

    // 确保表格行数与购物车项目数一致
    if (cartTable->rowCount() != cartItems.size())
    {
        createOrder->setEnabled(true);
        QMessageBox::warning(this, "错误", "购物车数据不一致，请刷新后重试！");
        updateCartDisplay();
        return;
    }

    for (int i = 0; i < cartItems.size(); ++i)
    {
        const CartItem &cartItem = cartItems[i];

        // 从表格获取UI显示的现价
        QTableWidgetItem *nameItem = cartTable->item(i, 0);
        QTableWidgetItem *finalPriceItem = cartTable->item(i, 2); // 现价列
        QTableWidgetItem *quantityItem = cartTable->item(i, 3);

        if (!nameItem || !finalPriceItem || !quantityItem)
        {
            createOrder->setEnabled(true);
            QMessageBox::warning(this, "错误", "表格数据不完整，请刷新后重试！");
            updateCartDisplay();
            return;
        }

        double uiCurrentPrice = finalPriceItem->text().toDouble(); // UI显示的现价
        unsigned int quantity = quantityItem->text().toUInt();

        QVariantMap validationItem;
        validationItem["commodityId"] = cartItem.commodityId;
        validationItem["commodityName"] = nameItem->text();
        validationItem["quantity"] = quantity;
        validationItem["uiCurrentPrice"] = uiCurrentPrice; // 只传递UI显示的现价

        orderItemsForValidation.append(validationItem);
    }

    // 发送验证请求到服务器
    QJsonObject request;
    request["mtype"] = "validate_order_items";
    request["username"] = currentUser->getName();

    QJsonArray itemsArray;
    for (const QVariantMap &item : orderItemsForValidation)
    {
        QJsonObject itemObj;
        itemObj["commodityId"] = static_cast<qint64>(item["commodityId"].toUInt());
        itemObj["quantity"] = static_cast<qint64>(item["quantity"].toUInt());
        itemObj["uiCurrentPrice"] = item["uiCurrentPrice"].toDouble();
        itemsArray.append(itemObj);
    }
    request["items"] = itemsArray;

    qDebug() << "MainWindow: Sending order validation request with UI current price data";
    HandleMessage::sendJsonData(&tcpSocket, request);
}
void MainWindow::handleValidateOrderItemsResponse(const QString &status, const QString &message, const QJsonArray &validatedItems, unsigned long long totalPrice, const QString &changes)
{
    // 重新启用创建订单按钮
    createOrder->setEnabled(true);

    if (status == "error")
    {
        QMessageBox::warning(this, "错误", "创建订单失败：\n" + message);
        updateCartDisplay(); // 刷新购物车显示
        return;
    }

    if (status == "price_changed")
    {
        // 显示价格变动确认对话框
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "价格变动确认",
            QString("检测到以下变动：\n\n%1\n\n新的总价：%2 元\n\n是否继续创建订单？")
                .arg(changes)
                .arg(totalPrice / 100.0, 0, 'f', 2),
            QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No)
        {
            updateCartDisplay(); // 刷新购物车显示
            return;
        }
    }

    // 验证通过或用户确认继续，构建订单项列表
    QList<QVariantMap> orderItems;
    for (const QJsonValue &itemValue : validatedItems)
    {
        if (!itemValue.isObject())
            continue;

        QJsonObject itemObj = itemValue.toObject();
        QVariantMap orderItem;
        orderItem["commodityId"] = static_cast<unsigned int>(itemObj["commodityId"].toInteger());
        orderItem["commodityName"] = itemObj["commodityName"].toString();
        orderItem["quantity"] = static_cast<unsigned int>(itemObj["quantity"].toInteger());
        orderItem["originalPrice"] = itemObj["originalPrice"].toDouble();
        orderItem["discount"] = itemObj["discount"].toDouble();
        orderItem["uniDiscount"] = itemObj["uniDiscount"].toDouble();
        orderItems.append(orderItem);
    }

    // 发送创建订单请求
    Order::createOrderFromServer(currentUser->getName(), orderItems, totalPrice);

    // 再次禁用按钮
    createOrder->setEnabled(false);
}
void MainWindow::handleCreateOrderResponse(bool success, const QString &message, unsigned long long newBalance, unsigned long long totalPrice)
{
    // 重新启用创建订单按钮
    createOrder->setEnabled(true);

    if (success)
    {
        // 显示成功消息（订单创建成功，但未支付）
        QMessageBox::information(this, "成功",
                                 QString("订单创建成功！\n订单金额：%1 元\n状态：未支付\n\n您可以在 我的订单 中查看并支付订单。")
                                     .arg(static_cast<double>(totalPrice) / 100.0, 0, 'f', 2));

        // 刷新相关显示
        updateCartDisplay();  // 刷新购物车（应该显示为空）
        updateOrderDisplay(); // 刷新订单列表

        qDebug() << "MainWindow: Order created successfully (unpaid). Order amount:" << totalPrice;
    }
    else
    {
        QMessageBox::warning(this, "错误", "创建订单失败：\n" + message);
        qDebug() << "MainWindow: Order creation failed:" << message;

        // 刷新购物车显示以确保数据同步
        updateCartDisplay();
    }
}

// 订单页
void MainWindow::updateOrderDisplay()
{
    if (!currentUser)
    {
        orderListView->setModel(nullptr);
        for (int row = 0; row < 5; ++row)
        {
            r_order_table->setItem(row, 1, new QTableWidgetItem(""));
        }
        r_order_items_table->setRowCount(0);
        r_order_total_label->setText("总价: 0.00");
        r_order_pay_button->hide();
        return;
    }

    // 使用Order类的静态方法发送请求到服务器
    Order::fetchOrdersFromServer(currentUser->getName());
}
void MainWindow::handleFetchOrdersResponse(bool success, const QString &message, const QJsonArray &orders)
{
    if (!success)
    {
        qDebug() << "MainWindow: Fetch orders failed. Message:" << message;
        QMessageBox::warning(this, "错误", "获取订单失败：" + message);

        // 清空订单显示
        orderListView->setModel(nullptr);
        for (int row = 0; row < 5; ++row)
        {
            r_order_table->setItem(row, 1, new QTableWidgetItem(""));
        }
        r_order_items_table->setRowCount(0);
        r_order_total_label->setText("总价: 0.00");
        r_order_pay_button->hide();
        return;
    }

    qDebug() << "MainWindow: Fetch orders successful. Orders count:" << orders.size();

    // 清空用户内存中的订单数据
    currentUser->clearOrders();

    // 创建新的订单列表模型
    QStandardItemModel *model = new QStandardItemModel(this);

    for (const QJsonValue &orderValue : orders)
    {
        if (!orderValue.isObject())
            continue;

        QJsonObject orderObj = orderValue.toObject();
        unsigned int orderId = static_cast<unsigned int>(orderObj["orderId"].toInteger());
        QString userName = orderObj["userName"].toString();
        QString state = orderObj["state"].toString();
        QString orderTime = orderObj["orderTime"].toString();
        unsigned long long totalPrice = static_cast<unsigned long long>(orderObj["totalPrice"].toInteger());

        // 解析订单项目
        QList<QVariantMap> items;
        QStringList itemNames;
        QJsonArray itemsArray = orderObj["items"].toArray();
        for (const QJsonValue &itemValue : itemsArray)
        {
            if (!itemValue.isObject())
                continue;

            QJsonObject itemObj = itemValue.toObject();
            QVariantMap item;
            item["commodityId"] = static_cast<unsigned int>(itemObj["commodityId"].toInteger());
            item["commodityName"] = itemObj["commodityName"].toString(); // 直接从订单项获取商品名称
            item["quantity"] = static_cast<unsigned int>(itemObj["quantity"].toInteger());
            item["originalPrice"] = itemObj["originalPrice"].toDouble();
            item["discount"] = itemObj["discount"].toDouble();
            item["uniDiscount"] = itemObj["uniDiscount"].toDouble();
            items.append(item);

            itemNames << itemObj["commodityName"].toString();
        }

        // 创建订单对象并存储到用户内存中
        Order *order = new Order(userName, items, totalPrice, state);
        order->setOrderId(orderId);
        order->setOrderTime(orderTime);
        currentUser->addOrder(order);

        // 为UI显示创建列表项
        QString itemsText = itemNames.join(", ");
        QString displayText = QString("订单ID: %1\n状态: %2\n总价: %3\n商品: %4")
                                  .arg(orderId)
                                  .arg(state)
                                  .arg(totalPrice / 100.0, 0, 'f', 2)
                                  .arg(itemsText);

        QStandardItem *item = new QStandardItem(displayText);
        item->setData(orderId, Qt::UserRole); // 只存储订单ID，实际数据从内存中获取
        model->appendRow(item);
    }

    // 设置模型到视图
    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->setFilterRole(Qt::DisplayRole);
    orderListView->setModel(proxyModel);

    // 清空右侧订单信息和商品表格
    for (int row = 0; row < 5; ++row)
    {
        r_order_table->setItem(row, 1, new QTableWidgetItem(""));
    }
    r_order_items_table->setRowCount(0);
    r_order_total_label->setText("总价: 0.00");
    r_order_pay_button->hide();
}
void MainWindow::onOrderSelected(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    // 从模型中获取订单ID
    unsigned int orderId = index.data(Qt::UserRole).toUInt();

    // 从用户内存中获取订单数据
    Order *selectedOrder = currentUser->getOrderById(orderId);
    if (!selectedOrder)
    {
        QMessageBox::warning(this, "错误", "无法找到选中的订单！");
        return;
    }

    // 更新基本信息
    r_order_table->setItem(0, 1, new QTableWidgetItem(QString::number(selectedOrder->getOrderId())));
    r_order_table->setItem(1, 1, new QTableWidgetItem(selectedOrder->getUserName()));
    r_order_table->setItem(2, 1, new QTableWidgetItem(QString::number(selectedOrder->getTotalPrice() / 100.0, 'f', 2)));
    r_order_table->setItem(3, 1, new QTableWidgetItem(selectedOrder->getState()));
    r_order_table->setItem(4, 1, new QTableWidgetItem(selectedOrder->getOrderTime()));

    // 更新商品表格
    QList<QVariantMap> items = selectedOrder->getItems();
    r_order_items_table->setRowCount(items.size());

    int row = 0;
    for (const QVariantMap &item : items)
    {
        QString name = item["commodityName"].toString(); // 直接从订单项获取商品名称，无需查询数据库
        unsigned int quantity = item["quantity"].toUInt();
        double originalPrice = item["originalPrice"].toDouble();
        double discount = item["discount"].toDouble();
        double uniDiscount = item["uniDiscount"].toDouble();
        double finalPrice = originalPrice * discount * uniDiscount;

        r_order_items_table->setItem(row, 0, new QTableWidgetItem(name));
        r_order_items_table->setItem(row, 1, new QTableWidgetItem(QString::number(originalPrice, 'f', 2)));
        r_order_items_table->setItem(row, 2, new QTableWidgetItem(QString::number(finalPrice, 'f', 2)));
        r_order_items_table->setItem(row, 3, new QTableWidgetItem(QString::number(quantity)));
        row++;
    }

    // 设置商品表格项为只读
    for (int r = 0; r < r_order_items_table->rowCount(); ++r)
    {
        for (int c = 0; c < r_order_items_table->columnCount(); ++c)
        {
            QTableWidgetItem *tableItem = r_order_items_table->item(r, c);
            if (tableItem)
            {
                tableItem->setFlags(tableItem->flags() & ~Qt::ItemIsEditable);
            }
        }
    }

    // 更新总价和支付按钮
    r_order_total_label->setText(QString("总价: %1").arg(selectedOrder->getTotalPrice() / 100.0, 0, 'f', 2));
    if (selectedOrder->getState() == "unPaid")
    {
        r_order_pay_button->show();
    }
    else
    {
        r_order_pay_button->hide();
    }
}
void MainWindow::updateOrderDisplayFromMemory()
{
    if (!currentUser)
    {
        orderListView->setModel(nullptr);
        for (int row = 0; row < 5; ++row)
        {
            r_order_table->setItem(row, 1, new QTableWidgetItem(""));
        }
        r_order_items_table->setRowCount(0);
        r_order_total_label->setText("总价: 0.00");
        r_order_pay_button->hide();
        return;
    }

    // 创建新的订单列表模型，使用内存中的数据
    QStandardItemModel *model = new QStandardItemModel(this);
    QList<Order *> orders = currentUser->getOrders();

    for (Order *order : orders)
    {
        // 获取订单商品名称
        QStringList itemNames;
        for (const QVariantMap &item : order->getItems())
        {
            itemNames << item["commodityName"].toString();
        }

        QString itemsText = itemNames.join(", ");
        QString displayText = QString("订单ID: %1\n状态: %2\n总价: %3\n商品: %4")
                                  .arg(order->getOrderId())
                                  .arg(order->getState())
                                  .arg(order->getTotalPrice() / 100.0, 0, 'f', 2)
                                  .arg(itemsText);

        QStandardItem *item = new QStandardItem(displayText);
        item->setData(order->getOrderId(), Qt::UserRole);
        model->appendRow(item);
    }

    // 设置模型到视图
    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(model);
    proxyModel->setFilterRole(Qt::DisplayRole);
    orderListView->setModel(proxyModel);

    // 清空右侧订单信息
    for (int row = 0; row < 5; ++row)
    {
        r_order_table->setItem(row, 1, new QTableWidgetItem(""));
    }
    r_order_items_table->setRowCount(0);
    r_order_total_label->setText("总价: 0.00");
    r_order_pay_button->hide();
}

void MainWindow::onPayOrderClicked()
{
    if (!currentUser)
    {
        QMessageBox::warning(this, "错误", "请先登录！");
        return;
    }

    // 获取当前选中的订单
    QModelIndexList selectedIndexes = orderListView->selectionModel()->selectedIndexes();
    if (selectedIndexes.isEmpty())
    {
        QMessageBox::warning(this, "错误", "请先选择要支付的订单！");
        return;
    }

    unsigned int orderId = selectedIndexes.first().data(Qt::UserRole).toUInt();
    Order *selectedOrder = currentUser->getOrderById(orderId);
    if (!selectedOrder)
    {
        QMessageBox::warning(this, "错误", "无法找到选中的订单！");
        return;
    }

    // 检查订单状态
    if (selectedOrder->getState() != "unPaid")
    {
        QMessageBox::warning(this, "错误", QString("订单状态为 %1，无法支付！").arg(selectedOrder->getState()));
        return;
    }

    // 检查用户余额是否足够
    unsigned long long orderTotal = selectedOrder->getTotalPrice();
    if (currentUser->getBalance() < orderTotal)
    {
        QMessageBox::warning(this, "余额不足",
                             QString("订单金额：%1 元\n当前余额：%2 元\n请先充值！")
                                 .arg(orderTotal / 100.0, 0, 'f', 2)
                                 .arg(currentUser->getBalance() / 100.0, 0, 'f', 2));
        return;
    }

    // 确认支付
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "确认支付",
        QString("确定要支付订单 %1 吗？\n订单金额：%2 元\n支付后余额：%3 元")
            .arg(orderId)
            .arg(orderTotal / 100.0, 0, 'f', 2)
            .arg((currentUser->getBalance() - orderTotal) / 100.0, 0, 'f', 2),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No)
    {
        return;
    }

    // 禁用支付按钮防止重复点击
    r_order_pay_button->setEnabled(false);

    // 发送支付请求到服务器
    Order::payOrderFromServer(currentUser->getName(), orderId);
}
void MainWindow::handlePayOrderResponse(bool success, const QString &message, unsigned int orderId, unsigned long long paidAmount, unsigned long long newBalance)
{
    // 重新启用支付按钮
    r_order_pay_button->setEnabled(true);

    if (success)
    {
        qDebug() << "MainWindow: Pay order successful. Order ID:" << orderId
                 << ", Paid Amount:" << paidAmount << ", New Balance:" << newBalance;

        // 更新用户余额
        currentUser->setBalanceWithout100(newBalance);
        l_profile_balance->setText(QString("余额：%1").arg(static_cast<double>(newBalance) / 100.0, 0, 'f', 2));

        // 更新内存中的订单状态
        Order *paidOrder = currentUser->getOrderById(orderId);
        if (paidOrder)
        {
            paidOrder->setState("paid");
        }

        // 刷新订单显示
        updateOrderDisplayFromMemory();

        // 显示成功消息
        QMessageBox::information(this, "支付成功",
                                 QString("订单 %1 支付成功！\n支付金额：%2 元\n剩余余额：%3 元")
                                     .arg(orderId)
                                     .arg(paidAmount / 100.0, 0, 'f', 2)
                                     .arg(newBalance / 100.0, 0, 'f', 2));

        // 隐藏支付按钮（因为订单已支付）
        r_order_pay_button->hide();
    }
    else
    {
        qDebug() << "MainWindow: Pay order failed. Message:" << message;
        QMessageBox::warning(this, "支付失败", "订单支付失败：\n" + message);
    }
}

void MainWindow::onDeleteOrderClicked()
{
    if (!currentUser)
    {
        QMessageBox::warning(this, "错误", "请先登录！");
        return;
    }

    // 获取当前选中的订单
    QModelIndexList selectedIndexes = orderListView->selectionModel()->selectedIndexes();
    if (selectedIndexes.isEmpty())
    {
        QMessageBox::warning(this, "错误", "请先选择要删除的订单！");
        return;
    }

    unsigned int orderId = selectedIndexes.first().data(Qt::UserRole).toUInt();
    Order *selectedOrder = currentUser->getOrderById(orderId);
    if (!selectedOrder)
    {
        QMessageBox::warning(this, "错误", "无法找到选中的订单！");
        return;
    }

    // 根据订单状态生成不同的确认信息
    QString confirmMessage;
    if (selectedOrder->getState() == "unPaid")
    {
        confirmMessage = QString("确定要删除订单 %1 吗？\n删除后商品库存将会恢复。").arg(orderId);
    }
    else
    {
        confirmMessage = QString("确定要删除订单 %1 吗？\n订单状态：%2\n（已支付订单删除后不会恢复库存）")
                             .arg(orderId)
                             .arg(selectedOrder->getState());
    }

    // 确认删除操作
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "确认删除订单",
        confirmMessage,
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No)
    {
        return;
    }

    // 发送删除订单请求到服务器
    Order::deleteOrderFromServer(currentUser->getName(), orderId);
}
void MainWindow::handleDeleteOrderResponse(bool success, const QString &message, unsigned int deletedOrderId)
{
    if (success)
    {
        qDebug() << "MainWindow: Delete order successful. Deleted Order ID:" << deletedOrderId << ", Message:" << message;

        // 从用户内存中移除订单
        currentUser->removeOrder(deletedOrderId);

        // 直接更新UI，不重新从服务器获取数据
        updateOrderDisplayFromMemory();

        // 显示成功提示
        QMessageBox::information(this, "成功", message);
    }
    else
    {
        qDebug() << "MainWindow: Delete order failed. Message:" << message;
        QMessageBox::warning(this, "错误", "删除订单失败：" + message);
    }
}
