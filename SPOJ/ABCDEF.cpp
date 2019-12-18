#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>
using namespace std;

// http://www.spoj.com/problems/ABCDEF/
const int MAX_N = 100;

int input[MAX_N];
namespace detail
{
	size_t next_prime(size_t size);
	template<typename Result, typename Functor>
	struct functor_storage : Functor
	{
		functor_storage() = default;
		functor_storage(const Functor & functor)
			: Functor(functor)
		{
		}
		template<typename... Args>
		Result operator()(Args &&... args)
		{
			return static_cast<Functor &>(*this)(std::forward<Args>(args)...);
		}
		template<typename... Args>
		Result operator()(Args &&... args) const
		{
			return static_cast<const Functor &>(*this)(std::forward<Args>(args)...);
		}
	};
	template<typename Result, typename... Args>
	struct functor_storage<Result, Result(*)(Args...)>
	{
		typedef Result(*function_ptr)(Args...);
		function_ptr function;
		functor_storage(function_ptr function)
			: function(function)
		{
		}
		Result operator()(Args... args) const
		{
			return function(std::forward<Args>(args)...);
		}
		operator function_ptr &()
		{
			return function;
		}
		operator const function_ptr &()
		{
			return function;
		}
	};
	constexpr size_t empty_hash = 0;
	inline size_t adjust_for_empty_hash(size_t value)
	{
		return std::max(size_t(1), value);
	}
	template<typename key_type, typename value_type, typename hasher>
	struct KeyOrValueHasher : functor_storage<size_t, hasher>
	{
		typedef functor_storage<size_t, hasher> hasher_storage;
		KeyOrValueHasher(const hasher & hash)
			: hasher_storage(hash)
		{
		}
		size_t operator()(const key_type & key)
		{
			return adjust_for_empty_hash(static_cast<hasher_storage &>(*this)(key));
		}
		size_t operator()(const key_type & key) const
		{
			return adjust_for_empty_hash(static_cast<const hasher_storage &>(*this)(key));
		}
		size_t operator()(const value_type & value)
		{
			return adjust_for_empty_hash(static_cast<hasher_storage &>(*this)(value.first));
		}
		size_t operator()(const value_type & value) const
		{
			return adjust_for_empty_hash(static_cast<const hasher_storage &>(*this)(value.first));
		}
		template<typename F, typename S>
		size_t operator()(const std::pair<F, S> & value)
		{
			return adjust_for_empty_hash(static_cast<hasher_storage &>(*this)(value.first));
		}
		template<typename F, typename S>
		size_t operator()(const std::pair<F, S> & value) const
		{
			return adjust_for_empty_hash(static_cast<const hasher_storage &>(*this)(value.first));
		}
	};
	template<typename key_type, typename value_type, typename key_equal>
	struct KeyOrValueEquality : functor_storage<bool, key_equal>
	{
		typedef functor_storage<bool, key_equal> equality_storage;
		KeyOrValueEquality(const key_equal & equality)
			: equality_storage(equality)
		{
		}
		bool operator()(const key_type & lhs, const key_type & rhs)
		{
			return static_cast<equality_storage &>(*this)(lhs, rhs);
		}
		bool operator()(const key_type & lhs, const value_type & rhs)
		{
			return static_cast<equality_storage &>(*this)(lhs, rhs.first);
		}
		bool operator()(const value_type & lhs, const key_type & rhs)
		{
			return static_cast<equality_storage &>(*this)(lhs.first, rhs);
		}
		bool operator()(const value_type & lhs, const value_type & rhs)
		{
			return static_cast<equality_storage &>(*this)(lhs.first, rhs.first);
		}
		template<typename F, typename S>
		bool operator()(const key_type & lhs, const std::pair<F, S> & rhs)
		{
			return static_cast<equality_storage &>(*this)(lhs, rhs.first);
		}
		template<typename F, typename S>
		bool operator()(const std::pair<F, S> & lhs, const key_type & rhs)
		{
			return static_cast<equality_storage &>(*this)(lhs.first, rhs);
		}
		template<typename F, typename S>
		bool operator()(const value_type & lhs, const std::pair<F, S> & rhs)
		{
			return static_cast<equality_storage &>(*this)(lhs.first, rhs.first);
		}
		template<typename F, typename S>
		bool operator()(const std::pair<F, S> & lhs, const value_type & rhs)
		{
			return static_cast<equality_storage &>(*this)(lhs.first, rhs.first);
		}
		template<typename FL, typename SL, typename FR, typename SR>
		bool operator()(const std::pair<FL, SL> & lhs, const std::pair<FR, SR> & rhs)
		{
			return static_cast<equality_storage &>(*this)(lhs.first, rhs.first);
		}
	};
	template<typename T>
	struct lazily_defauly_construct
	{
		operator T() const
		{
			return T();
		}
	};
	template<typename It>
	struct WrappingIterator : std::iterator<std::random_access_iterator_tag, void, ptrdiff_t, void, void>
	{
		WrappingIterator(It it, It begin, It end)
			: it(it), begin(begin), end(end)
		{
		}
		WrappingIterator & operator++()
		{
			if (++it == end)
				it = begin;
			return *this;
		}
		WrappingIterator operator++(int)
		{
			WrappingIterator copy(*this);
			++*this;
			return copy;
		}
		WrappingIterator & operator--()
		{
			if (it == begin) it += end - begin;
			--it;
			return *this;
		}
		WrappingIterator operator--(int)
		{
			WrappingIterator copy(*this);
			--*this;
			return copy;
		}
		WrappingIterator & operator+=(ptrdiff_t distance)
		{
			it += distance;
			if (it >= end)
			{
				ptrdiff_t range_size = end - begin;
				ptrdiff_t to_begin = it - begin;
				it -= range_size * (to_begin / range_size);
			}
			return *this;
		}
		WrappingIterator & operator-=(ptrdiff_t distance)
		{
			it -= distance;
			if (it < begin)
			{
				ptrdiff_t range_size = end - begin;
				ptrdiff_t to_end = end - it;
				it += range_size * (to_end / range_size);
			}
			return *this;
		}
		WrappingIterator operator+(ptrdiff_t distance) const
		{
			return WrappingIterator(*this) += distance;
		}
		WrappingIterator operator-(ptrdiff_t distance) const
		{
			return WrappingIterator(*this) -= distance;
		}
		ptrdiff_t operator-(const WrappingIterator & other) const
		{
			if (other.it < it) return (end - begin) + (other.it - it);
			else return other.it - it;
		}
		bool operator==(const WrappingIterator & other) const
		{
			return it == other.it;
		}
		bool operator!=(const WrappingIterator & other) const
		{
			return !(*this == other);
		}

		It it;
		It begin;
		It end;
	};
	inline size_t required_capacity(size_t size, float load_factor)
	{
		return ::ceil(size / load_factor);
	}
	template<typename It, typename Do, typename Undo>
	void exception_safe_for_each(It begin, It end, Do && do_func, Undo && undo_func)
	{
		for (It it = begin; it != end; ++it)
		{
			try
			{
				do_func(*it);
			}
			catch (...)
			{
				while (it != begin)
				{
					undo_func(*--it);
				}
				throw;
			}
		}
	}
	std::invalid_argument invalid_max_load_factor();
	std::out_of_range at_out_of_range();
}

class sherwood_map
{
public:
	typedef int key_type;
	typedef int mapped_type;
	typedef std::pair<int, int> value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef std::hash<int> hasher;
	typedef std::equal_to<int> key_equal;
	typedef std::allocator<std::pair<int, int> > allocator_type;
	typedef value_type & reference;
	typedef const value_type & const_reference;
private:
	typedef typename std::allocator_traits<allocator_type>::template rebind_alloc<value_type> pretend_alloc;
public:
	typedef typename std::allocator_traits<pretend_alloc>::pointer pointer;
	typedef typename std::allocator_traits<pretend_alloc>::const_pointer const_pointer;

private:
	struct Entry;

	typedef typename std::allocator_traits<allocator_type>::template rebind_alloc<Entry> actual_alloc;
	typedef std::allocator_traits<actual_alloc> allocator_traits;

	typedef detail::KeyOrValueHasher<key_type, value_type, hasher> KeyOrValueHasher;
	typedef detail::KeyOrValueEquality<key_type, value_type, key_equal> KeyOrValueEquality;

	struct StorageType : actual_alloc, KeyOrValueHasher, KeyOrValueEquality
	{
		typedef typename allocator_traits::pointer iterator;
		typedef typename allocator_traits::const_pointer const_iterator;

		explicit StorageType(size_t size = 0, const hasher & hash = hasher(), const key_equal & equality = key_equal(), const actual_alloc & alloc = actual_alloc())
			: actual_alloc(std::move(alloc))
			, KeyOrValueHasher(hash)
			, KeyOrValueEquality(equality)
			, _begin(), _end()
		{
			if (size)
			{
				_begin = allocator_traits::allocate(*this, size);
				_end = _begin + static_cast<ptrdiff_t>(size);
			}
			try
			{
				detail::exception_safe_for_each(begin(), end(), [this](Entry & entry)
				{
					allocator_traits::construct(*this, &entry);
				},
					[this](Entry & entry)
				{
					allocator_traits::destroy(*this, &entry);
				});
			}
			catch (...)
			{
				allocator_traits::deallocate(*this, _begin, size);
				throw;
			}
		}
		StorageType(const StorageType & other, const actual_alloc & alloc)
			: StorageType(other.capacity(), static_cast<const hasher &>(other), static_cast<const key_equal &>(other), alloc)
		{
		}
		StorageType(const StorageType & other)
			: StorageType(other, static_cast<const actual_alloc &>(other))
		{
		}
		StorageType(StorageType && other, const actual_alloc & alloc)
			: actual_alloc(alloc)
			, KeyOrValueHasher(static_cast<KeyOrValueHasher &&>(other))
			, KeyOrValueEquality(static_cast<KeyOrValueEquality &&>(other))
			, _begin(other._begin), _end(other._end)
		{
			other._begin = other._end = iterator();
		}
		StorageType(StorageType && other) noexcept(std::is_nothrow_move_constructible<actual_alloc>::value && std::is_nothrow_move_constructible<hasher>::value && std::is_nothrow_move_constructible<key_equal>::value)
			: actual_alloc(static_cast<actual_alloc &&>(other))
			, KeyOrValueHasher(static_cast<KeyOrValueHasher &&>(other))
			, KeyOrValueEquality(static_cast<KeyOrValueEquality &&>(other))
			, _begin(other._begin), _end(other._end)
		{
			other._begin = other._end = iterator();
		}
		StorageType & operator=(const StorageType & other)
		{
			StorageType(other).swap(*this);
			return *this;
		}
		StorageType & operator=(StorageType && other) noexcept(std::is_nothrow_move_assignable<hasher>::value && std::is_nothrow_move_assignable<key_equal>::value && std::is_nothrow_move_assignable<actual_alloc>::value && std::is_nothrow_move_assignable<typename allocator_traits::pointer>::value)
		{
			swap(other);
			return *this;
		}
		void swap(StorageType & other) noexcept(std::is_nothrow_move_assignable<hasher>::value && std::is_nothrow_move_assignable<key_equal>::value && std::is_nothrow_move_assignable<actual_alloc>::value && std::is_nothrow_move_assignable<typename allocator_traits::pointer>::value)
		{
			using std::swap;
			swap(static_cast<hasher &>(static_cast<KeyOrValueHasher &>(*this)), static_cast<hasher &>(static_cast<KeyOrValueHasher &>(other)));
			swap(static_cast<key_equal &>(static_cast<KeyOrValueEquality &>(*this)), static_cast<key_equal &>(static_cast<KeyOrValueEquality &>(other)));
			swap(static_cast<actual_alloc &>(*this), static_cast<actual_alloc &>(other));
			swap(_begin, other._begin);
			swap(_end, other._end);
		}
		~StorageType()
		{
			if (!_begin)
				return;
			for (Entry & entry : *this)
				allocator_traits::destroy(*this, &entry);
			allocator_traits::deallocate(*this, _begin, capacity());
		}

		hasher hash_function() const
		{
			return static_cast<const KeyOrValueHasher &>(*this);
		}
		key_equal key_eq() const
		{
			return static_cast<const KeyOrValueEquality &>(*this);
		}
		actual_alloc get_allocator() const
		{
			return static_cast<const actual_alloc &>(*this);
		}

		size_t capacity() const
		{
			return _end - _begin;
		}

		iterator begin() { return _begin; }
		iterator end() { return _end; }
		const_iterator begin() const { return _begin; }
		const_iterator end() const { return _end; }

		void clear()
		{
			for (Entry & entry : *this)
			{
				if (entry.hash != detail::empty_hash)
				{
					entry.clear();
				}
			}
		}

		typedef detail::WrappingIterator<iterator> WrapAroundIt;

		iterator erase(iterator first, iterator last)
		{
			if (first == last) return first;
			size_t capacity = this->capacity();
			do
			{
				iterator initial = initial_bucket(first->hash, capacity);
				--initial->bucket_size;
				WrapAroundIt wrap(first, begin(), end());
				for (WrapAroundIt next = wrap;; wrap = next)
				{
					++next;
					size_t hash = next.it->hash;
					if (hash == detail::empty_hash || next.it->bucket_distance == 0)
					{
						break;
					}
					wrap.it->entry = std::move(next.it->entry);
					wrap.it->hash = hash;
					if (next.it == last) --last;
				}
				for (WrapAroundIt next_bucket = std::next(WrapAroundIt(initial, begin(), end())); next_bucket.it->bucket_distance > 0; ++next_bucket)
				{
					--next_bucket.it->bucket_distance;
				}
				wrap.it->clear();
				while (first->hash == detail::empty_hash)
				{
					++first;
					if (first == end()) return first;
				}
			} while (first != end() && first != last);
			return first;
		}

		iterator initial_bucket(size_type hash, size_t capacity)
		{
			return _begin + ptrdiff_t(hash % capacity);
		}
		const_iterator initial_bucket(size_type hash, size_t capacity) const
		{
			return _begin + ptrdiff_t(hash % capacity);
		}
		template<typename First>
		inline iterator find_hash(size_type hash, const First & first)
		{
			if (_begin == _end)
			{
				return _end;
			}
			iterator initial = initial_bucket(hash, this->capacity());
			uint32_t bucket_size = initial->bucket_size;
			if (bucket_size == 0)
				return _end;
			WrapAroundIt it{ initial, _begin, _end };
			for (std::advance(it, initial->bucket_distance); bucket_size--; ++it)
			{
				if (static_cast<KeyOrValueEquality &>(*this)(it.it->entry, first))
					return it.it;
			}
			return end();
		}
		enum EmplacePosResultType
		{
			FoundEqual,
			FoundNotEqual
		};
		struct EmplacePosResult
		{
			iterator bucket_it;
			iterator insert_it;
			EmplacePosResultType result;
		};


		template<typename First>
		EmplacePosResult find_emplace_pos(size_type hash, const First & first)
		{
			if (_begin == _end)
			{
				return { _end, _end, FoundNotEqual };
			}
			iterator initial = initial_bucket(hash, this->capacity());
			WrapAroundIt it{ initial, _begin, _end };
			std::advance(it, initial->bucket_distance);
			for (uint32_t i = initial->bucket_size; i > 0; --i, ++it)
			{
				if (it.it->hash == hash && static_cast<KeyOrValueEquality &>(*this)(it.it->entry, first))
					return { initial, it.it, FoundEqual };
			}
			return { initial, it.it, FoundNotEqual };
		}

	private:
		typename allocator_traits::pointer _begin;
		typename allocator_traits::pointer _end;
	};

	struct AdvanceIterator {};
	struct DoNotAdvanceIterator {};
	template<typename O, typename It>
	struct templated_iterator : std::iterator<std::forward_iterator_tag, O, ptrdiff_t>
	{
		templated_iterator()
			: it(), end()
		{
		}
		templated_iterator(It it, It end, AdvanceIterator)
			: it(it), end(end)
		{
			for (; this->it != this->end && this->it->hash == detail::empty_hash;)
			{
				++this->it;
			}
		}
		templated_iterator(It it, It end, DoNotAdvanceIterator)
			: it(it), end(end)
		{
		}
		template<typename OO, typename OIt>
		templated_iterator(templated_iterator<OO, OIt> other)
			: it(other.it), end(other.end)
		{
		}

		O & operator*() const
		{
			return it->entry;
		}
		O * operator->() const
		{
			return &it->entry;
		}
		templated_iterator & operator++()
		{
			return *this = templated_iterator(it + ptrdiff_t(1), end, AdvanceIterator{});
		}
		templated_iterator operator++(int)
		{
			templated_iterator copy(*this);
			++*this;
			return copy;
		}
		bool operator==(const templated_iterator & other) const
		{
			return it == other.it;
		}
		bool operator!=(const templated_iterator & other) const
		{
			return !(*this == other);
		}
		template<typename OO, typename OIt>
		bool operator==(const templated_iterator<OO, OIt> & other) const
		{
			return it == other.it;
		}
		template<typename OO, typename OIt>
		bool operator!=(const templated_iterator<OO, OIt> & other) const
		{
			return !(*this == other);
		}

	private:
		friend class sherwood_map;
		It it;
		It end;
	};

public:
	typedef templated_iterator<value_type, typename StorageType::iterator> iterator;
	typedef templated_iterator<const value_type, typename StorageType::const_iterator> const_iterator;


	const_iterator begin()	const { return { entries.begin(), entries.end(),	AdvanceIterator{} }; }
	const_iterator end()	const { return { entries.end(),	entries.end(),	DoNotAdvanceIterator{} }; }
	const_iterator cbegin()	const { return { entries.begin(), entries.end(),	AdvanceIterator{} }; }
	const_iterator cend()	const { return { entries.end(),	entries.end(),	DoNotAdvanceIterator{} }; }

	bool empty() const
	{
		return _size == 0;
	}
	size_type size() const
	{
		return _size;
	}
	std::pair<iterator, bool> insert(const value_type & value)
	{
		return emplace(value);
	}
	std::pair<iterator, bool> insert(value_type && value)
	{
		return emplace(std::move(value));
	}
	template<typename First, typename... Args>
	std::pair<iterator, bool> emplace(First && first, Args &&... args)
	{
		return emplace_with_hash(static_cast<KeyOrValueHasher &>(entries)(first), std::forward<First>(first), std::forward<Args>(args)...);
	}
	std::pair<iterator, bool> emplace()
	{
		return emplace(value_type());
	}
	template<typename... Args>
	iterator emplace_hint(const_iterator, Args &&... args)
	{
		return emplace(std::forward<Args>(args)...).first;
	}
	mapped_type & operator[](const key_type & key)
	{
		return emplace(key, detail::lazily_defauly_construct<mapped_type>()).first->second;
	}
	mapped_type & operator[](key_type && key)
	{
		return emplace(std::move(key), detail::lazily_defauly_construct<mapped_type>()).first->second;
	}
	template<typename T>
	iterator find(const T & key)
	{
		size_t hash = static_cast<KeyOrValueHasher &>(entries)(key);
		return { entries.find_hash(hash, key), entries.end(), DoNotAdvanceIterator{} };
	}
	template<typename T>
	const_iterator find(const T & key) const
	{
		return const_cast<sherwood_map &>(*this).find(key);
	}
	template<typename T>
	mapped_type & at(const T & key)
	{
		auto found = find(key);
		if (found == end()) throw detail::at_out_of_range();
		else return found->second;
	}
	template<typename T>
	const mapped_type & at(const T & key) const
	{
		return const_cast<sherwood_map &>(*this).at(key);
	}
	template<typename T>
	size_type count(const T & key) const
	{
		return find(key) == end() ? 0 : 1;
	}
	template<typename T>
	std::pair<iterator, iterator> equal_range(const T & key)
	{
		auto found = find(key);
		if (found == end()) return { found, found };
		else return { found, std::next(found) };
	}
	template<typename T>
	std::pair<const_iterator, const_iterator> equal_range(const T & key) const
	{
		return const_cast<sherwood_map &>(*this).equal_range(key);
	}
	void swap(sherwood_map & other)
	{
		using std::swap;
		entries.swap(other.entries);
		swap(_size, other._size);
		swap(_max_load_factor, other._max_load_factor);
	}
	float load_factor() const
	{
		size_type capacity_copy = entries.capacity();
		if (capacity_copy) return static_cast<float>(size()) / capacity_copy;
		else return 0.0f;
	}
	float max_load_factor() const
	{
		return _max_load_factor;
	}
	void max_load_factor(float value)
	{
		if (value >= 0.01f && value <= 1.0f)
		{
			_max_load_factor = value;
		}
		else
		{
			throw detail::invalid_max_load_factor();
		}
	}
	void reserve(size_type count)
	{
		size_t new_size = detail::required_capacity(count, max_load_factor());
		if (new_size > entries.capacity()) reallocate(detail::next_prime(new_size));
	}
	void rehash(size_type count)
	{
		if (count < size() / max_load_factor()) count = detail::next_prime(size_type(::ceil(size() / max_load_factor())));
		reallocate(count);
	}
	size_type bucket(const key_type & key) const
	{
		return entries.initial_bucket(static_cast<const KeyOrValueHasher &>(entries)(key), entries.capacity()) - entries.begin();
	}
	size_type bucket_count() const
	{
		return entries.capacity();
	}
	size_type max_bucket_count() const
	{
		return (allocator_traits::max_size(entries) - sizeof(*this)) / sizeof(Entry);
	}
	bool operator==(const sherwood_map & other) const
	{
		if (size() != other.size()) return false;
		return std::all_of(begin(), end(), [&other](const value_type & value)
		{
			auto found = other.find(value.first);
			return found != other.end() && *found == value;
		});
	}
	bool operator!=(const sherwood_map & other) const
	{
		return !(*this == other);
	}

	key_equal key_eq() const
	{
		return entries;
	}
	hasher hash_function() const
	{
		return entries;
	}
	allocator_type get_allocator() const
	{
		return static_cast<const actual_alloc &>(entries);
	}

private:
	void grow()
	{
		reallocate(detail::next_prime(std::max(detail::required_capacity(_size + 1, _max_load_factor), entries.capacity() * 2)));
	}
	void reallocate(size_type size)
	{
		StorageType replacement(size, entries.hash_function(), entries.key_eq(), entries.get_allocator());
		entries.swap(replacement);
		_size = 0;
		for (Entry & entry : replacement)
		{
			size_t hash = entry.hash;
			if (hash != detail::empty_hash)
				emplace_with_hash(hash, std::move(entry.entry));
		}
	}

	iterator iterator_const_cast(const_iterator it)
	{
		return { entries.begin() + ptrdiff_t(it.it - entries.begin()), entries.end(), DoNotAdvanceIterator{} };
	}

	struct Entry
	{
		Entry()
			: hash(detail::empty_hash), bucket_distance(0), bucket_size(0)
		{
		}
		template<typename... Args>
		void init(size_t hash, uint32_t distance, uint32_t size, Args &&... args)
		{
			new (&entry) value_type(std::forward<Args>(args)...);
			this->hash = hash;
			bucket_distance = distance;
			bucket_size = size;
		}
		void reinit(size_t hash, uint32_t distance, uint32_t size, value_type && value)
		{
			entry = std::move(value);
			this->hash = hash;
			this->bucket_distance = distance;
			this->bucket_size = size;
		}
		void clear()
		{
			entry.~pair();
			hash = detail::empty_hash;
			bucket_distance = 0;
			bucket_size = 0;
		}

		~Entry()
		{
			if (hash == detail::empty_hash)
				return;
			entry.~pair();
		}

		void swap_non_empty(Entry & other)
		{
			using std::swap;
			swap(hash, other.hash);
			swap(entry, other.entry);
		}

		size_t hash;
		uint32_t bucket_distance;
		uint32_t bucket_size;
		union
		{
			value_type entry;
		};
	};
	template<typename First, typename... Args>
	std::pair<iterator, bool> emplace_with_hash(size_t hash, First && first, Args &&... args)
	{
		auto found = entries.find_emplace_pos(hash, first);
		if (found.result == StorageType::FoundEqual)
		{
			return { { found.insert_it, entries.end(), DoNotAdvanceIterator{} }, false };
		}
		if (size() + 1 > _max_load_factor * entries.capacity())
		{
			grow();
			found = entries.find_emplace_pos(hash, first);
		}
		typedef typename StorageType::WrapAroundIt WrapIt;
		if (found.insert_it->hash == detail::empty_hash)
		{
			if (found.insert_it == found.bucket_it)
			{
				found.insert_it->init(hash, 0, 1, std::forward<First>(first), std::forward<Args>(args)...);
			}
			else
			{
				found.insert_it->init(hash, 1, 0, std::forward<First>(first), std::forward<Args>(args)...);
				++found.bucket_it->bucket_size;
				for (WrapIt bucket_wrap = ++WrapIt(found.bucket_it, entries.begin(), entries.end()); bucket_wrap.it != found.insert_it; ++bucket_wrap)
				{
					++bucket_wrap.it->bucket_distance;
				}
			}
			++_size;
			return { { found.insert_it, entries.end(), DoNotAdvanceIterator{} }, true };
		}
		value_type new_value(std::forward<First>(first), std::forward<Args>(args)...);
		auto insert_wrap = WrapIt(found.insert_it, entries.begin(), entries.end());
		value_type & current_value = found.insert_it->entry;
		size_t & current_hash = found.insert_it->hash;
		for (auto bucket_wrap = std::next(WrapIt(found.bucket_it, entries.begin(), entries.end())); ; ++bucket_wrap)
		{
			++bucket_wrap.it->bucket_distance;
			uint32_t bucket_size = bucket_wrap.it->bucket_size;
			if (!bucket_size) continue;
			std::advance(insert_wrap, bucket_size);
			if (insert_wrap.it->hash == detail::empty_hash)
			{
				for (++bucket_wrap; bucket_wrap != insert_wrap; ++bucket_wrap)
				{
					++bucket_wrap.it->bucket_distance;
				}
				++_size;
				insert_wrap.it->init(current_hash, 1, 0, std::move(current_value));
				break;
			}
			std::swap(current_value, insert_wrap.it->entry);
			std::swap(current_hash, insert_wrap.it->hash);
		}
		++found.bucket_it->bucket_size;
		current_value = std::move(new_value);
		current_hash = hash;
		return { { found.insert_it, entries.end(), DoNotAdvanceIterator{} }, true };
	}

	StorageType entries;
	size_t _size = 0;
	static constexpr const float default_load_factor = 0.85f;
	float _max_load_factor = default_load_factor;
};

int main()
{
	sherwood_map set;
	set.reserve(MAX_N*MAX_N*MAX_N);
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	int t, v2_size = 0, n = 0;
	cin >> t;

	while(t--)
	{
		cin >> input[n++];
	}

	for (int i = 0; i<n; i++)
	{
		for (int j = 0; j<n; j++)
		{
			for (int k = 0; k<n; k++)
			{
				if (input[k] == 0) continue;
				const int tmp = (input[i] + input[j])*input[k];

				auto it = set.find(tmp);
				if (it == set.end())
					set[tmp] = 1;
				else
					++it->second;
			}
		}
	}

	int lo, hi, res = 0;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			for (int k = 0; k < n; k++)
			{
				const int tmp = input[i] * input[j] + input[k];
				auto it = set.find(tmp);
				if (it != set.end())
					res += it->second;
			}
		}
	}

	cout << res;
	return 0;
}
namespace detail
{
	// prime numbers generated by the following method:
	// 1. start with a prime p = 2
	// 2. go to wolfram alpha and get p = NextPrime(2 * p)
	// 3. repeat 2. until you overflow 64 bits
	// you now have large gaps which you would hit if somebody called reserve() with an unlucky number.
	// 4. to fill the gaps for every prime p go to wolfram alpha and get ClosestPrime(p * 2^(1/3)) and ClosestPrime(p * 2^(2/3)) and put those in the gaps
	// 5. get PrevPrime(2^64) and put it at the end
	static constexpr const size_t prime_list[] =
	{
		2llu, 3llu, 5llu, 7llu, 11llu, 13llu, 17llu, 23llu, 29llu, 37llu, 47llu,
		59llu, 73llu, 97llu, 127llu, 151llu, 197llu, 251llu, 313llu, 397llu,
		499llu, 631llu, 797llu, 1009llu, 1259llu, 1597llu, 2011llu, 2539llu,
		3203llu, 4027llu, 5087llu, 6421llu, 8089llu, 10193llu, 12853llu, 16193llu,
		20399llu, 25717llu, 32401llu, 40823llu, 51437llu, 64811llu, 81649llu,
		102877llu, 129607llu, 163307llu, 205759llu, 259229llu, 326617llu,
		411527llu, 518509llu, 653267llu, 823117llu, 1037059llu, 1306601llu,
		1646237llu, 2074129llu, 2613229llu, 3292489llu, 4148279llu, 5226491llu,
		6584983llu, 8296553llu, 10453007llu, 13169977llu, 16593127llu, 20906033llu,
		26339969llu, 33186281llu, 41812097llu, 52679969llu, 66372617llu,
		83624237llu, 105359939llu, 132745199llu, 167248483llu, 210719881llu,
		265490441llu, 334496971llu, 421439783llu, 530980861llu, 668993977llu,
		842879579llu, 1061961721llu, 1337987929llu, 1685759167llu, 2123923447llu,
		2675975881llu, 3371518343llu, 4247846927llu, 5351951779llu, 6743036717llu,
		8495693897llu, 10703903591llu, 13486073473llu, 16991387857llu,
		21407807219llu, 26972146961llu, 33982775741llu, 42815614441llu,
		53944293929llu, 67965551447llu, 85631228929llu, 107888587883llu,
		135931102921llu, 171262457903llu, 215777175787llu, 271862205833llu,
		342524915839llu, 431554351609llu, 543724411781llu, 685049831731llu,
		863108703229llu, 1087448823553llu, 1370099663459llu, 1726217406467llu,
		2174897647073llu, 2740199326961llu, 3452434812973llu, 4349795294267llu,
		5480398654009llu, 6904869625999llu, 8699590588571llu, 10960797308051llu,
		13809739252051llu, 17399181177241llu, 21921594616111llu, 27619478504183llu,
		34798362354533llu, 43843189232363llu, 55238957008387llu, 69596724709081llu,
		87686378464759llu, 110477914016779llu, 139193449418173llu,
		175372756929481llu, 220955828033581llu, 278386898836457llu,
		350745513859007llu, 441911656067171llu, 556773797672909llu,
		701491027718027llu, 883823312134381llu, 1113547595345903llu,
		1402982055436147llu, 1767646624268779llu, 2227095190691797llu,
		2805964110872297llu, 3535293248537579llu, 4454190381383713llu,
		5611928221744609llu, 7070586497075177llu, 8908380762767489llu,
		11223856443489329llu, 14141172994150357llu, 17816761525534927llu,
		22447712886978529llu, 28282345988300791llu, 35633523051069991llu,
		44895425773957261llu, 56564691976601587llu, 71267046102139967llu,
		89790851547914507llu, 113129383953203213llu, 142534092204280003llu,
		179581703095829107llu, 226258767906406483llu, 285068184408560057llu,
		359163406191658253llu, 452517535812813007llu, 570136368817120201llu,
		718326812383316683llu, 905035071625626043llu, 1140272737634240411llu,
		1436653624766633509llu, 1810070143251252131llu, 2280545475268481167llu,
		2873307249533267101llu, 3620140286502504283llu, 4561090950536962147llu,
		5746614499066534157llu, 7240280573005008577llu, 9122181901073924329llu,
		11493228998133068689llu, 14480561146010017169llu, 18446744073709551557llu
	};
	size_t next_prime(size_t size)
	{
		auto small_end = std::begin(prime_list) + 8;
		auto result = std::lower_bound(std::begin(prime_list), small_end, size);
		if (result == small_end) return *std::lower_bound(small_end, std::end(prime_list), size);
		else return *result;
	}
}