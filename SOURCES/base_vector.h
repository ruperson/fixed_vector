//
// Created by jesus on 09.06.18.
//

#pragma once

#include <vector>

namespace base {

template<typename T, std::size_t N>
class vector {
public:
    using inner_t = std::vector<T>;
    using iterator = typename inner_t::iterator;
    using const_iterator = typename inner_t::const_iterator;

    using reverse_iterator = typename inner_t::reverse_iterator;
    using reverse_const_iterator = typename inner_t::const_reverse_iterator;

private:
    inner_t d;

public:
    auto begin() { return d.begin(); }

    auto end() { return d.end(); }

    auto begin() const { return d.cbegin(); }

    auto end() const { return d.cend(); }

    auto rbegin() { return d.rbegin(); }

    auto rend() { return d.rend(); }

    auto rbegin() const { return d.crbegin(); }

    auto rend() const { return d.crend(); }

    vector() { d.reserve(N); }

    void push_back(T const & v) { d.push_back(v); }

    void push_back(T &&v) { d.push_back(std::move(v)); }

    void pop_back() { d.pop_back(); }

    decltype(auto) back() { return d.back(); }

    decltype(auto) front() { return d.front(); }

    auto empty() const { return d.empty(); }

    decltype(auto) operator[](std::size_t i) { return d[i]; }

    decltype(auto) operator[](std::size_t i) const { return d[i]; }

    auto size() const { return d.size(); }

    auto capacity() const { return d.capacity(); }

    auto data() const { return d.d(); }

    void reserve(std::size_t s) { return d.reserve(s); }

    void shrink_to_fit() { return d.shrink_to_fit(); }

    void clear() { d.clear(); }

    auto insert(const_iterator pos, T const &value) { return d.insert(pos, value); }

    auto erase(const_iterator pos) { return d.erase(pos); }

    auto erase(const_iterator first, const_iterator last) { return d.erase(first, last); }

    void swap(vector &v) { d.swap(v); }
};

} // namespace base
