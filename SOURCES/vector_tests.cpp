#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include <doctest/doctest.h>
#include "fixed_vector.h"
//#include "base_vector.h"

template<class T, size_t size>
using test_vector = fixed_vector<T, size>;

 //template<class T, size_t size>
 //using test_vector = base::vector<T, size>;

TEST_CASE("Empty vector") {
    SUBCASE("int") {
        test_vector<int, 100> v;
        CHECK(v.size() == 0);
        CHECK(v.capacity() == 100);
    }

    SUBCASE("string") {
        test_vector<std::string, 50> v;
        CHECK(v.size() == 0);
        CHECK(v.capacity() == 50);
    }

    SUBCASE("unique_ptr") {
        test_vector<std::unique_ptr<int>, 10> v;
        CHECK(v.size() == 0);
        CHECK(v.capacity() == 10);
    }
}

TEST_CASE("One value") {
    SUBCASE("int") {
        test_vector<int, 100> v;
        v.push_back(10);
        REQUIRE(v.size() == 1);
        CHECK(v.back() == 10);
    }

    SUBCASE("string") {
        test_vector<std::string, 50> v;
        std::string s("asdf");
        v.push_back(s);
        REQUIRE(v.size() == 1);
        CHECK(v.back() == s);
    }

    // SUBCASE("unique_ptr") {
    //     test_vector<std::unique_ptr<int>, 10> v;
    //     int *x = new int(10);
    //     std::unique_ptr<int> up(x);
    //     v.push_back(std::move(up));
    //     REQUIRE(v.size() == 1);
    //     CHECK(v.back().get() == x);
    // }
}

TEST_CASE("insertion") {
    test_vector<int, 3> v;
    REQUIRE(v.capacity() == 3);

    SUBCASE("push_back") {
        v.push_back(1);
        v.push_back(2);
        v.push_back(3);
        CHECK(v[0] == 1);
        CHECK(v[1] == 2);
        CHECK(v[2] == 3);
    }

    SUBCASE("insert") {
        v.insert(v.begin(), 1);
        v.insert(v.begin(), 2);
        v.insert(v.begin(), 3);
        CHECK(v[2] == 1);
        CHECK(v[1] == 2);
        CHECK(v[0] == 3);
    }
}

TEST_CASE("erasing") {
    const std::vector<std::string> strings({"0", "1"});
    
    test_vector<std::string, 2> v;
    v.push_back(strings[0]);
    v.push_back(strings[1]);


    SUBCASE("pop_back") {
       v.pop_back();
       CHECK(v.front() == strings.front());
    }
    
    SUBCASE("erase") {
        v.erase(v.begin());
        CHECK(v.front() == strings.back());
    }
    
     SUBCASE("clear") {
         v.clear();
         CHECK(v.size() == 0);
         CHECK(v.empty());
     }
}

TEST_CASE("iterators") {
    const std::vector<std::vector<int>> matrix{{0, 1, 2},
                                               {3, 4, 5},
                                               {6, 7, 8}};

    test_vector<std::vector<int>, 3> vector;
    for (const auto &v : matrix) vector.push_back(v);

    SUBCASE("begin/end") {
        for (auto it = vector.begin(); it != vector.end(); ++it) {
            std::iterator_traits<decltype(it)>::difference_type pos = std::distance(vector.begin(), it);
            CHECK(*it == matrix[pos]);
        }
    }

    SUBCASE("rbegin/rend") {
        for (auto it = vector.rbegin(); it != vector.rend(); ++it) {
            std::iterator_traits<decltype(it)>::difference_type pos = std::distance(vector.rbegin(), it);
            CHECK(*it == *(matrix.rbegin() + pos));
        }
    }

    const auto &cvector(vector);
    SUBCASE("cbegin/cend") {
        CHECK(std::equal(cvector.begin(), cvector.end(), matrix.begin()));
    }

    SUBCASE("crbegin/crend") {
        CHECK(std::equal(cvector.rbegin(), cvector.rend(), matrix.rbegin()));
    }
}

TEST_CASE("random tests") {
    constexpr uint32_t cnt_tests = 100'000;
    constexpr uint32_t cnt_values = 1000;
    constexpr uint32_t cnt_erase = cnt_values / 10;

    std::vector<int> values;
    test_vector<int, cnt_values> vector;

    for (size_t j = 0; j < cnt_values; ++j) {
        values.push_back(random() % cnt_values);
        vector.push_back(random() % cnt_values);
    }

    SUBCASE("insert/erase") {
        for (size_t i = 0; i < cnt_erase; ++i) {
            vector.erase(vector.begin() + (random() % vector.size()));
            values.erase(values.begin() + (random() % values.size()));
       }
   }
}

