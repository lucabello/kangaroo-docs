//
// Created by lucab on 06/05/2019.
//

#ifndef KANGAROO_DOCS_SYMBOL_H
#define KANGAROO_DOCS_SYMBOL_H

#include <vector>

/**
 * Char symbol represented as a CRDT (Common Replicated Data Type).
 */
class Symbol {
    char c;
    int siteId;
    int siteCounter;
    std::vector<int> position;
public:
    Symbol();
    Symbol(char c, int site, int counter, std::vector<int> pos);
    std::vector<int> getPosition();
};


#endif //KANGAROO_DOCS_SYMBOL_H
