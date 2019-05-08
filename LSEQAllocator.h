//
// Created by lucab on 07/05/2019.
//

#ifndef KANGAROO_DOCS_LSEQALLOCATOR_H
#define KANGAROO_DOCS_LSEQALLOCATOR_H

#include <map>
#include <vector>

/**
 * This class implements the algorithm presented in the paper:
 *
 * Brice Nédelec, Pascal Molli, Achour Mostefaoui, Emmanuel Desmontils.
 * LSEQ: an Adaptive Structure for Sequences in Distributed Collaborative
 * Editing. 13th ACM Symposium on Document Engineering (DocEng), Sep 2013,
 * Florence, Italy. pp.37–46, ff10.1145/2494266.2494278ff. ffhal-00921633
 * (online copy at https://hal.archives-ouvertes.fr/hal-00921633/document)
 *
 * It is an allocator for sequences in distributed collaborative editing,
 * and it is used by the CRDT to find a new position between two.
 */
class LSEQAllocator {
    int boundary;
    int siteId;
    std::map<int,bool> allocStrategy;
public:
    LSEQAllocator();
    /**
     * Allocates a new position between p and q, using LSEQ allocation function.
     * Allocated positions always have a position number in even positions
     * followed by the siteId of the editor in the following odd position.
     * This makes impossible to have two symbols with the same position.
     *
     * Example:
     * INSERT between [8,1,4,1] and [8,2,5,2] -> [8,1,5,1,number,siteId]
     *
     * @param p position of preceding element
     * @param q position of successive element
     * @return
     */
    std::vector<int> alloc(const std::vector<int> p, const std::vector<int> q);
    void setSiteId(int id);
};


#endif //KANGAROO_DOCS_LSEQALLOCATOR_H
