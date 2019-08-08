#include <QCoreApplication>
#include "KangarooServer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    KangarooServer *server = new KangarooServer("127.0.0.1", 1501);

    return a.exec();
}
