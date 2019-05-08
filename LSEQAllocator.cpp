//
// Created by lucab on 07/05/2019.
//

#include <stdlib.h>
#include <time.h>
#include "LSEQAllocator.h"

bool getRandomBool();
int getRandomInt(int min, int max);

const std::vector<int> baseSize {32, 64, 128, 256, 512, 1024, 2048, 4096, 8192,
    16384, 32768, 65536, 131072, 262144, 524288, 1048576, 2097152, 4194304,
    8388608, 16777216, 33554432, 67108864, 134217728, 268435456, 536870912,
    1073741824, 2147483647};

LSEQAllocator::LSEQAllocator() {
    srand(time(NULL));
}

std::vector<int> LSEQAllocator::alloc(const std::vector<int> p,
        const std::vector<int> q) {
    int depth = -1, interval = 0, step;
    int pPos, qPos, base = baseSize.at(0), offset;
    bool strategy;
    std::vector<int> newPos;

    while(interval < 2){ //not enough for 1 insert
        depth++; //starting depth is 0
        //calculate the new base saturating at max int value
        if(depth < baseSize.size())
            base = baseSize.at(depth);
        //calculate position for p and q at the current depth
        pPos = (depth < p.size())? p.at(depth) : 0;
        //if interval is 1, previous pPos and qPos were consecutives, so
        //at this depth the identifier can be chosen between pPos and base
        qPos = (depth < q.size() && interval!=1)? q.at(depth) : base;
        interval = qPos - pPos;
        newPos.push_back(pPos); //copy position while exploring the tree
    }
    //step = min(boundary, interval), maximum step to stay between p and q
    step = interval;
    if(boundary < interval)
        step = boundary;

    //define strategy for the specified depth if not defined already
    if(allocStrategy.count(depth) == 0)
        allocStrategy.insert({depth, getRandomBool()});
    //strategy at specified depth
    strategy = (*allocStrategy.find(depth)).second;
    if(strategy) //boundary+
        offset = pPos+getRandomInt(1, step-1);
    else //boundary-
        offset = qPos-getRandomInt(1, step-1);
    newPos.at(newPos.size()-1) = offset; //add offset to last index

    return newPos;
}

/**
 * Generate a random boolean value.
 *
 * @return
 */
bool getRandomBool() {
    return rand() % 2;
}

/**
 * Generate a random number in the interval [min, max].
 *
 *
 * @param min
 * @param max
 * @return
 */
int getRandomInt(int min, int max) {
    return rand() % (max-min+1) + min;
}