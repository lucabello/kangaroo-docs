QT += widgets
QT += network
requires(qtConfig(filedialog))
qtHaveModule(printsupport): QT += printsupport

TEMPLATE        = app
TARGET          = kangaroo-docs

HEADERS         = ClientSocket.h \
    LSEQAllocator.h \
    ../common/Message.h \
    ../common/MessageQueue.h \
    ../common/SharedEditor.h \
    ../common/Symbol.h \
    TextEdit.h

SOURCES         = ClientSocket.cpp \
    LSEQAllocator.cpp \
    ../common/Message.cpp \
    main_client.cpp \
    ../common/MessageQueue.cpp \
    ../common/SharedEditor.cpp \
    ../common/Symbol.cpp \
    TextEdit.cpp \

RESOURCES += kangaroodocs.qrc
build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

EXAMPLE_FILES = kangaroodocs.qdoc

# install
target.path = D:\Programmazione\kangaroo-docs\qt
INSTALLS += target
