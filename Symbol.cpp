//
// Created by lucab on 06/05/2019.
//

#include "Symbol.h"

Symbol::Symbol() : c(0), siteId(-1), siteCounter(-1) {};

Symbol::Symbol(char c, int site, int counter, std::vector<int> pos) : c(c),
    siteId(site), siteCounter(counter), position(pos){};

bool Symbol::operator<(const Symbol &other) const {
    int depth=0, p=-1, q=-1;
    int pSize=this->position.size(), qSize = other.position.size();
    if(pSize == 0 && qSize == 0) //both vectors are empty
        return false;
    do {
        p = (depth < pSize)? this->position.at(depth) : -1;
        q = (depth < qSize)? other.position.at(depth) : -1;
        depth++;
    } while (p == q && p != -1);
    if(p < q)
        return true;
    return false;
}

std::vector<int> Symbol::getPosition() {
    return position;
}

char Symbol::getValue() {
    return c;
}