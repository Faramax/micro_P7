////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                     /
// This library is free software; you can redistribute it and/or modify it under the terms of the provided License.    /
//                                                                                                                     /
// This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even  the  implied/
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more/
// details.                                                                                                            /
// You should have received a copy of the the License along with this library.                                         /
//                                                                                                                     /
// 2012-2024 (c) Baical                                                                                                /
//                                                                                                                     /
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef UP7_HASH_H
#define UP7_HASH_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static inline uint32_t _getHash(const char *i_pName)
{
    uint32_t l_uResult = 2166136261ul;

    if (!i_pName)
    {
        return 0;
    }

    //Hash description: http://isthe.com/chongo/tech/comp/fnv/#FNV-param
    //Hash parameters investigation (collisions, randomnessification)
    //http://programmers.stackexchange.com/questions/49550/which-hashing-algorithm-is-best-for-uniqueness-and-speed
    //Collisions risk: FNV-1a produce 4 coll. on list of  216,553 English 
    // words
    while (*i_pName)
    {
        l_uResult = (l_uResult ^ *i_pName) * 16777619ul;
        i_pName++;
    }
    return l_uResult;
}

#endif //UP7_HASH_H
