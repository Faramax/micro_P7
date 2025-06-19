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
#ifndef UP7HELPERS_H
#define UP7HELPERS_H

#if defined(__GNUC__)
    #define ST_PACK_ENTER(x) 
    #define ST_PACK_EXIT(x) 
    #define ST_ATTR_PACK(x)   __attribute__ ((aligned(x), packed))

#elif defined(_MSC_VER)
    #define ST_PACK_ENTER(x)  __pragma(pack(push, x))
    #define ST_PACK_EXIT()    __pragma(pack(pop))
    #define ST_ATTR_PACK(x)
#endif

#define CONCATENATE_DETAIL(x, y) x##y
#define CONCATENATE(x, y) CONCATENATE_DETAIL(x, y)
#define MAKE_UNIQUE(x) CONCATENATE(x, __COUNTER__)

#if !defined(TRUE)
    #define TRUE              1
    #define FALSE             0
#endif

#if !defined(UNUSED_ARG)
    #define UNUSED_ARG(x)     (void)(x)
#endif


#if !defined(ST_ASSERT)
    #define ST_ASSERT(cond)   typedef int MAKE_UNIQUE(assert_type) [(cond) ? 1 : -1]
#endif


#endif //UP7HELPERS_H
