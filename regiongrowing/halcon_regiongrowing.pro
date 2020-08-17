#-------------------------------------------------
#
# Project created by QtCreator 2020-08-05T18:18:35
#
#-------------------------------------------------

QT       += core gui
QT       += sql
QT       +=charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = halcon_regiongrowing
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
macx {
  QMAKE_CXXFLAGS += -F/Library/Frameworks
  QMAKE_LFLAGS   += -F/Library/Frameworks
  LIBS           += -framework HALCONCpp
}
else {
  #defines
  win32:DEFINES += WIN32

  #includes
  INCLUDEPATH   += "$$(HALCONROOT)/include"
  INCLUDEPATH   += "$$(HALCONROOT)/include/halconcpp"

  #libs
  QMAKE_LIBDIR  += "$$(HALCONROOT)/lib/$$(HALCONARCH)"
  unix:LIBS     += -lhalconcpp -lhalcon -lXext -lX11 -ldl -lpthread
  win32:LIBS    += "$$(HALCONROOT)/lib/$$(HALCONARCH)/halconcpp.lib" \
                   "$$(HALCONROOT)/lib/$$(HALCONARCH)/halcon.lib"
}

SOURCES += main.cpp\
        mainwindow.cpp \
    count.cpp

HEADERS  += mainwindow.h \
    count.h

FORMS    += mainwindow.ui \
    count.ui
