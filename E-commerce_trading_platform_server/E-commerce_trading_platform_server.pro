QT = core
QT       +=sql
QT       +=network

CONFIG += c++17 cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        BLL_ShoppingCart.cpp \
        BLL_commodity.cpp \
        BLL_order.cpp \
        BLL_user.cpp \
        DAL_CommodityDAO.cpp \
        DAL_OrderDAO.cpp \
        DAL_ShoppingCartDAO.cpp \
        DAL_UserDAO.cpp \
        NL_ConnectionsHandler.cpp \
        NL_ConnectionsManager.cpp \
        NL_MessageSent.cpp \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    All.h \
    BLL_ShoppingCart.h \
    BLL_commodity.h \
    BLL_order.h \
    BLL_user.h \
    DAL_CommodityDAO.h \
    DAL_OrderDAO.h \
    DAL_ShoppingCartDAO.h \
    DAL_UserDAO.h \
    NL_ConnectionsHandler.h \
    NL_ConnectionsManager.h \
    NL_MessageSent.h
