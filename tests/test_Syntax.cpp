#define BOOST_TEST_MODULE Syntax
#include <boost/test/unit_test.hpp>

#include <platzhalter/Constraint.hpp>

//using namespace std;
using namespace platzhalter;

class Syntax_Fixture {
  protected:
};

BOOST_FIXTURE_TEST_SUITE(Syntax, Syntax_Fixture )

BOOST_AUTO_TEST_CASE( constants )
{
  Variable<unsigned,0> x;
  check(x <  10);
  check(x == 100); 
  check(x >= 3); 
  check((x != 510, x == 510)); 
  check((x != 510 && x == 510)); 
}

BOOST_AUTO_TEST_SUITE_END() // Syntax

//  vim: ft=cpp:ts=2:sw=2:expandtab
