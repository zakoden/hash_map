#pragma once

#include <list>
#include <stdexcept>
#include <utility>
#include <vector>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType> >
class HashMap {
 public:
    using ElemType = typename std::pair<const KeyType, ValueType>;
    using iterator = typename std::list<ElemType>::iterator;
    using const_iterator = typename std::list<ElemType>::const_iterator;

 private:
    size_t elem_count;
    size_t capacity;
    Hash hasher;
    std::list<ElemType> elements;
    std::vector<iterator> buckets;
    std::vector<size_t> buckets_size;

 public:
    HashMap() {
        elem_count = 0;
        capacity = 1;
        buckets.push_back(elements.end());
        buckets_size.push_back(0);
    }

    explicit HashMap(const Hash& set_hasher) : hasher(set_hasher) {
        elem_count = 0;
        capacity = 1;
        buckets.push_back(elements.end());
        buckets_size.push_back(0);
    }

    template<class InputIt>
    HashMap(InputIt first, InputIt last)  {
        elem_count = 0;
        capacity = 1;
        buckets.push_back(elements.end());
        buckets_size.push_back(0);
        for (InputIt it = first; it != last; ++it) {
            insert(*it);
        }
    }

    template<class InputIt>
    HashMap(InputIt first, InputIt last, const Hash& set_hasher) : hasher(set_hasher)  {
        elem_count = 0;
        capacity = 1;
        buckets.push_back(elements.end());
        buckets_size.push_back(0);
        for (InputIt it = first; it != last; ++it) {
            insert(*it);
        }
    }

    HashMap(std::initializer_list<ElemType> init_list) {
        elem_count = 0;
        capacity = init_list.size();
        buckets.resize(capacity);
        buckets_size.resize(capacity);
        for (size_t i = 0; i < capacity; ++i) {
            buckets[i] = elements.end();
            buckets_size[i] = 0;
        }
        for (auto it : init_list) {
            insert(it);
        }
    }

    HashMap(std::initializer_list<ElemType> init_list,
            const Hash& set_hasher) : hasher(set_hasher)  {
        elem_count = 0;
        capacity = init_list.size();
        buckets.resize(capacity);
        buckets_size.resize(capacity);
        for (size_t i = 0; i < capacity; ++i) {
            buckets[i] = elements.end();
            buckets_size[i] = 0;
        }
        for (auto it : init_list) {
            insert(it);
        }
    }

    HashMap(const HashMap& other) {
        elem_count = 0;
        capacity = other.capacity;
        buckets.resize(capacity);
        buckets_size.resize(capacity);
        for (size_t i = 0; i < capacity; ++i) {
            buckets[i] = elements.end();
            buckets_size[i] = 0;
        }
        for (auto it : other) {
            insert(it);
        }
    }

    HashMap& operator=(const HashMap& other) {
        if (this == &other) {
            return *this;
        }
        elem_count = 0;
        capacity = other.capacity;
        buckets.resize(capacity);
        buckets_size.resize(capacity);
        elements.clear();
        for (size_t i = 0; i < capacity; ++i) {
            buckets[i] = elements.end();
            buckets_size[i] = 0;
        }
        for (auto it : other) {
            insert(it);
        }
        return *this;
    }

    size_t size() const {
        return elem_count;
    }

    bool empty() const {
        return elem_count == 0;
    }

    Hash hash_function() const {
        return hasher;
    }

    iterator begin() {
        return elements.begin();
    }

    const_iterator begin() const {
        return elements.begin();
    }

    iterator end() {
        return elements.end();
    }

    const_iterator end() const {
        return elements.end();
    }

    bool need_rebuild() {
        return elem_count > capacity;
    }

    void rebuild() {
        capacity *= 2;
        buckets.resize(capacity);
        buckets_size.resize(capacity);
        for (size_t i = 0; i < capacity; ++i) {
            buckets[i] = elements.end();
            buckets_size[i] = 0;
        }
        std::list<ElemType> old_list;
        for (iterator it = elements.begin(); it != elements.end(); ++it) {
            old_list.push_back(*it);
        }
        elements.clear();
        elem_count = 0;
        for (iterator it = old_list.begin(); it != old_list.end(); ++it) {
            insert(*it);
        }
        old_list.clear();
    }

    void insert(ElemType elem) {
        if (need_rebuild()) {
            rebuild();
        }
        size_t bucket = hasher(elem.first) % capacity;
        if (buckets[bucket] == elements.end()) {
            iterator pos = elements.end();
            buckets[bucket] = elements.insert(pos, elem);
            ++elem_count;
            ++buckets_size[bucket];
        } else {
            iterator pos = buckets[bucket];
            for (size_t i = 0; i < buckets_size[bucket]; ++i) {
                if (pos->first == elem.first) {
                    return;
                }
                ++pos;
            }
            elements.insert(pos, elem);
            ++elem_count;
            ++buckets_size[bucket];
        }
    }

    void erase(KeyType key) {
        size_t bucket = hasher(key) % capacity;
        if (buckets[bucket] != elements.end()) {
            iterator pos = buckets[bucket];
            for (size_t i = 0; i < buckets_size[bucket]; ++i) {
                if (pos->first == key) {
                    iterator follow = elements.erase(pos);
                    if (i == 0) {
                        buckets[bucket] = follow;
                    }
                    --elem_count;
                    --buckets_size[bucket];
                    if (buckets_size[bucket] == 0) {
                        buckets[bucket] = elements.end();
                    }
                    break;
                }
                ++pos;
            }
        }
    }

    iterator find(KeyType key) {
        size_t bucket = hasher(key) % capacity;
        if (buckets[bucket] != elements.end()) {
            iterator pos = buckets[bucket];
            for (size_t i = 0; i < buckets_size[bucket]; ++i) {
                if (pos->first == key) {
                    return pos;
                }
                ++pos;
            }
        }
        return elements.end();
    }

    const_iterator find(KeyType key) const {
        size_t bucket = hasher(key) % capacity;
        if (buckets[bucket] != elements.end()) {
            iterator pos = buckets[bucket];
            for (size_t i = 0; i < buckets_size[bucket]; ++i) {
                if (pos->first == key) {
                    return pos;
                }
                ++pos;
            }
        }
        return elements.end();
    }

    ValueType& operator[](const KeyType& key) {
        iterator elem = find(key);
        if (elem == elements.end()) {
            insert({key, ValueType()});
            elem = find(key);
        }
        return elem->second;
    }

    const ValueType& at(const KeyType& key) const {
        const_iterator elem = find(key);
        if (elem == elements.end()) {
            throw std::out_of_range("");
        }
        return elem->second;
    }

    void clear() {
        while (!empty()) {
            erase(begin()->first);
        }
    }
};
