//
// Created by lucab on 06/05/2019.
//

#include "Symbol.h"

Symbol::Symbol() : c(0), siteId(-1), siteCounter(-1) {};

Symbol::Symbol(char c, int site, int counter, std::vector<int> pos) : c(c),
    siteId(site), siteCounter(counter), position(pos){};

std::vector<int> Symbol::getPosition() {
    return position;
}