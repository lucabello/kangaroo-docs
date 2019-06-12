QT += widgets
requires(qtConfig(filedialog))
qtHaveModule(printsupport): QT += printsupport

TEMPLATE        = app
TARGET          = kangaroo-docs

HEADERS         = textedit.h \
    mytcpsocket.h \
    qsharededitor.h \
    ../*.h

SOURCES         = textedit.cpp \
                  main.cpp \
                  mytcpsocket.cpp \
                  qsharededitor.cpp \
                  ../*.cpp

QT += network

RESOURCES += textedit.qrc
build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

EXAMPLE_FILES = textedit.qdoc

# install
target.path = D:\Programmazione\kangaroo-docs\qt
INSTALLS += target
