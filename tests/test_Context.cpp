#define BOOST_TEST_MODULE Context
#include <boost/test/unit_test.hpp>

#include <platzhalter/Constraint.hpp>
#include <platzhalter/Context.hpp>

#include <set>

//using namespace std;
using namespace platzhalter;

struct Context_Fixture {
  Context_Fixture ()
  : ctx(get_context())
  {
  }
  protected:
    Context ctx;
};

BOOST_FIXTURE_TEST_SUITE(Context, Context_Fixture )

BOOST_AUTO_TEST_CASE( constants )
{
  Variable<unsigned,0> x;
  Generator<> gen( x == 135421 );
  gen();
  BOOST_CHECK_EQUAL( gen[x], 135421 );
}

BOOST_AUTO_TEST_CASE( alu )
{
  Variable<unsigned,0> op;
  Variable<unsigned,1> a;
  Variable<unsigned,2> b;

  Generator<> gen;  
  gen
    ( a < 16)
    ( b < 16)
    (op < 4 )  // 4 opcodes
    (op != 0 || a + b < 16 )  // no add overflow
    (op != 1 || a > b      )  // no sub underflow
    (op != 2 || a * b < 16 )  // no m overflow
    (op != 3 || b != 0     )  // div valid
  ;
  gen(); // create a new assignment

  //printf("result: op=%d, a=%d, b=%d\n", gen[op], gen[a], gen[b]);
}

BOOST_AUTO_TEST_CASE( alu_enum )
{
  Variable<unsigned,0> op;
  Variable<unsigned,1> a;
  Variable<unsigned,2> b;

  Generator<> gen;  
  gen
    ( a < 16)
    ( b < 16)
    (op < 4 )  // 4 opcodes
    (op != 0 || a + b < 16 )  // no add overflow
    (op != 1 || a > b      )  // no sub underflow
    (op != 2 || a * b < 16 )  // no m overflow
    (op != 3 || b != 0     )  // div valid
  ;

  unsigned count=0;
  while( gen.next() ) {
    ++count;
    gen( op != gen[op] || a != gen[a] || b != gen[b] );
    //printf("result: op=%d, a=%d, b=%d\n", gen[op], gen[a], gen[b]);
  }
  BOOST_CHECK_EQUAL(count, 572);
}

BOOST_AUTO_TEST_CASE( boolean )
{
  Variable<bool,0> b;

  Generator<> gen;  
  gen(b == b)(); // create a new assignment

  bool b1 = gen[b];
  //printf("result: b1=%d\n", b1);
  gen(b != b1)();
  bool b2 = gen[b];
  //printf("result: b2=%d\n", b2);

  BOOST_CHECK_THROW ( gen( b != b2 )(), std::runtime_error );
}

BOOST_AUTO_TEST_CASE( pythagoras )
{
  Variable<unsigned long long,0> a;
  Variable<unsigned long long,1> b;
  Variable<unsigned long long,2> c;

  Generator<> gen;  
  gen(a*a + b*b == c*c)(); // create a new assignment

  unsigned long long av = gen[a];
  unsigned long long bv = gen[b];
  unsigned long long cv = gen[c];

  BOOST_CHECK_EQUAL( av*av + bv*bv, cv*cv);
}

BOOST_AUTO_TEST_CASE( less )
{
  Variable<unsigned,0> a;

  Generator<> gen;  
  gen(a < 256);

  std::set<unsigned> generated;
  while (gen.next()) {
    unsigned av = gen[a];
    generated.insert(av);
    gen( a != av );
  }

  BOOST_CHECK_EQUAL( generated.size(), 256);
}

BOOST_AUTO_TEST_CASE( less_equal )
{
  Variable<unsigned,0> a;

  Generator<> gen;  
  gen(a <= 256);

  std::set<unsigned> generated;
  while (gen.next()) {
    unsigned av = gen[a];
    generated.insert(av);
    gen( a != av );
  }

  BOOST_CHECK_EQUAL( generated.size(), 257);
}

BOOST_AUTO_TEST_CASE( greater )
{
  Variable<unsigned,0> a;

  Generator<> gen;  
  gen(a > (std::numeric_limits<unsigned>::max()-256) );

  std::set<unsigned> generated;
  while (gen.next()) {
    unsigned av = gen[a];
    generated.insert(av);
    gen( a != av );
  }

  BOOST_CHECK_EQUAL( generated.size(), 256);
}

BOOST_AUTO_TEST_CASE( greater_equal )
{
  Variable<unsigned,0> a;

  Generator<> gen;  
  gen(a >= (std::numeric_limits<unsigned>::max()-256) );

  std::set<unsigned> generated;
  while (gen.next()) {
    unsigned av = gen[a];
    generated.insert(av);
    gen( a != av );
  }

  BOOST_CHECK_EQUAL( generated.size(), 257);
}

BOOST_AUTO_TEST_CASE( divide ) 
{
  Variable<unsigned char, 0> a;
  Variable<unsigned char, 1> b;
  Variable<unsigned char, 2> q;
  Variable<unsigned char, 3> r;

  Generator<> gen;
  gen
    ( b != (unsigned char)  0 )
    ( a  < (unsigned char) 16 )
    ( b  < (unsigned char) 16 )
    ( q == a/b )
    ( r == a%b )
  ;

  while( gen.next() ) {
    gen( a != gen[a] || b != gen[b] );
    //printf("result: a=%d, b=%d, q=%d, r=%d\n", gen[a], gen[b], gen[q], gen[r]);
    BOOST_CHECK_EQUAL( gen[a]/gen[b], gen[q] );
    BOOST_CHECK_EQUAL( gen[a]%gen[b], gen[r] );
  }
}

BOOST_AUTO_TEST_CASE ( shiftleft )
{
  Variable<unsigned , 0> a;
  Variable<unsigned , 1> b;
  Variable<unsigned , 2> c;

  Generator<> gen;
  gen
    ( a <  16 )
    ( b <  256 )
    ( c == (a << b) )
  ;

  while( gen.next() ) {
    BOOST_CHECK(true);
    unsigned av = gen[a];
    BOOST_CHECK(true);
    unsigned bv = gen[b];
    BOOST_CHECK(true);
    gen( a != gen[a] || b != gen[b] );
    BOOST_CHECK(true);
    printf("result: a=%d, b=%d, c=%d\n", gen[a], gen[b], gen[c]);
    BOOST_REQUIRE_EQUAL( (unsigned) (gen[a] << gen[b]), gen[c] );
  }
}


BOOST_AUTO_TEST_SUITE_END() // Context

//  vim: ft=cpp:ts=2:sw=2:expandtab
