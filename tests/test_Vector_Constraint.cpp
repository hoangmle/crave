#define BOOST_TEST_MODULE Vector_Constraint_t
#include <boost/test/unit_test.hpp>

#include <crave/ConstrainedRandom.hpp>

#include <boost/format.hpp>

#include <set>
#include <iostream>

using boost::format;
using namespace crave;

#define ref(x) reference(x)
#define IF_THEN(a, b) !(a) || (b)

template<typename T>
bool check_unique(rand_vec<T>& v) {
  for (uint i = 0; i < v.size(); i++)
    for (uint j = 0; j < i; j++)
      if (v[i] == v[j]) return false;
  return true;
}

struct Context_Fixture {};

BOOST_FIXTURE_TEST_SUITE(Vector_Constraint_t, Context_Fixture )

struct Item : public rand_obj {
  Item() : v(this) {
    constraint(30 <= v().size() && v().size() <= 50);
    constraint.foreach(v, _i, v()[_i] == v()[_i - 1] + v()[_i - 2]);
    constraint.foreach(v, _i, IF_THEN(_i <= 1, v()[_i] == _i ) );
  }

  rand_vec<unsigned int> v;
};

BOOST_AUTO_TEST_CASE ( fibo_test )
{
  Item it;
  it.next();
  BOOST_REQUIRE(30 <= it.v.size() && it.v.size() <= 50);
  BOOST_REQUIRE(it.v[0] == 0);
  BOOST_REQUIRE(it.v[1] == 1);
  for (uint i = 2; i < it.v.size(); i++)
    BOOST_REQUIRE(it.v[i] == it.v[i - 1] + it.v[i - 2]);
}

struct Item1 : public rand_obj {
  Item1() : v(this) {
  }

  __rand_vec<unsigned int> u;
  rand_vec<unsigned int> v;
};

BOOST_AUTO_TEST_CASE ( free_vector_test )
{
  Item1 it;
  it.next();
  BOOST_REQUIRE(it.u.size() == 0);
  BOOST_REQUIRE(it.v.size() > 0);
  for (uint i = 0; i < it.v.size(); i++)
    std::cout << it.v[i] << " ";
  std::cout << std::endl;
}

struct Item2 : public rand_obj {
  Item2() : v(this) {
    constraint.foreach(v, _i, 100 <= v()[_i] && v()[_i] <= 200);
  }

  rand_vec<unsigned int> v;
};

BOOST_AUTO_TEST_CASE ( default_size_test )
{
  Item2 it;
  it.next();
  BOOST_REQUIRE(5 <= it.v.size() && it.v.size() <= 10);
  for (uint i = 0; i < it.v.size(); i++) {
    std::cout << it.v[i] << " ";
    BOOST_REQUIRE(100 <= it.v[i] && it.v[i] <= 200);
  }
  std::cout << std::endl;
}

struct Item3 : public rand_obj {
  Item3() : v(this) {
    constraint(v().size() == 100);
    constraint.foreach(v, _i, v()[_i] < 100);
    constraint.foreach(v, _i, v()[_i] >= 0);
    constraint.unique(v);
  }

  rand_vec<int> v;
};

BOOST_AUTO_TEST_CASE ( unique_test_1 )
{
  Item3 it;
  it.next();
  BOOST_REQUIRE(it.v.size() == 100);
  for (uint i = 0; i < it.v.size(); i++) {
    BOOST_REQUIRE(0 <= it.v[i] && it.v[i] < 100);
    for (uint j = 0; j < i; j++)
      BOOST_REQUIRE(it.v[i] != it.v[j]);
  }
}

BOOST_AUTO_TEST_CASE ( unique_test_2 )
{
  rand_vec<unsigned int> v(NULL);
  DefaultGenerator gen;
  gen(v().size() == 11);
  gen.foreach(v, _i, v()[_i] < 10);

  gen.unique(v);
  BOOST_REQUIRE(!gen.next());

  gen.non_unique(v);
  BOOST_REQUIRE(gen.next());
  BOOST_REQUIRE(v.size() == 11);
  for (uint i = 0; i < v.size(); i++) {
    std::cout << v[i] << " ";
    BOOST_REQUIRE(v[i] < 10);
  }
  std::cout << std::endl;
}

struct Item4 : public rand_obj {
  Item4() : v(this) {
    constraint(v().size() == 10);
    constraint.foreach("c1", v, _i, v()[_i] <= 100);
    constraint.foreach("c2", v, _i, v()[_i] > 100);
  }

  rand_vec<unsigned int> v;
};

BOOST_AUTO_TEST_CASE ( constraint_management_test )
{
  Item4 it;
  BOOST_REQUIRE( !it.next() );

  it.disable_constraint("c1");
  BOOST_REQUIRE( it.next() );
  for (uint i = 0; i < it.v.size(); i++)
    BOOST_REQUIRE( it.v[i] > 100 );

  it.enable_constraint("c1");
  it.disable_constraint("c2");
  BOOST_REQUIRE( it.next() );
  for (uint i = 0; i < it.v.size(); i++)
    BOOST_REQUIRE( it.v[i] <= 100 );

  it.enable_constraint("c2");
  BOOST_REQUIRE( !it.next() );
}

struct Item5 : public rand_obj {
  Item5() : v(this) {
    constraint(v().size() == 50);
    constraint.foreach(v, _i, IF_THEN(_i < 25, v()[_i] == _i));
    constraint.foreach(v, _i, IF_THEN(_i == 25, v()[_i] == 0));
    constraint.foreach(v, _i, IF_THEN(_i > 25, v()[_i] + _i == 200));
  }

  rand_vec<unsigned int> v;
};

BOOST_AUTO_TEST_CASE ( index_constraint_test )
{
  Item5 it;
  it.next();
  BOOST_REQUIRE(it.v.size() == 50);
  for (uint i = 0; i < it.v.size(); i++) {
    if (i < 25) BOOST_REQUIRE(it.v[i] == i);
    else if (i > 25) BOOST_REQUIRE(it.v[i] + i == 200);
    else BOOST_REQUIRE(it.v[i] == 0);
  }
}

BOOST_AUTO_TEST_CASE ( soft_vec_constraint )
{
  rand_vec<unsigned int> v(NULL);

  DefaultGenerator gen;
  gen(v().size() == 10);
  gen.foreach(v, _i, v()[_i] >= v()[_i - 1]);
  gen.soft_foreach(v, _i, v()[_i] < v()[_i - 1]);
  BOOST_REQUIRE(gen.next());

  DefaultGenerator gen1;
  gen1(v().size() == 4);
  gen1.foreach(v, _i, v()[_i] >= v()[_i - 1]);
  gen1.foreach(v, _i, v()[_i] <= 1000);
  gen1.soft_foreach(v, _i, v()[_i] <= v()[_i - 1]);
  gen1.soft_foreach(v, _i, IF_THEN(_i == 0, v()[_i] % 13 == 3));
  BOOST_REQUIRE(gen1.next());
  for (int i = 0; i < 10; i++) {
    BOOST_REQUIRE(v.size() == 4);
    std::cout << v[0] << " " << v[1] << " " << v[2] << " " << v[3] << std::endl;
    BOOST_REQUIRE(v[0] == v[1] && v[1] == v[2] && v[2] == v[3] && v[0] % 13 == 3);
    if (!gen1.next()) break;
  }
}

BOOST_AUTO_TEST_CASE ( mixed_bv_width_1 )
{
  rand_vec<signed char> a(NULL);
  Generator<Context> gen;
  gen(a().size() == 138);
  gen.foreach(a, _i, a()[_i] < (short) 10 );
  gen.unique(a);

  BOOST_REQUIRE(gen.next());
  for (uint i = 0; i < a.size(); i++)
    BOOST_REQUIRE(a[i] < 10);
  BOOST_REQUIRE(check_unique(a));
}

BOOST_AUTO_TEST_CASE ( mixed_bv_width_2 )
{
  rand_vec<short> a(NULL);
  DefaultGenerator gen;
  gen(a().size() == 19);
  gen.foreach(a, _i, a()[_i] < 10);
  gen.foreach(a, _i, a()[_i] > -10 );
  gen.unique(a);

  BOOST_REQUIRE(gen.next());
  for (uint i = 0; i < a.size(); i++)
    BOOST_REQUIRE(-10 < a[i] && a[i] < 10);
  BOOST_REQUIRE(check_unique(a));
}

BOOST_AUTO_TEST_CASE ( mixed_bv_width_3 )
{
  rand_vec<int> a(NULL);
  DefaultGenerator gen;
  gen(a().size() == 19);
  gen.foreach(a, _i, a()[_i] < (signed char) 10);
  gen.foreach(a, _i, a()[_i] > (short) -10 );
  gen.unique(a);

  BOOST_REQUIRE(gen.next());
  for (uint i = 0; i < a.size(); i++)
    BOOST_REQUIRE(-10 < a[i] && a[i] < 10);
  BOOST_REQUIRE(check_unique(a));
}

BOOST_AUTO_TEST_CASE ( mixed_bv_width_4 )
{
  rand_vec<short> a(NULL);
  DefaultGenerator gen;
  gen(a().size() == 10);
  gen.foreach(a, _i, -3 <= a()[_i] && a()[_i] <= 3);
  gen.foreach(a, _i, a()[_i] != 0);
  gen.foreach(a, _i, a()[_i] * a()[_i - 1] % 6 == 0);
  gen.foreach(a, _i, _i != 0 || a()[_i] % 2 == 0 );

  BOOST_REQUIRE(gen.next());
  for (uint i = 0; i < a.size(); i++) {
    BOOST_REQUIRE(-3 <= a[i] && a[i] <= 3);
    BOOST_REQUIRE(a[i] != 0);
    if (i == 0)
      BOOST_REQUIRE(a[i] % 2 == 0);
    else
      BOOST_REQUIRE(a[i] * a[i - 1] % 6 == 0);
  }
}

BOOST_AUTO_TEST_CASE ( bool_rand_vec )
{
  rand_vec<bool> a(NULL);
  Generator<Context> gen;
  gen(a().size() == 10);
  gen.foreach(a, _i, a()[_i] != a()[_i - 1]);

  BOOST_REQUIRE(gen.next());
  BOOST_REQUIRE(a.size() == 10);
  for (uint i = 1; i < a.size(); i++)
    BOOST_REQUIRE(a[i] != a[i - 1]);
}

BOOST_AUTO_TEST_SUITE_END() // Context

//  vim: ft=cpp:ts=2:sw=2:expandtab
