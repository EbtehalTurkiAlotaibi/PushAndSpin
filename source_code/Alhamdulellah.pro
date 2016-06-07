#-------------------------------------------------
#
# Project created by QtCreator 2016-02-14T14:07:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Alhamdulellah
TEMPLATE = app


SOURCES +=\
    Graph.cpp \
    Logger.cpp \
    PASDemo.cpp \
    bibox.cpp \
    Map.cpp \
    OccupancyGridMap.cpp \
    ScenarioLoader.cpp \
    GraphRec.cpp \
    TestInput.cpp \
    TestPublicBenchmark.cpp \
    Statistics.cpp \
    TestPushAndSpin.cpp \
    TestPushAndSwap.cpp \
    mainwindow.cpp \
    testingcriticalproblems.cpp \
    PushAndSpin.cpp \
    PushAndSwap.cpp \
    testuserinput.cpp \
    testingpublicbenchmark.cpp \
    testingbiconnected.cpp \
    testinguibibox.cpp \
    testingrandombiconnected.cpp \
    testingrandomtwcl.cpp \
    TestTreeWithCycleLeaves.cpp \
    testingrandomgrid.cpp \
    TestingGrid.cpp \
    testrandomgrid.cpp

HEADERS  += \
    Agent.h \
    ElementaryCyclesSearch.h \
    Graph.h \
    Instance.h \
    Logger.h \
    version.h \
    Map.h \
    OccupancyGridMap.h \
    ScenarioLoader.h \
    mainwindow.h \
    testingcriticalproblems.h \
    PushAndSpin.h \
    PushAndSwap.h \
    testuserinput.h \
    testingpublicbenchmark.h \
    DataStructures.h \
    testingbiconnected.h \
    testinguibibox.h \
    testingrandombiconnected.h \
    testingrandomtwcl.h \
    testingrandomgrid.h \
    testrandomgrid.h

FORMS    += \
    mainwindow.ui \
    testingcriticalproblems.ui \
    testuserinput.ui \
    testingpublicbenchmark.ui \
    testingbiconnected.ui \
    testinguibibox.ui \
    testingrandombiconnected.ui \
    testingrandomtwcl.ui \
    testrandomgrid.ui

INCLUDEPATH += /home/ubuntu/boost
DEPENDPATH += /home/ubuntu/boost


LIBS +=-L"/home/ubuntu/boost/lib/"

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../boost/lib/release/ -lboost_regex
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../boost/lib/debug/ -lboost_regex
else:unix: LIBS += -L$$PWD/../boost/lib/ -lboost_regex

INCLUDEPATH += $$PWD/../boost/include
DEPENDPATH += $$PWD/../boost/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../boost/lib/release/libboost_regex.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../boost/lib/debug/libboost_regex.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../boost/lib/release/boost_regex.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../boost/lib/debug/boost_regex.lib
else:unix: PRE_TARGETDEPS += $$PWD/../boost/lib/libboost_regex.a

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../boost/lib/release/ -lboost_graph
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../boost/lib/debug/ -lboost_graph
else:unix: LIBS += -L$$PWD/../boost/lib/ -lboost_graph

INCLUDEPATH += $$PWD/../boost/include
DEPENDPATH += $$PWD/../boost/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../boost/lib/release/libboost_graph.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../boost/lib/debug/libboost_graph.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../boost/lib/release/boost_graph.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../boost/lib/debug/boost_graph.lib
else:unix: PRE_TARGETDEPS += $$PWD/../boost/lib/libboost_graph.a

OTHER_FILES += \
    GraphRec \
    version \
    main.cpp.T11311 \
    Untitled Document \
    parser \
    Alhamdulellah.pro.user \
    makefile \
    objects.mk \
    sources.mk \
    subdir.mk \
    Untitled Document \
    main.o \
    ScenarioLoader.o \
    main.d \
    Makefile \
    ScenarioLoader.d \
    Untitled Document
