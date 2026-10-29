// SPDX-FileCopyrightText: 2026 Artur Bać
// SPDX-License-Identifier: BSL-1.0
// SPDX-PackageHomePage: https://github.com/arturbac/stralgo
#include <unit_test_core.h>
#include <stralgo/utf/case.h>
#include <cwchar>
#include <string_view>

namespace ut = boost::ut;
using ut::operator""_test;
using ut::expect;
namespace utf = stralgo::utf;
using namespace std::string_view_literals;

struct case_sample_t
  {
  char32_t upper;
  char32_t lower;
  };

// pairs mapping both ways with simple case mapping
inline constexpr case_sample_t bidirectional_samples[]{
  {U'A', U'a'},
  {U'Z', U'z'},
  {U'À', U'à'},  // LATIN A WITH GRAVE
  {U'Ø', U'ø'},  // LATIN O WITH STROKE
  {U'Ą', U'ą'},  // LATIN A WITH OGONEK
  {U'Ł', U'ł'},  // LATIN L WITH STROKE
  {U'Ż', U'ż'},  // LATIN Z WITH DOT ABOVE
  {U'Α', U'α'},  // GREEK ALPHA
  {U'Σ', U'σ'},  // GREEK SIGMA
  {U'Ж', U'ж'},  // CYRILLIC ZHE
  {U'Ё', U'ё'},  // CYRILLIC IO
  {U'Ա', U'ա'},  // ARMENIAN AYB
  {U'Ⴀ', U'ⴀ'},  // GEORGIAN AN
  {U'Ａ', U'ａ'},  // FULLWIDTH A
};

// pairs outside BMP, only for 32 bit character types
inline constexpr case_sample_t bidirectional_samples_32[]{
  {U'\U00010400', U'\U00010428'},  // DESERET LONG I
  {U'\U0001E900', U'\U0001E922'},  // ADLAM ALIF
};

// characters without simple mapping
inline constexpr char32_t unmapped_samples[]{
  U'0', U'_', U' ', U'ß' /* SHARP S has only full uppercase mapping */, U'一', U'€', U'\U0001F600'};

template<typename char_type>
static void check_bidirectional(std::span<case_sample_t const> samples)
  {
  for(auto const & sample: samples)
    {
    auto const upper{static_cast<char_type>(sample.upper)};
    auto const lower{static_cast<char_type>(sample.lower)};
    expect(utf::to_lower(upper) == lower);
    expect(utf::to_upper(lower) == upper);
    expect(utf::to_lower(lower) == lower);
    expect(utf::to_upper(upper) == upper);
    }
  }

int main()
  {
  "to_lower_to_upper_char32_t"_test = []
  {
    check_bidirectional<char32_t>(bidirectional_samples);
    check_bidirectional<char32_t>(bidirectional_samples_32);
  };

  "to_lower_to_upper_char16_t"_test = [] { check_bidirectional<char16_t>(bidirectional_samples); };

  "to_lower_to_upper_wchar_t"_test = []
  {
    check_bidirectional<wchar_t>(bidirectional_samples);
#if WCHAR_MAX > 0xffff
    check_bidirectional<wchar_t>(bidirectional_samples_32);
#endif
  };

  "one_way_mappings"_test = []
  {
    expect(utf::to_lower(U'İ') == U'i');             // LATIN I WITH DOT ABOVE
    expect(utf::to_upper(U'i') == U'I');                  // simple mapping, not Turkish
    expect(utf::to_upper(U'µ') == U'Μ');        // MICRO SIGN
    expect(utf::to_upper(U'ς') == U'Σ');        // GREEK FINAL SIGMA
    expect(utf::to_upper(U'ÿ') == U'Ÿ');        // LATIN Y WITH DIAERESIS
    expect(utf::to_lower(U'Ω') == U'ω');        // OHM SIGN
    expect(utf::to_upper(U'ΐ') == U'ΐ');        // no simple uppercase mapping
  };

  "unmapped"_test = []
  {
    for(char32_t c: unmapped_samples)
      {
      expect(utf::to_lower(c) == c);
      expect(utf::to_upper(c) == c);
      }
    // UTF-16 surrogate code units are left unchanged
    expect(utf::to_lower(u'\xD801') == u'\xD801');
    expect(utf::to_upper(u'\xDC28') == u'\xDC28');
  };

  "compare_no_case"_test = []
  {
    expect(utf::compare_no_case(u"Żółw ĄĘ"sv, u"żÓŁW ąę"sv) == 0);
    expect(utf::compare_no_case(U"ЖУРНАЛ"sv, U"журнал"sv) == 0);
    expect(utf::compare_no_case(L"Straße"sv, L"STRAßE"sv) == 0);
    expect(utf::compare_no_case(u"abc"sv, u"ABD"sv) < 0);
    expect(utf::compare_no_case(u"abd"sv, u"ABC"sv) > 0);
    expect(utf::compare_no_case(u"ab"sv, u"ABC"sv) < 0);
    expect(utf::compare_no_case(u"abc"sv, u"AB"sv) > 0);
    expect(utf::compare_no_case(U""sv, U""sv) == 0);
  };
  }
