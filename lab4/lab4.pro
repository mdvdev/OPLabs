QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/CsvParser.cpp \
    src/List.cpp \
    src/Model.cpp \
    src/Presenter.cpp \
    src/String.cpp \
    src/main.cpp \
    src/MainWindow.cpp

HEADERS += \
    include/AppData.h \
    include/CsvParser.h \
    include/Error.h \
    include/List.h \
    include/MainWindow.h \
    include/Model.h \
    include/MyString.h \
    include/Operation.h \
    include/Point.h \
    include/Presenter.h

FORMS += \
    MainWindow.ui

INCLUDEPATH += $$PWD/include

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
