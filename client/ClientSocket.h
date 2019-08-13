#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <QObject>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QAbstractSocket>
#include <QDebug>
#include "../common/Symbol.h"
#include "../common/Message.h"

/**
 * This class adds a layer to separate the transmission of data through
 * the socket from the application logic.
 * To read something from the socket, the readyRead slot is used.
 * To write something to the socket, the writeData slot is used.
 * The other slots provide debug prints to check for correctness.
 */
class ClientSocket : public QObject
{
    Q_OBJECT
public:
    //explicit ClientSocket(QObject *parent = nullptr);
    /**
     * Constructor for ClientSocket having nullptr as parent QObject.
     * Sets up a socket with the IPv4 address and port of the server.
     *
     * @param address IPv4 address of the server
     * @param port port of the server
     */
    explicit ClientSocket(std::string address, unsigned short port);

    /**
     * Connects to the server specified in the constructor.
     * Creates the instance of a QTcpSocket and connects public slots
     * to proper signals.
     */
    void doConnect();

signals:
    /**
     * This signal is emitted when a Message is read from the socket.
     */
    void signalMessage(Message);

public slots:
    /**
     * Not used by now
     */
    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    /**
     * Slots called to send/receive messages on the socket.
     * Note that readyRead signal will not be received while inside
     * the readyRead slot, so the function handles it by reading all
     * messages that arrived completely.
     */
    void readMessage();
    void writeMessage(Message message);


private:
    QTcpSocket *socket;
    std::string address;
    unsigned short port;
};

#endif // CLIENTSOCKET_H
