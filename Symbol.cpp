//
// Created by lucab on 06/05/2019.
//

#include "Symbol.h"

Symbol::Symbol() : c(0), siteId(-1), siteCounter(-1) {};

Symbol::Symbol(char c, int site, int counter, std::vector<int> pos) : c(c),
    siteId(site), siteCounter(counter), position(pos){};

bool Symbol::operator<(const Symbol &other) const {
    int depth=0, p=0, q=0;
    while(p == q){
        p = (depth < this->position.size())? this->position.at(depth) : 0;
        q = (depth < other.position.size())? other.position.at(depth) : 0;
        if(p == q && p == 0){
            if(this->siteId < other.siteId)
                return true;
            else if(this->siteCounter < other.siteCounter)
                return true;
            else
                return false;
        }
        depth++;
    }
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