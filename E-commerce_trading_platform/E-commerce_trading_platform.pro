QT       += core gui
QT       += sql
QT       += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    BLL_commodity.cpp \
    BLL_order.cpp \
    BLL_shoppingcart.cpp \
    BLL_user.cpp \
    NL_HandleMessage.cpp \
    UIL_changepassword.cpp \
    UIL_loginwindow.cpp \
    UIL_mainwindow.cpp \
    main.cpp

HEADERS += \
    All.h \
    BLL_commodity.h \
    BLL_order.h \
    BLL_shoppingcart.h \
    BLL_user.h \
    NL_HandleMessage.h \
    UIL_changepassword.h \
    UIL_commoditydelegate.h \
    UIL_loginwindow.h \
    UIL_mainwindow.h

FORMS += \
    UIL_mainwindow.ui

RESOURCES += \
    all.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
