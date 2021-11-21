#include <strconv_numeric.h>
#include <vector>
#include <iostream>
#include <random>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <string_view>


using std::chrono::duration_cast;
using std::chrono::steady_clock;
using std::chrono::microseconds;
using std::chrono::milliseconds;
using std::string_view;
using std::string;
using std::cout;
using std::endl;

constexpr auto perf( steady_clock::time_point start, steady_clock::time_point end ) 
  { return duration_cast<milliseconds>(end - start).count(); }

template< typename result_type, typename test_type, typename test_function>
auto test( std::vector<test_type> data_fx, test_function fnobj )
  {
  size_t test_size { data_fx.size() };
  std::vector<result_type> out_data_fx;
  out_data_fx.resize( test_size );
  auto start = steady_clock::now();
  std::transform( data_fx.begin(), data_fx.end(), out_data_fx.begin(), fnobj );
  auto end = steady_clock::now();
  return perf(start,end);
  }

template<typename test_function>
struct test_executor
  {
  auto operator()( string_view info )
    {
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> distrib(1, 256);
    size_t test_size { 100000 };

    std::vector<typename test_function::test_data_record_t> data_fx;
    data_fx.reserve( test_size );

    for(size_t n=0; n<test_size; ++n)
      data_fx.emplace_back( test_function{}.issue_test_record(gen) );
    
    test_function obj{};
    auto perf_stralo = test<string>(data_fx, [&obj]( auto const & data )
      {
      return obj.stralgo_test(data);
      } );
    auto perf_legacy = test<string>(data_fx, [&obj]( auto const & data )
      {
      return obj.legacy_test(data);
      } );
    using namespace std::string_view_literals;
    return strconv::compose<char>( info, " stralgo: "sv, perf_stralo, " legacy: "sv, perf_legacy, " ms "sv);
    }
  };
  
struct compose_test_t
  {
  struct test_data_record_t
    {
    string example_string;
    double emaple_float;
    int64_t example_int;
    };
    
  auto issue_test_record( std::mt19937 & gen ) const noexcept
    {
    test_data_record_t result;
    
    std::uniform_int_distribution<> intd(65538, 1384096);
    result.example_int = intd(gen);
    result.emaple_float = static_cast<double>( intd(gen)/(0.1 * intd(gen)) );
    
    std::uniform_int_distribution<uint32_t> strld(1, 512);
    std::uniform_int_distribution<char> chard('a', 'z');
    result.example_string.resize( strld(gen ) );
    for( size_t sz{}; sz != result.example_string.size(); ++sz )
      result.example_string[sz] = chard(gen);
    return result;
    }
    
  string stralgo_test( test_data_record_t const & data ) const noexcept
    {
    using strconv::fmt;
    using strconv::format_e;
    using strconv::char_case_e;
    using strconv::prepend_sign_e;
    using strconv::padd_with_e;
    using strconv::include_prefix_e;
    using strconv::alignment_e;
    
    return strconv::compose<char>( 'T',
                                   data.example_string, ' ',
                                   data.emaple_float,
                                   ' ',
                                   strconv::fmt<strconv::integral_format_traits{
                                           .precision = 11,
                                           .format = format_e::hexadecimal,
                                           .char_case = char_case_e::lowercase,
                                           .padd_with = padd_with_e::zeros,
                                           .include_prefix = include_prefix_e::with_prefix,
                                                  }>(data.example_int) );
    }
  string legacy_test( test_data_record_t const & data ) const noexcept
    {
    char const * const fmt{ "T%s %f %11.11x" };
    int sz = snprintf(nullptr,0,fmt, data.example_string.c_str(), data.emaple_float, data.example_int );
    string result;
    result.resize(static_cast<size_t>(sz+1));
    sz = snprintf(result.data(), result.size(),fmt, data.example_string.c_str(), data.emaple_float, data.example_int );
    result.resize(static_cast<size_t>(sz));
    return result;
    }
  };
  
int main(int argc, char **argv) 
{
  cout << test_executor<compose_test_t>{}("compose") << endl;
  return EXIT_SUCCESS;
}
