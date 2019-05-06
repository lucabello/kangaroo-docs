//
// Created by lucab on 06/05/2019.
//

#ifndef KANGAROO_DOCS_SHAREDEDITOR_H
#define KANGAROO_DOCS_SHAREDEDITOR_H

#include <string>
#include "NetworkServer.h"
#include "Symbol.h"
#include "Message.h"

class NetworkServer; //forward declaration
/**
 * Class modeling an instance of a shared editor.
 */
class SharedEditor {
    NetworkServer& _server;
    int _siteId; //unique identifier
    std::vector<Symbol> _symbols;
    int _counter; //incremented at every insertion
public:
    SharedEditor(NetworkServer& s);
    /**
     * Insert a char locally and send a Message to the server.
     *
     * @param index
     * @param value
     */
    void localInsert(int index, char value);
    /**
     * Erase a char locally and send a Message to the server
     *
     * @param index
     */
    void localErase(int index);
    /**
     * Process the content of a message and execute its actions.
     *
     * @param m
     */
    void process(const Message &m);
    /**
     * Return the char sequence in the Editor from its internal representation.
     *
     * @return
     */
    std::string to_string();
};


#endif //KANGAROO_DOCS_SHAREDEDITOR_H
