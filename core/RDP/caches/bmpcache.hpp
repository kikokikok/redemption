/*
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   Product name: redemption, a FLOSS RDP proxy
   Copyright (C) Wallix 2010
   Author(s): Christophe Grosjean, Javier Caverni, Xavier Dunat, Martin Potier
*/

#ifndef _REDEMPTION_CORE_RDP_CACHES_BMPCACHE_HPP_
#define _REDEMPTION_CORE_RDP_CACHES_BMPCACHE_HPP_

#include <set>
#include <algorithm>

#include "bitmap.hpp"
#include "RDP/PersistentKeyListPDU.hpp"
#include "RDP/orders/RDPOrdersSecondaryBmpCache.hpp"
#include "fileutils.hpp"
#include "unique_ptr.hpp"


enum {
      BITMAP_FOUND_IN_CACHE
    , BITMAP_ADDED_TO_CACHE
};

class BmpCache {

public:
    static const uint8_t  MAXIMUM_NUMBER_OF_CACHES        = 5;

private:
    static const uint16_t MAXIMUM_NUMBER_OF_CACHE_ENTRIES = 8192;

    static const uint8_t IN_WAIT_LIST = 0x80;

    struct cache_element
    {
        Bitmap bmp;
        uint_fast32_t stamp;
        union {
            uint8_t  sig_8[8];
            uint32_t sig_32[2];
        } sig;
        uint8_t sha1[20];

        cache_element()
        : stamp(0)
        {}

        cache_element(Bitmap const & bmp)
        : bmp(bmp)
        , stamp(0)
        {}

        void reset() {
            this->stamp = 0;
            this->bmp.reset();
        }

        operator bool () const {
            return this->bmp.is_valid();
        }
    };

    class storage_value_set {
        size_t elem_size;
        unique_ptr<uint8_t[]> data;
        unique_ptr<void*[]> free_list;
        void* * free_list_cur;

        storage_value_set(storage_value_set const &);
        storage_value_set& operator=(storage_value_set const &);

    public:
        storage_value_set()
        : elem_size(0)
        {}

        template<class T>
        void update() {
            this->elem_size = std::max(
                this->elem_size, sizeof(typename std::aligned_storage<sizeof(T), alignof(T)>::type)
            );
        }

        void reserve(size_t sz) {
            REDASSERT(!this->data.get());
            this->data.reset(new uint8_t[sz * this->elem_size]);
            this->free_list.reset(new void*[sz]);
            this->free_list_cur = this->free_list.get();

            uint8_t * p = this->data.get();
            uint8_t * pe = p + sz * this->elem_size;
            for (; p != pe; p += this->elem_size, ++this->free_list_cur) {
                *this->free_list_cur = p;
            }
        }

        void * pop() {
            REDASSERT(this->free_list_cur != this->free_list.get());
            return *--this->free_list_cur;
        }

        void push(void * p) {
            *this->free_list_cur = p;
            ++this->free_list_cur;
        }
    };

    template<class T>
    class aligned_set_allocator
    : public std::allocator<T>
    {
    public:
        storage_value_set & storage;

        typedef typename std::allocator<T>::pointer pointer;
        typedef typename std::allocator<T>::size_type size_type;

        template<class U>
        struct rebind {
            typedef aligned_set_allocator<U> other;
        };

        aligned_set_allocator(storage_value_set & storage)
        : storage(storage)
        {
            this->storage.template update<T>();
        }

        aligned_set_allocator(aligned_set_allocator const & other)
        : std::allocator<T>(other)
        , storage(other.storage)
        {}

        template<class U>
        aligned_set_allocator(aligned_set_allocator<U> const & other)
        : std::allocator<T>(other)
        , storage(other.storage)
        {
            this->storage.template update<T>();
        }

        T * allocate(size_type /*n*/, std::allocator<void>::const_pointer /*hint*/ = 0)
        {
            return static_cast<T*>(this->storage.pop());
        }

        void deallocate(pointer p, size_type /*n*/)
        {
            this->storage.push(p);
        }
    };

    struct value_set
    {
        cache_element const & elem;

        value_set(cache_element const & elem)
        : elem(elem)
        {}

        bool operator<(value_set const & other) const {
            if (this->elem.bmp.cx() < other.elem.bmp.cx()) {
                return true;
            }
            else if (this->elem.bmp.cx() > other.elem.bmp.cx()) {
                return false;
            }

            if (this->elem.bmp.cy() < other.elem.bmp.cy()) {
                return true;
            }
            else if (this->elem.bmp.cy() > other.elem.bmp.cy()) {
                return false;
            }

            typedef std::pair<const uint8_t *, const uint8_t *> iterator_pair;
            const uint8_t * e = this->elem.sha1 + sizeof(this->elem.sha1);
            iterator_pair p = std::mismatch(this->elem.sha1 + 0, e, other.elem.sha1 + 0);
            return p.first == e ? false : *p.first < *p.second;
        }
    };

    class cache_range {
        cache_element * first;
        cache_element * last;

        typedef aligned_set_allocator<value_set> set_allocator;
        typedef std::less<value_set> set_compare;
        typedef std::set<value_set, set_compare, set_allocator> set_type;

        set_type sorted_elements;

    public:
        cache_range(cache_element * first, size_t sz, storage_value_set & storage)
        : first(first)
        , last(first + sz)
        , sorted_elements(set_compare(), storage)
        {}

        cache_element & operator[](size_t i) {
            return this->first[i];
        }

        const cache_element & operator[](size_t i) const {
            return this->first[i];
        }

        size_t size() const {
            return this->last - this->first;
        }

        void clear() {
            this->sorted_elements.clear();
            for (cache_element * p = this->first; p != this->last; ++p) {
                p->reset();
            }
        }

        static const uint32_t invalid_cache_index = 0xFFFFFFFF;

        uint16_t get_old_index() const {
            return this->priv_get_old_index(this->size());
        }

        uint16_t get_old_index(bool use_waiting_list) const {
            const int_fast16_t entries_max = this->size();
            return this->priv_get_old_index(use_waiting_list ? entries_max - 1 : entries_max);
        }

    private:
        inline uint16_t priv_get_old_index(int_fast16_t entries_max) const {
            cache_element * first = this->first;
            cache_element * last = first + entries_max;
            cache_element * current = first;
            while (++first < last) {
                if (first->stamp < current->stamp) {
                    current = first;
                }
            }
            return current - this->first;
        }

    public:
        uint32_t get_cache_index(const cache_element & e) const {
            set_type::const_iterator it = this->sorted_elements.find(e);
            if (it == this->sorted_elements.end()) {
                return invalid_cache_index;
            }

            return &it->elem - this->first;
        }

        void remove(cache_element const & e) {
            this->sorted_elements.erase(e);
        }

        void add(cache_element const & e) {
            this->sorted_elements.insert(e);
        }

        cache_range(cache_range &&) = default; // FIXME g++ (4.8, 4.9, other ?)
        cache_range(cache_range const &) = delete;
        cache_range& operator=(cache_range const &) = delete;
    };

public:
    struct CacheOption {
        uint16_t entries;
        uint16_t bmp_size;
        bool is_persistent;

        CacheOption(uint16_t entries = 0, uint16_t bmp_size = 0, bool is_persistent = false)
        : entries(entries)
        , bmp_size(bmp_size)
        , is_persistent(is_persistent)
        {}
    };

    class Cache : public cache_range {
        uint16_t bmp_size_;
        bool is_persistent_;

    public:
        Cache(cache_element * pdata, const CacheOption & opt, storage_value_set & storage)
        : cache_range(pdata, opt.entries, storage)
        , bmp_size_(opt.bmp_size)
        , is_persistent_(opt.is_persistent)
        {}

        uint16_t bmp_size() const {
            return this->bmp_size_;
        }

        bool persistent() const {
            return this->is_persistent_;
        }

        size_t entries() const {
            return this->size();
        }

        typedef cache_element Element;
    };


    const enum Owner {
          Front
        , Mod_rdp
        , Recorder
    } owner;

    const uint8_t bpp;
    const uint8_t number_of_cache;
    const bool    use_waiting_list;

private:
    const size_t size_elements;
    const unique_ptr<cache_element[]> elements;
    storage_value_set storage;

    static const size_t WAIT_LIST_INDEX = MAXIMUM_NUMBER_OF_CACHES;
    Cache caches[MAXIMUM_NUMBER_OF_CACHES + 1 /* wait_list */];

          uint32_t stamp;
    const uint32_t verbose;

public:
    BmpCache(Owner owner,
             const uint8_t bpp,
             uint8_t number_of_cache,
             bool use_waiting_list,
             CacheOption c0 = CacheOption(),
             CacheOption c1 = CacheOption(),
             CacheOption c2 = CacheOption(),
             CacheOption c3 = CacheOption(),
             CacheOption c4 = CacheOption(),
             uint32_t verbose = 0)
    : owner(owner)
    , bpp(bpp)
    , number_of_cache(number_of_cache)
    , use_waiting_list(use_waiting_list)
    , size_elements(c0.entries + c1.entries + c2.entries + c3.entries + c4.entries + (use_waiting_list ? MAXIMUM_NUMBER_OF_CACHE_ENTRIES : 0))
    , elements(new cache_element[this->size_elements])
    , caches{
        {this->elements.get(), c0, this->storage},
        {this->elements.get() + c0.entries, c1, this->storage},
        {this->elements.get() + c0.entries + c1.entries, c2, this->storage},
        {this->elements.get() + c0.entries + c1.entries + c2.entries, c3, this->storage},
        {this->elements.get() + c0.entries + c1.entries + c2.entries + c3.entries, c4, this->storage},
        {this->elements.get() + c0.entries + c1.entries + c2.entries + c3.entries + c4.entries, CacheOption(use_waiting_list ? MAXIMUM_NUMBER_OF_CACHE_ENTRIES : 0), this->storage}
    }
    , stamp(0)
    , verbose(verbose)
    {
        REDASSERT(
           (number_of_cache == (!c0.entries ? 0 :
           !c1.entries ? 1 :
           !c2.entries ? 2 :
           !c3.entries ? 3 :
           !c4.entries ? 4 : 5))
        //&&
        //    (number_of_cache > 1 ? c0.entries <= c1.entries && (
        //        number_of_cache > 2 ? c1.entries <= c2.entries && (
        //            number_of_cache > 3 ? c2.entries <= c3.entries && (
        //                number_of_cache > 4 ? c3.entries <= c4.entries : true
        //            ) : true
        //        ) : true
        //    ) : true)
        );

        this->storage.reserve(this->size_elements);

        if (this->verbose) {
            LOG( LOG_INFO
                , "BmpCache: %s bpp=%u number_of_cache=%u use_waiting_list=%s "
                    "cache_0(%u, %u, %s) cache_1(%u, %u, %s) cache_2(%u, %u, %s) "
                    "cache_3(%u, %u, %s) cache_4(%u, %u, %s)"
                , ((this->owner == Front) ? "Front" : ((this->owner == Mod_rdp) ? "Mod_rdp" : "Recorder"))
                , this->bpp, this->number_of_cache, (this->use_waiting_list ? "yes" : "no")
                , this->caches[0].size(), this->caches[0].bmp_size(), (caches[0].persistent() ? "yes" : "no")
                , this->caches[1].size(), this->caches[1].bmp_size(), (caches[1].persistent() ? "yes" : "no")
                , this->caches[2].size(), this->caches[2].bmp_size(), (caches[2].persistent() ? "yes" : "no")
                , this->caches[3].size(), this->caches[3].bmp_size(), (caches[3].persistent() ? "yes" : "no")
                , this->caches[4].size(), this->caches[4].bmp_size(), (caches[4].persistent() ? "yes" : "no")
                );
        }

        if (this->number_of_cache > MAXIMUM_NUMBER_OF_CACHES) {
            LOG( LOG_ERR, "BmpCache: %s number_of_cache(%u) > %u"
                , ((this->owner == Front) ? "Front" : ((this->owner == Mod_rdp) ? "Mod_rdp" : "Recorder"))
                , this->number_of_cache
                , MAXIMUM_NUMBER_OF_CACHES);
            throw Error(ERR_RDP_PROTOCOL);
        }

        const size_t max_entries = std::min(std::max({
            this->caches[0].size()
          , this->caches[1].size()
          , this->caches[2].size()
          , this->caches[3].size()
          , this->caches[4].size()
        })*2, size_t(MAXIMUM_NUMBER_OF_CACHE_ENTRIES));

        aux_::BmpMemAlloc::MemoryDef mems[] {
            aux_::BmpMemAlloc::MemoryDef(max_entries + 2048, 128),
            aux_::BmpMemAlloc::MemoryDef(max_entries + 2048, 1024),
            aux_::BmpMemAlloc::MemoryDef(max_entries + 2048, 4096),
            aux_::BmpMemAlloc::MemoryDef(max_entries + 1028, 8192),
            aux_::BmpMemAlloc::MemoryDef(std::min<size_t>(100, max_entries), 16384)
        };

        const size_t coef = this->use_waiting_list ? 3 : 2; /*+ compressed*/
        const size_t add_mem = (this->bpp == 8 ? sizeof(BGRPalette) : 0) + 32 /*arbitrary*/;

        for (unsigned i_cache = 0; i_cache < MAXIMUM_NUMBER_OF_CACHES; ++i_cache) {
            if (this->caches[i_cache].size()) {
                for (aux_::BmpMemAlloc::MemoryDef & mem: mems) {
                    if (this->caches[i_cache].bmp_size() + add_mem <= mem.sz) {
                        mem.cel += this->caches[i_cache].size() * coef;
                        break;
                    }
                }
            }
        }

        aux_::bitmap_data_allocator.reserve(mems[0], mems[1], mems[2], mems[3], mems[4]);
    }

    ~BmpCache() {
        if (this->verbose) {
            this->log();
        }
    }

    void reset() {
        if (this->verbose) {
            this->log();
        }
        this->stamp = 0;
        for (Cache & cache : this->caches) {
            cache.clear();
        }
    }

    void put(uint8_t id, uint16_t idx, const Bitmap & bmp, uint32_t key1, uint32_t key2) {
        REDASSERT((id & IN_WAIT_LIST) == 0);
        Cache & r = this->caches[id];
        if (idx == RDPBmpCache::BITMAPCACHE_WAITING_LIST_INDEX) {
            // Last bitmap cache entry is used by waiting list.
            //LOG(LOG_INFO, "BmpCache: Put bitmap to waiting list.");
            idx = r.size() - 1;
        }

        cache_element & e = r[idx];
        if (e) {
            r.remove(e);
        }
        e.bmp = bmp;
        e.bmp.compute_sha1(e.sha1);
        e.stamp = ++this->stamp;

        if (r.persistent()) {
            REDASSERT(key1 && key2);
            e.sig.sig_32[0] = key1;
            e.sig.sig_32[1] = key2;
        }
        REDASSERT(r.persistent() || (!key1 && !key2));
        r.add(e);
    }

    const Bitmap & get(uint8_t id, uint16_t idx) {
        if (id & IN_WAIT_LIST) {
            REDASSERT(this->owner != Mod_rdp);
            return this->caches[MAXIMUM_NUMBER_OF_CACHES][idx].bmp;
        }
        Cache & r = this->caches[id];
        if (idx == RDPBmpCache::BITMAPCACHE_WAITING_LIST_INDEX) {
            REDASSERT(this->owner != Front);
            // Last bitmap cache entry is used by waiting list.
            //LOG(LOG_INFO, "BmpCache: Get bitmap from waiting list.");
            idx = r.size() - 1;
        }
        return r[idx].bmp;
    }

    bool is_cache_persistent(uint8_t id) {
        if (id < MAXIMUM_NUMBER_OF_CACHES) {
            return this->caches[id].persistent();
        }
        if (id == MAXIMUM_NUMBER_OF_CACHES) {
            return true;
        }
        REDASSERT(id <= MAXIMUM_NUMBER_OF_CACHES);
        return false;
    }

    bool has_cache_persistent() const {
        for (unsigned i = 0; i < this->number_of_cache; ++i) {
            if (this->caches[i].persistent()) {
                return true;
            }
        }
        return false;
    }

    uint16_t get_cache_usage(uint8_t cache_id) const {
        REDASSERT((cache_id & IN_WAIT_LIST) == 0);
        uint16_t cache_entries = 0;
        unsigned cache_index = 0;
        const cache_range & r = this->caches[cache_id];
        const unsigned last_index = this->caches[cache_id].size();
        for (; cache_index < last_index; ++cache_index) {
            if (r[cache_index]) {
                ++cache_entries;
            }
        }
        return cache_entries;
    }

    void log() const {
        LOG( LOG_INFO
            , "BmpCache: %s (0=>%u, %u%s) (1=>%u, %u%s) (2=>%u, %u%s) (3=>%u, %u%s) (4=>%u, %u%s)"
            , ((this->owner == Front) ? "Front" : ((this->owner == Mod_rdp) ? "Mod_rdp" : "Recorder"))
            , get_cache_usage(0), this->caches[0].size(), (this->caches[0].persistent() ? ", persistent" : "")
            , get_cache_usage(1), this->caches[1].size(), (this->caches[1].persistent() ? ", persistent" : "")
            , get_cache_usage(2), this->caches[2].size(), (this->caches[2].persistent() ? ", persistent" : "")
            , get_cache_usage(3), this->caches[3].size(), (this->caches[3].persistent() ? ", persistent" : "")
            , get_cache_usage(4), this->caches[4].size(), (this->caches[4].persistent() ? ", persistent" : ""));
    }

    TODO("palette to use for conversion when we are in 8 bits mode should be passed from memblt.cache_id, not stored in bitmap")
    uint32_t cache_bitmap(const Bitmap & oldbmp) {
        REDASSERT(this->owner != Mod_rdp);

        // Generating source code for unit test.
        //if (this->verbose & 8192) {
        //    if (this->finding_counter == 500) {
        //        BOOM;
        //    }
        //    LOG(LOG_INFO, "uint8_t palette_data_%d[] = {", this->finding_counter);
        //    hexdump_d((const char *)(void *)oldbmp.original_palette, sizeof(oldbmp.original_palette));
        //    LOG(LOG_INFO, "};", this->finding_counter);
        //    LOG(LOG_INFO, "uint8_t bitmap_data_%d[] = {", this->finding_counter);
        //    hexdump_d((const char *)(void *)oldbmp.data_bitmap.get(), oldbmp.bmp_size);
        //    LOG(LOG_INFO, "};", this->finding_counter);
        //    LOG(LOG_INFO, "memcpy(palette, palette_data_%d, sizeof(palette));", this->finding_counter);
        //    LOG(LOG_INFO, "init_palette332(palette);", this->finding_counter);
        //    LOG(LOG_INFO,
        //        "{Bitmap bmp_%d(%u, %u, &palette, %u, %u, bitmap_data_%d, %u, false);",
        //        this->finding_counter, this->bpp, oldbmp.original_bpp, oldbmp.cx, oldbmp.cy,
        //        this->finding_counter, oldbmp.bmp_size);
        //}

        Bitmap bmp(this->bpp, oldbmp);

        uint8_t id_real  = 0;

        for (const uint32_t bmp_size = bmp.bmp_size(); id_real < this->number_of_cache; ++id_real) {
            if (bmp_size <= this->caches[id_real].bmp_size()) {
                break;
            }
        }

        if (id_real == this->number_of_cache) {
            LOG( LOG_ERR
                , "BmpCache: %s bitmap size(%u) too big: cache_0=%u cache_1=%u cache_2=%u cache_3=%u cache_4=%u"
                , ((this->owner == Front) ? "Front" : ((this->owner == Mod_rdp) ? "Mod_rdp" : "Recorder"))
                , bmp.bmp_size()
                , (this->caches[0].size() ? this->caches[0].bmp_size() : 0)
                , (this->caches[1].size() ? this->caches[1].bmp_size() : 0)
                , (this->caches[2].size() ? this->caches[2].bmp_size() : 0)
                , (this->caches[3].size() ? this->caches[3].bmp_size() : 0)
                , (this->caches[4].size() ? this->caches[4].bmp_size() : 0)
                );
            REDASSERT(0);
            throw Error(ERR_BITMAP_CACHE_TOO_BIG);
        }

        Cache & cache = this->caches[id_real];
        const bool persistent = cache.persistent();

        cache_element e_compare(bmp);
        bmp.compute_sha1(e_compare.sha1);

        const uint32_t cache_index_32 = cache.get_cache_index(e_compare);
        if (cache_index_32 != cache_range::invalid_cache_index) {
            if (this->verbose & 512) {
                if (persistent) {
                    LOG( LOG_INFO
                        , "BmpCache: %s use bitmap %02X%02X%02X%02X%02X%02X%02X%02X stored in persistent disk bitmap cache"
                        , ((this->owner == Front) ? "Front" : ((this->owner == Mod_rdp) ? "Mod_rdp" : "Recorder"))
                        , e_compare.sha1[0], e_compare.sha1[1], e_compare.sha1[2], e_compare.sha1[3]
                        , e_compare.sha1[4], e_compare.sha1[5], e_compare.sha1[6], e_compare.sha1[7]);
                }
            }
            cache[cache_index_32].stamp = ++this->stamp;
            // Generating source code for unit test.
            //if (this->verbose & 8192) {
            //    LOG(LOG_INFO, "cache_id    = %u;", id_real);
            //    LOG(LOG_INFO, "cache_index = %u;", cache_index_32);
            //    LOG(LOG_INFO,
            //        "BOOST_CHECK_EQUAL(((BITMAP_FOUND_IN_CACHE << 24) | (cache_id << 16) | cache_index), "
            //            "bmp_cache.cache_bitmap(*bmp_%d));",
            //        this->finding_counter - 1);
            //    LOG(LOG_INFO, "delete bmp_%d;", this->finding_counter - 1);
            //    LOG(LOG_INFO, "");
            //}
            return (BITMAP_FOUND_IN_CACHE << 24) | (id_real << 16) | cache_index_32;
        }

        uint8_t  id = id_real;
        uint16_t oldest_cidx = cache.get_old_index(this->use_waiting_list);

        if (persistent && this->use_waiting_list) {
            // The bitmap cache is persistent.

            const uint32_t cache_index_32 = this->caches[WAIT_LIST_INDEX].get_cache_index(e_compare);
            if (cache_index_32 == cache_range::invalid_cache_index) {
                oldest_cidx = this->caches[WAIT_LIST_INDEX].get_old_index();
                id_real     =  MAXIMUM_NUMBER_OF_CACHES;
                id          |= IN_WAIT_LIST;

                if (this->verbose & 512) {
                    LOG( LOG_INFO, "BmpCache: %s Put bitmap %02X%02X%02X%02X%02X%02X%02X%02X into wait list."
                        , e_compare.sha1[0], e_compare.sha1[1], e_compare.sha1[2], e_compare.sha1[3]
                        , e_compare.sha1[4], e_compare.sha1[5], e_compare.sha1[6], e_compare.sha1[7]);
                }
            }
            else {
                this->caches[WAIT_LIST_INDEX].remove(e_compare);
                this->caches[WAIT_LIST_INDEX][cache_index_32].reset();

                if (this->verbose & 512) {
                    LOG( LOG_INFO
                        , "BmpCache: %s Put bitmap %02X%02X%02X%02X%02X%02X%02X%02X into persistent cache, cache_index=%u"
                        , e_compare.sha1[0], e_compare.sha1[1], e_compare.sha1[2], e_compare.sha1[3]
                        , e_compare.sha1[4], e_compare.sha1[5], e_compare.sha1[6], e_compare.sha1[7], oldest_cidx);
                }
            }
        }

        // find oldest stamp (or 0) and replace bitmap
        Cache & cache_real = this->caches[id_real];
        cache_element & e = cache_real[oldest_cidx];
        if (e) {
            cache_real.remove(e);
        }
        if (id_real == id) {
            ::memcpy(e.sig.sig_8, e_compare.sha1, sizeof(e.sig.sig_8));
        }
        ::memcpy(e.sha1, e_compare.sha1, 20);
        e.bmp = bmp;
        e.stamp = ++this->stamp;
        cache_real.add(e);
        // Generating source code for unit test.
        //if (this->verbose & 8192) {
        //    LOG(LOG_INFO, "cache_id    = %u;", id);
        //    LOG(LOG_INFO, "cache_index = %u;", oldest_cidx);
        //    LOG(LOG_INFO,
        //        "BOOST_CHECK_EQUAL(((BITMAP_ADDED_TO_CACHE << 24) | (cache_id << 16) | cache_index), "
        //            "bmp_cache.cache_bitmap(bmp_%d));",
        //        this->finding_counter - 1);
        //    LOG(LOG_INFO, "}");
        //    LOG(LOG_INFO, "");
        //}

        return (BITMAP_ADDED_TO_CACHE << 24) | (id << 16) | oldest_cidx;
    }

    const Cache & get_cache(uint8_t cache_id) const {
        return this->caches[cache_id];
    }
};

#endif
