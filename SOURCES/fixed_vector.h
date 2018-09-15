#include <cstddef>
#include <algorithm>
#include <cassert>
#include <iterator>

template <typename T, std::size_t N>
class fixed_vector {
public:
	fixed_vector() : _size(0) {}
	
	fixed_vector(const fixed_vector & rhs) : fixed_vector() {
		assert(rhs._size < N);
		for (std::size_t i = 0; i < rhs.size(); ++i) {
			push_back(rhs[i]);
		}
	}
	fixed_vector& operator=(const fixed_vector & rhs) {
		fixed_vector tmp(rhs);
		swap(*this, tmp);
		return *this;
	}
	~fixed_vector() {
		for (std::size_t i = 0; i < _size; ++i) {
			reinterpret_cast<T*>(&data[i])->~T();
		}
	}
	
	std::size_t size() const {
		return _size;
	 }
	constexpr std::size_t capacity() const {
		return N;
	}
	constexpr std::size_t max_size() const {
		return N;
	}
	
	T& operator[](std::size_t pos) {
		return *reinterpret_cast<T*>(&data[pos]);
	}
	const T& operator[](std::size_t pos) const {
		return *reinterpret_cast<const T*>(&data[pos]);
	}

	const T& back() const {
		return operator[](_size - 1);
	}
	T& back() {
		return operator[](_size - 1);
	}
	const T& front() const {
		return operator[](0);
	}
	T& front() {
		return operator[](0);
	}
	bool empty() {
		return _size == 0;
	}
	void clear() {
		while (!empty()) {
			pop_back();
		}
	}
	void push_back(T const& value) {
		assert(_size < N);
		new(&data[_size]) T(value);
		++_size;
	}
	void pop_back() {
		reinterpret_cast<T*>(&data[_size - 1])->~T();
		--_size;
	}
	friend void swap(fixed_vector &a, fixed_vector &b) noexcept {
		std::swap(a.data, b.data);
		std::swap(a._size, b._size);
	}
private:
	typename std::aligned_storage<sizeof(T), alignof(T)>::type data[N];
	std::size_t _size;

public:
	using iterator = T*;
	using const_iterator = const T*;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:
	iterator begin() { 
		return iterator(reinterpret_cast<T*>(data)); 
	}
	const_iterator begin() const {
		return const_iterator(const_cast<T*> (reinterpret_cast<const T*>(data)));
	}
	
	iterator end() { 
		return begin() + _size; 
	}
	const_iterator end() const { 
		return begin() + _size; 
	}
	
	const_iterator cend() const { 
		return end(); 
	}
	const_iterator cbegin() const { 
		return begin(); 
	}

	reverse_iterator rbegin() { 
		return reverse_iterator(end()); 
	}
	const_reverse_iterator rbegin() const {
		return const_reverse_iterator(end());
	}

	reverse_iterator rend() { 
		return reverse_iterator(begin()); 
	}
	const_reverse_iterator rend() const {
		return const_reverse_iterator(begin());
	}

	const_reverse_iterator crend() const {
		return const_reverse_iterator(begin());
	}
	const_reverse_iterator crbegin() const {
		return const_reverse_iterator(end());
	}

	
	iterator insert(const_iterator pos, T const& value) {
		auto i = static_cast<std::size_t>(std::distance(cbegin(), pos));
		push_back(back());
		for (auto j = _size - 1; j != i; --j) {
			operator[](j) = operator[](j - 1); 
		}

		operator[](i) = value;
		return iterator(pos);
	}
	
	iterator erase(const_iterator pos) {
		auto i = static_cast<std::size_t>(std::distance(cbegin(), pos));
		for (; i + 1 < _size; ++i) {
			operator[](i) = operator[](i + 1); 
		}
		pop_back();
		return iterator(pos);
	}

	iterator erase(const_iterator begin, const_iterator end) {
		for (const_iterator i = begin; i != end;) {
			i = erase(i);
			--end;
		}
		return iterator(end);
	}
};
