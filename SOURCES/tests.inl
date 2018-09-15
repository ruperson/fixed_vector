#include <gtest/gtest.h>

#ifndef _MSC_VER
#include "fault_injection.h"
#endif 

//using std::as_const;

template <typename T>
T const& as_const(T& obj)
{
    return obj;
}


template <typename C, typename T>
void mass_push_back(C& c, std::initializer_list<T> elems)
{
	for (T const& e : elems)
		c.push_back(e);
}

template <typename C, typename T>
void mass_push_front(C& c, std::initializer_list<T> elems)
{
	for (T const& e : elems)
		c.push_front(e);
}

template <typename It, typename T>
void expect_eq(It i1, It e1, std::initializer_list<T> elems)
{
	auto i2 = elems.begin(), e2 = elems.end();

	for (;;)
	{
		if (i1 == e1 || i2 == e2)
		{
			EXPECT_TRUE(i1 == e1 && i2 == e2);
			break;
		}

		EXPECT_EQ(*i2, *i1);
		++i1;
		++i2;
	}
}

template <typename C, typename T>
void expect_eq(C const& c, std::initializer_list<T> elems)
{
	expect_eq(c.begin(), c.end(), elems);
}

template <typename C, typename T>
void expect_reverse_eq(C const& c, std::initializer_list<T> elems)
{
	expect_eq(c.rbegin(), c.rend(), elems);
}

#ifndef _MSC_VER

TEST(fault_injection, push_back)
{
	faulty_run([] {
		counted::no_new_instances_guard g;

		container c;
		mass_push_back(c, { 1, 2, 3, 4 });
	});
}

TEST(fault_injection, push_back_2)
{
	faulty_run([]
	{
		container c;
		mass_push_back(c, { 1, 2, 3, 4 });

		try
		{
			c.push_back(5);
		}
		catch (...)
		{
			fault_injection_disable dg;
			expect_eq(c, { 1, 2, 3, 4 });
			throw;
		}

		fault_injection_disable dg;
		expect_eq(c, { 1, 2, 3, 4, 5 });
	});
}

TEST(fault_injection, assignment_operator)
{
	faulty_run([] {
		counted::no_new_instances_guard g;

		container c;
		mass_push_back(c, { 1, 2, 3, 4 });
		container c2;
		mass_push_back(c2, { 5, 6, 7, 8 });
		try
		{
			c2 = c;
		}
		catch (...)
		{
			expect_eq(c2, { 5, 6, 7, 8 });
			throw;
		}
		expect_eq(c2, { 1, 2, 3, 4 });
	});
}

TEST(fault_injection, non_throwing_default_ctor)
{
	faulty_run([]
	{
		try
		{
			container();
		}
		catch (...)
		{
			fault_injection_disable dg;
			ADD_FAILURE();
			throw;
		}
	});
}

TEST(fault_injection, copy_ctor)
{
	faulty_run([]
	{
		container c;
		mass_push_back(c, { 3, 2, 4, 1 });
		container c2 = c;
		fault_injection_disable dg;
		expect_eq(c, { 3, 2, 4, 1 });
	});
}

TEST(fault_injection, non_throwing_clear)
{
	faulty_run([]
	{
		container c;
		mass_push_back(c, { 3, 2, 4, 1 });
		try
		{
			c.clear();
		}
		catch (...)
		{
			fault_injection_disable dg;
			ADD_FAILURE();
			throw;
		}
	});
}

TEST(fault_injection, assignment_operator_2)
{
	faulty_run([]
	{
		container c;
		mass_push_back(c, { 3, 2, 4, 1 });
		container c2;
		mass_push_back(c2, { 8, 7, 2, 14 });

		try
		{
			c = c2;
		}
		catch (...)
		{
			fault_injection_disable dg;
			expect_eq(c, { 3, 2, 4, 1 });
			throw;
		}

		fault_injection_disable dg;
		expect_eq(c, { 8, 7, 2, 14 });
	});
}



TEST(fault_injection, insert)
{
	faulty_run([]
	{
		container c;
		mass_push_back(c, { 3, 2, 4, 1 });

		try
		{
			c.push_back(5);
		}
		catch (...)
		{
			fault_injection_disable dg;
			expect_eq(c, { 3, 2, 4, 1 });
			throw;
		}
		fault_injection_disable dg;
		expect_eq(c, { 3, 2, 4, 1, 5 });
	});
}

TEST(fault_injection, erase)
{
	faulty_run([]
	{
		container c;
		mass_push_back(c, { 6, 3, 8, 2, 5, 7, 10 });
		try
		{
			c.erase(c.begin());
		}
		catch (...)
		{
			fault_injection_disable dg;
			expect_eq(c, { 6, 3, 8, 2, 5, 7, 10 });
			throw;
		}
		fault_injection_disable dg;
		expect_eq(c, { 3, 8, 2, 5, 7, 10 });
	});
}

#endif 

static_assert(!std::is_constructible<container::iterator, std::nullptr_t>::value, "iterator should not be constructible from nullptr");
static_assert(!std::is_constructible<container::const_iterator, std::nullptr_t>::value, "const_iterator should not be constructible from nullptr");

TEST(correctness, default_ctor)
{
	counted::no_new_instances_guard g;

	container c;
	g.expect_no_instances();
	
}

TEST(correctness, begin_iterator)
{
	counted::no_new_instances_guard g;

	container c;
	container::iterator i = c.begin();

	EXPECT_EQ(c.begin(), i);
	c.push_back(5);

	EXPECT_EQ(5, *i);

	i = c.begin();
	EXPECT_EQ(5, *i);
}



TEST(correctness, back_front)
{
	counted::no_new_instances_guard g;

	container c;
	mass_push_back(c, { 1, 2, 3, 4, 5 });
	EXPECT_EQ(1, c.front());
	EXPECT_EQ(1, as_const(c).front());
	EXPECT_EQ(5, c.back());
	EXPECT_EQ(5, as_const(c).back());
}

TEST(correctness, back_front_ref)
{
	counted::no_new_instances_guard g;

	container c;
	mass_push_back(c, { 1, 2, 3, 4, 5 });
	c.front() = 6;
	c.back() = 7;
	expect_eq(c, { 6, 2, 3, 4, 7 });
}

TEST(correctness, back_front_cref)
{
	counted::no_new_instances_guard g;

	container c;
	mass_push_back(c, { 1, 2, 3, 4, 5 });
	EXPECT_TRUE(&c.front() == &as_const(c).front());
	EXPECT_TRUE(&c.back() == &as_const(c).back());
}

void magic(counted& c)
{
	c = 42;
}

void magic(counted const& c)
{}

TEST(correctness, back_front_ncref)
{
	counted::no_new_instances_guard g;

	container c;
	mass_push_back(c, { 1, 2, 3, 4, 5 });
	magic(as_const(c).front());
	magic(as_const(c).back());

	expect_eq(c, { 1, 2, 3, 4, 5 });
}

TEST(correctness, iterator_deref_1)
{
	counted::no_new_instances_guard g;

	container c;
	mass_push_back(c, { 1, 2, 3, 4, 5, 6 });
	container::iterator i = std::next(c.begin(), 3);
	EXPECT_EQ(4, *i);
	magic(*i);
	expect_eq(c, { 1, 2, 3, 42, 5, 6 });

	container::const_iterator j = std::next(c.begin(), 2);
	EXPECT_EQ(3, *j);
	magic(*j);
	expect_eq(c, { 1, 2, 3, 42, 5, 6 });
}

TEST(correctness, iterator_deref_1c)
{
	counted::no_new_instances_guard g;

	container c;
	mass_push_back(c, { 1, 2, 3, 4, 5, 6 });
	container::iterator const i = std::next(c.begin(), 3);
	EXPECT_EQ(4, *i);
	magic(*i);
	expect_eq(c, { 1, 2, 3, 42, 5, 6 });

	container::const_iterator const j = std::next(c.begin(), 2);
	EXPECT_EQ(3, *j);
	magic(*j);
	expect_eq(c, { 1, 2, 3, 42, 5, 6 });
}

TEST(correctness, iterator_deref_2)
{
	counted::no_new_instances_guard g;

	container c;
	mass_push_back(c, { 1, 2, 3, 4, 5, 6 });
	container::iterator i = std::next(c.begin(), 3);
	magic(*i.operator->());
	expect_eq(c, { 1, 2, 3, 42, 5, 6 });

	container::const_iterator j = std::next(c.begin(), 2);
	magic(*j.operator->());
	expect_eq(c, { 1, 2, 3, 42, 5, 6 });
}

TEST(correctness, iterator_deref_2c)
{
	counted::no_new_instances_guard g;

	container c;
	mass_push_back(c, { 1, 2, 3, 4, 5, 6 });
	container::iterator const i = std::next(c.begin(), 3);
	magic(*i.operator->());
	expect_eq(c, { 1, 2, 3, 42, 5, 6 });

	container::const_iterator const j = std::next(c.begin(), 2);
	EXPECT_EQ(3, *j);
	magic(*j.operator->());
	expect_eq(c, { 1, 2, 3, 42, 5, 6 });
}

TEST(correctness, iterator_constness)
{
	container c;
	mass_push_back(c, { 1, 2, 3 });
	magic(*as_const(c).begin());
	magic(*std::prev(as_const(c).end()));
	expect_eq(c, { 1, 2, 3 });
}

TEST(correctness, reverse_iterator_constness)
{
	container c;
	mass_push_back(c, { 1, 2, 3 });
	magic(*as_const(c).rbegin());
	magic(*std::prev(as_const(c).rend()));
	expect_eq(c, { 1, 2, 3 });
}

TEST(correctness, push_back)
{
	counted::no_new_instances_guard g;

	container c;
	mass_push_back(c, { 1, 2, 3, 4 });
	expect_eq(c, { 1, 2, 3, 4 });
}

TEST(correctness, copy_ctor)
{
	counted::no_new_instances_guard g;

	container c;
	mass_push_back(c, { 1, 2, 3, 4 });
	container c2 = c;
	expect_eq(c2, { 1, 2, 3, 4 });
}

TEST(correctness, copy_ctor_empty)
{
	counted::no_new_instances_guard g;

	container c;
	container c2 = c;
	EXPECT_TRUE(c2.empty());
}

TEST(correctness, assignment_operator)
{
	counted::no_new_instances_guard g;

	container c;
	mass_push_back(c, { 1, 2, 3, 4 });
	container c2;
	mass_push_back(c2, { 5, 6, 7, 8 });
	c2 = c;
	expect_eq(c2, { 1, 2, 3, 4 });
}

TEST(correctness, self_assignment)
{
	counted::no_new_instances_guard g;

	container c;
	mass_push_back(c, { 1, 2, 3, 4 });
	c = c;
	expect_eq(c, { 1, 2, 3, 4 });
}

TEST(correctness, pop_back)
{
	counted::no_new_instances_guard g;

	container c;
	mass_push_back(c, { 1, 2, 3, 4 });
	c.pop_back();
	expect_eq(c, { 1, 2, 3 });
	c.pop_back();
	expect_eq(c, { 1, 2 });
	c.pop_back();
	expect_eq(c, { 1 });
	c.pop_back();
	EXPECT_TRUE(c.empty());
}

TEST(correctness, empty)
{
	counted::no_new_instances_guard g;

	container c;
	EXPECT_EQ(c.begin(), c.end());
	EXPECT_TRUE(c.empty());
	c.push_back(1);
	EXPECT_NE(c.begin(), c.end());
	EXPECT_FALSE(c.empty());
	c.pop_back();
	EXPECT_EQ(c.begin(), c.end());
	EXPECT_TRUE(c.empty());
}

TEST(correctness, reverse_iterators)
{
	counted::no_new_instances_guard g;

	container c;
	mass_push_back(c, { 4,3,2,1 });
	expect_reverse_eq(c, { 1, 2, 3, 4 });

	EXPECT_EQ(1, *c.rbegin());
	EXPECT_EQ(2, *std::next(c.rbegin()));
	EXPECT_EQ(4, *std::prev(c.rend()));
}

TEST(correctness, iterator_conversions)
{
	counted::no_new_instances_guard g;

	container c;
	container::const_iterator i1 = c.begin();
	container::iterator i2 = c.end();
	EXPECT_TRUE(i1 == i1);
	EXPECT_TRUE(i1 == i2);
	EXPECT_TRUE(i2 == i1);
	EXPECT_TRUE(i2 == i2);
	EXPECT_FALSE(i1 != i1);
	EXPECT_FALSE(i1 != i2);
	EXPECT_FALSE(i2 != i1);
	EXPECT_FALSE(i2 != i2);

	EXPECT_TRUE(as_const(i1) == i1);
	EXPECT_TRUE(as_const(i1) == i2);
	EXPECT_TRUE(as_const(i2) == i1);
	EXPECT_TRUE(as_const(i2) == i2);
	EXPECT_FALSE(as_const(i1) != i1);
	EXPECT_FALSE(as_const(i1) != i2);
	EXPECT_FALSE(as_const(i2) != i1);
	EXPECT_FALSE(as_const(i2) != i2);

	EXPECT_TRUE(i1 == as_const(i1));
	EXPECT_TRUE(i1 == as_const(i2));
	EXPECT_TRUE(i2 == as_const(i1));
	EXPECT_TRUE(i2 == as_const(i2));
	EXPECT_FALSE(i1 != as_const(i1));
	EXPECT_FALSE(i1 != as_const(i2));
	EXPECT_FALSE(i2 != as_const(i1));
	EXPECT_FALSE(i2 != as_const(i2));

	EXPECT_TRUE(as_const(i1) == as_const(i1));
	EXPECT_TRUE(as_const(i1) == as_const(i2));
	EXPECT_TRUE(as_const(i2) == as_const(i1));
	EXPECT_TRUE(as_const(i2) == as_const(i2));
	EXPECT_FALSE(as_const(i1) != as_const(i1));
	EXPECT_FALSE(as_const(i1) != as_const(i2));
	EXPECT_FALSE(as_const(i2) != as_const(i1));
	EXPECT_FALSE(as_const(i2) != as_const(i2));
}

TEST(correctness, iterators_postfix)
{
	counted::no_new_instances_guard g;

	container s;
	mass_push_back(s, { 1, 2, 3 });
	container::iterator i = s.begin();
	EXPECT_EQ(1, *i);
	container::iterator j = i++;
	EXPECT_EQ(2, *i);
	EXPECT_EQ(1, *j);
	j = i++;
	EXPECT_EQ(3, *i);
	EXPECT_EQ(2, *j);
	j = i++;
	EXPECT_EQ(s.end(), i);
	EXPECT_EQ(3, *j);
	j = i--;
	EXPECT_EQ(3, *i);
	EXPECT_EQ(s.end(), j);
}

TEST(correctness, const_iterators_postfix)
{
	counted::no_new_instances_guard g;

	container s;
	mass_push_back(s, { 1, 2, 3 });
	container::const_iterator i = s.begin();
	EXPECT_EQ(1, *i);
	container::const_iterator j = i++;
	EXPECT_EQ(2, *i);
	EXPECT_EQ(1, *j);
	j = i++;
	EXPECT_EQ(3, *i);
	EXPECT_EQ(2, *j);
	j = i++;
	EXPECT_TRUE(i == s.end());
	EXPECT_EQ(3, *j);
	j = i--;
	EXPECT_EQ(3, *i);
	EXPECT_TRUE(j == s.end());
}

TEST(correctness, iterators_decrement)
{
	counted::no_new_instances_guard g;

	container s;
	mass_push_back(s, { 1, 2, 3, 5, 6, 7, 8, 10 });
	container::iterator i = s.end();
	EXPECT_EQ(10, *--i);
	EXPECT_EQ(8, *--i);
	EXPECT_EQ(7, *--i);
	EXPECT_EQ(6, *--i);
	EXPECT_EQ(5, *--i);
	EXPECT_EQ(3, *--i);
	EXPECT_EQ(2, *--i);
	EXPECT_EQ(1, *--i);
	EXPECT_EQ(s.begin(), i);
}

TEST(correctness, iterator_default_ctor)
{
	counted::no_new_instances_guard g;

	container::iterator i;
	container::const_iterator j;
	container s;
	mass_push_back(s, { 4, 1, 8, 6, 3, 2, 6 });

	i = s.begin();
	j = s.begin();
	EXPECT_EQ(4, *i);
	EXPECT_EQ(4, *j);
}


TEST(correctness, insert_begin)
{
	counted::no_new_instances_guard g;

	container c;
	mass_push_back(c, { 1, 2, 3, 4 });
	c.insert(c.begin(), 0);
	expect_eq(c, { 0, 1, 2, 3, 4 });
}

TEST(correctness, insert_middle)
{
	counted::no_new_instances_guard g;

	container c;
	mass_push_back(c, { 1, 2, 3, 4 });
	c.insert(std::next(c.begin(), 2), 5);
	expect_eq(c, { 1, 2, 5, 3, 4 });
}

TEST(correctness, insert_end)
{
	counted::no_new_instances_guard g;

	container c;
	mass_push_back(c, { 1, 2, 3, 4 });
	c.insert(c.end(), 5);
	expect_eq(c, { 1, 2, 3, 4, 5 });
}

TEST(correctness, insert_iterators)
{
	counted::no_new_instances_guard g;

	container c;
	mass_push_back(c, { 1, 2, 3, 4 });

	container::iterator i2 = c.begin();
	++i2;
	container::iterator i3 = i2;
	++i3;

	c.insert(i3, 5);
	++i2;
	EXPECT_EQ(5, *i2);
	++i2;
	EXPECT_EQ(3, *i2);
}

TEST(correctness, insert_return_value)
{
	counted::no_new_instances_guard g;

	container c;
	mass_push_back(c, { 1, 2, 3, 4 });

	container::iterator i = c.insert(std::next(c.begin(), 2), 5);
	EXPECT_EQ(5, *i);
	EXPECT_EQ(2, *std::prev(i));
	EXPECT_EQ(3, *std::next(i));
}

TEST(correctness, erase_begin)
{
	counted::no_new_instances_guard g;

	container c;
	mass_push_back(c, { 1, 2, 3, 4 });
	c.erase(c.begin());
	expect_eq(c, { 2, 3, 4 });
}

TEST(correctness, erase_middle)
{
	counted::no_new_instances_guard g;

	container c;
	mass_push_back(c, { 1, 2, 3, 4 });
	c.erase(std::next(c.begin(), 2));
	expect_eq(c, { 1, 2, 4 });
}

TEST(correctness, erase_close_to_end)
{
	counted::no_new_instances_guard g;

	container c;
	mass_push_back(c, { 1, 2, 3, 4, 5, 6 });
	c.erase(std::next(c.begin(), 4));
	expect_eq(c, { 1, 2, 3, 4, 6 });
}

TEST(correctness, erase_end)
{
	counted::no_new_instances_guard g;

	container c;
	mass_push_back(c, { 1, 2, 3, 4 });
	c.erase(std::prev(c.end()));
	expect_eq(c, { 1, 2, 3 });
}


TEST(correctness, erase_iterators)
{
	counted::no_new_instances_guard g;

	container c;
	mass_push_back(c, { 1, 2, 3, 4 });

	container::iterator i2 = c.begin();
	++i2;
	container::iterator i3 = i2;
	++i3;
	container::iterator i4 = i3;
	++i4;

	c.erase(i3);
	++i2;
	EXPECT_EQ(4, *i2);
}

TEST(correctness, erase_end_whole)
{
	counted::no_new_instances_guard g;

	container c;
	mass_push_back(c, { 1, 2, 3, 4 });
	c.erase(c.begin(), c.end());
	EXPECT_TRUE(c.empty());
	EXPECT_EQ(c.begin(), c.end());
}

TEST(correctness, erase_return_value)
{
	counted::no_new_instances_guard g;

	container c;
	mass_push_back(c, { 1, 2, 3, 4 });
	container::iterator i = c.erase(std::next(as_const(c).begin()));
	EXPECT_EQ(3, *i);
	i = c.erase(i);
	EXPECT_EQ(4, *i);
}

TEST(correctness, erase_range_return_value)
{
	counted::no_new_instances_guard g;

	container c;
	mass_push_back(c, { 1, 2, 3, 4, 5 });
	container::iterator i = c.erase(std::next(as_const(c).begin()), std::next(as_const(c).begin(), 3));
	EXPECT_EQ(4, *i);
	i = c.erase(i);
	EXPECT_EQ(5, *i);
}

TEST(correctness, erase_upto_end_return_value)
{
	counted::no_new_instances_guard g;

	container c;
	mass_push_back(c, { 1, 2, 3, 4, 5 });
	container::iterator i = c.erase(std::next(as_const(c).begin(), 2), as_const(c).end());
	EXPECT_TRUE(i == c.end());
	--i;
	EXPECT_EQ(2, *i);
}


TEST(correctness, size)
{
	counted::no_new_instances_guard g;

	container c;
	for (size_t i = 0; i != 10; ++i)
	{
		EXPECT_EQ(i, c.size());
		c.push_back(42);
	}
	EXPECT_EQ(10u, c.size());
}


TEST(correctness, swap)
{
	counted::no_new_instances_guard g;

	container c1, c2;
	mass_push_back(c1, { 1, 2, 3, 4 });
	mass_push_back(c2, { 5, 6, 7, 8 });
	swap(c1, c2);
	expect_eq(c1, { 5, 6, 7, 8 });
	expect_eq(c2, { 1, 2, 3, 4 });
}

TEST(correctness, swap_self)
{
	counted::no_new_instances_guard g;

	container c1;
	mass_push_back(c1, { 1, 2, 3, 4 });
	swap(c1, c1);
}

TEST(correctness, swap_empty_empty)
{
	counted::no_new_instances_guard g;

	container c1, c2;
	swap(c1, c2);
}

TEST(correctness, swap_empty_self)
{
	counted::no_new_instances_guard g;

	container c1;
	swap(c1, c1);
}

TEST(correctness, clear_empty)
{
	counted::no_new_instances_guard g;

	container c;
	c.clear();
	EXPECT_TRUE(c.empty());
	c.clear();
	EXPECT_TRUE(c.empty());
	c.clear();
	EXPECT_TRUE(c.empty());
}

TEST(correctness, clear)
{
	counted::no_new_instances_guard g;

	container c;
	mass_push_back(c, { 1, 2, 3, 4 });
	c.clear();
	EXPECT_TRUE(c.empty());
	EXPECT_EQ(c.begin(), c.end());
	mass_push_back(c, { 5, 6, 7, 8 });
	expect_eq(c, { 5, 6, 7, 8 });
}


TEST(correctness, swap_iterators_1)
{
	counted::no_new_instances_guard g;

	container c;
	mass_push_back(c, { 1, 2, 3 });
	container c2;
	mass_push_back(c2, { 4, 5, 6 });
	container::iterator i = std::next(c.begin(), 1);
	container::iterator j = std::next(c2.begin(), 2);

	{
		using std::swap;
		swap(i, j);
	}

	c2.erase(i);
	c.erase(j);
	expect_eq(c, { 1, 3 });
	expect_eq(c2, { 4, 5 });
}

TEST(correctness, bogus_queue)
{
	counted::no_new_instances_guard g;

	container c;
	mass_push_back(c, { 1, 2, 3, 4, 5 });

	for (int i = 6; i != 100; ++i)
	{
		c.push_back(i);
		c.erase(c.begin() + 1);
	}

	expect_eq(c, { 1, 96, 97, 98, 99 });
}

TEST(correctness, subscript)
{
	counted::no_new_instances_guard g;

	container c;
	mass_push_back(c, { 1, 2, 3, 4 });
	c.erase(c.begin() + 2);
	c.erase(c.begin());
	EXPECT_EQ(2, c[0]);
	EXPECT_EQ(4, c[1]);
	EXPECT_EQ(2, as_const(c)[0]);
	EXPECT_EQ(4, as_const(c)[1]);
}

int main(int ac, char *av[]) {
    testing::InitGoogleTest(&ac, av);
    return RUN_ALL_TESTS();
}