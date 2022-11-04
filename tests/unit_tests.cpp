
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include <cmath>
#include <decimal/decimal.hpp>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

static constexpr const auto NaN = std::numeric_limits<double>::quiet_NaN();

struct Testcase {
  std::string name;

  Testcase(std::string_view _name) : name{_name.data(), _name.size()} {
    std::cout << std::endl << "TEST: " << name << std::endl;
  }

  ~Testcase() { std::cout << std::endl; }

  auto expect(std::string const &result, std::string const &exp) -> bool {
    if (result == exp) {
      std::cout << "[OK] " << result << " == " << exp << std::endl;
    } else {
      std::cout << "[ERROR] " << result << " != " << exp << std::endl;
    }

    return result == exp;
  }

  auto expect(int const &result, int const &exp) -> bool {
    if (result == exp) {
      std::cout << "[OK] " << result << " == " << exp << std::endl;
    } else {
      std::cout << "[ERROR] " << result << " != " << exp << std::endl;
    }

    return result == exp;
  }

  auto expect(double const &result, double const &exp) -> bool {
    if (result == exp) {
      std::cout << "[OK] " << result << " == " << exp << std::endl;
    } else {
      std::cout << "[ERROR] " << result << " != " << exp << std::endl;
    }

    return result == exp;
  }

  auto expect(std::pair<double, int> const &result,
              std::pair<double, int> const &exp) -> bool {
    if (result.first == exp.first && result.second == exp.second) {
      std::cout << "[OK] "
                << "{" << std::fixed << std::setprecision(15) << result.first
                << "," << result.second << "}"
                << " == "
                << "{" << std::fixed << std::setprecision(15) << exp.first
                << "," << exp.second << "}" << std::endl;
    } else {
      std::cout << "[ERROR] "
                << "{" << std::fixed << std::setprecision(15) << result.first
                << "," << result.second << "}"
                << " != "
                << "{" << std::fixed << std::setprecision(15) << exp.first
                << "," << exp.second << "}" << std::endl;
    }

    return result == exp;
  }
};

void test_to_chars() {
  auto tc = Testcase("Decimal::ToChars");

  tc.expect(Decimal::ToChars(0.0), "0");
  tc.expect(Decimal::ToChars(-0.0), "-0");
  tc.expect(Decimal::ToChars(0), "0");
  tc.expect(Decimal::ToChars(-0), "0");

  tc.expect(Decimal::ToChars(123), "123");
  tc.expect(Decimal::ToChars(123.0), "123");
  tc.expect(Decimal::ToChars(123.123), "123.123");

  tc.expect(Decimal::ToChars(-123), "-123");
  tc.expect(Decimal::ToChars(-123.0), "-123");
  tc.expect(Decimal::ToChars(-123.123), "-123.123");
}

void test_dec_parts() {
  auto tc = Testcase("Decimal::GetDecimalSize");

  tc.expect(Decimal::GetDecimalSize(0.0), 1);
  tc.expect(Decimal::GetDecimalSize(-0.0), 2);
  tc.expect(Decimal::GetDecimalSize(0), 1);
  tc.expect(Decimal::GetDecimalSize(-0), 1);

  tc.expect(Decimal::GetDecimalSize(123), 3);
  tc.expect(Decimal::GetDecimalSize(123.0), 3);
  tc.expect(Decimal::GetDecimalSize(123.123), 7);

  tc.expect(Decimal::GetDecimalSize(-123), 4);
  tc.expect(Decimal::GetDecimalSize(-123.0), 4);
  tc.expect(Decimal::GetDecimalSize(-123.123), 8);
}

void test_int_parts() {
  auto tc = Testcase("Decimal::GetIntegeralPartSize");

  tc.expect(Decimal::GetIntegeralPartSize(0.0), 1);
  tc.expect(Decimal::GetIntegeralPartSize(-0.0), 2);
  tc.expect(Decimal::GetIntegeralPartSize(0), 1);
  tc.expect(Decimal::GetIntegeralPartSize(-0), 1);

  tc.expect(Decimal::GetIntegeralPartSize(123), 3);
  tc.expect(Decimal::GetIntegeralPartSize(123.0), 3);
  tc.expect(Decimal::GetIntegeralPartSize(123.123), 3);

  tc.expect(Decimal::GetIntegeralPartSize(-123), 4);
  tc.expect(Decimal::GetIntegeralPartSize(-123.0), 4);
  tc.expect(Decimal::GetIntegeralPartSize(-123.123), 4);
}

void test_frac_parts() {
  auto tc = Testcase("GetFractionalPartSize");

  tc.expect(Decimal::GetFractionalPartSize(0.0), 0);
  tc.expect(Decimal::GetFractionalPartSize(-0.0), 0);
  tc.expect(Decimal::GetFractionalPartSize(0), 0);
  tc.expect(Decimal::GetFractionalPartSize(-0), 0);

  tc.expect(Decimal::GetFractionalPartSize(123), 0);
  tc.expect(Decimal::GetFractionalPartSize(123.0), 0);
  tc.expect(Decimal::GetFractionalPartSize(123.123), 3);

  tc.expect(Decimal::GetFractionalPartSize(-123), 0);
  tc.expect(Decimal::GetFractionalPartSize(-123.0), 0);
  tc.expect(Decimal::GetFractionalPartSize(-123.123), 3);
}

void test_precision() {
  auto tc = Testcase("Decimal::GetPrecision");

  tc.expect(Decimal::GetPrecision(0.0), 0);
  tc.expect(Decimal::GetPrecision(-0.0), 0);
  tc.expect(Decimal::GetPrecision(0), 0);
  tc.expect(Decimal::GetPrecision(-0), 0);

  tc.expect(Decimal::GetPrecision(123), 0);
  tc.expect(Decimal::GetPrecision(123.0), 0);
  tc.expect(Decimal::GetPrecision(123.123), 3);

  tc.expect(Decimal::GetPrecision(-123), 0);
  tc.expect(Decimal::GetPrecision(-123.0), 0);
  tc.expect(Decimal::GetPrecision(-123.123), 3);
}

void test_to_chars_interval() {
  auto tc = Testcase("Decimal::ToChars(Interval)");

  tc.expect(Decimal::ToChars(0.0, 0.1), "0.0");
  tc.expect(Decimal::ToChars(-0.0, 0.001), "-0.000");
  tc.expect(Decimal::ToChars(0, 0.01), "0.00");
  tc.expect(Decimal::ToChars(-0, 1), "0");

  tc.expect(Decimal::ToChars(123, 0.01), "123.00");
  tc.expect(Decimal::ToChars(123.0, 10), "120");
  tc.expect(Decimal::ToChars(123.123, 3.1), "124.0");

  tc.expect(Decimal::ToChars(-123, 0), "-123");
  tc.expect(Decimal::ToChars(-123.0, 1000.0), "-0");
  tc.expect(Decimal::ToChars(-123.123, 0.123), "-123.123");
}

void test_specials() {
  auto tc = Testcase("Decimal::ToChars(Special)");

  tc.expect(Decimal::ToChars(7.40200133400), "7.402001334");
  tc.expect(Decimal::ToChars(0.00000000000740200133400),
            "0.000000000007402001334");
  tc.expect(Decimal::ToChars(128.82), "128.82");

  tc.expect(Decimal::ToChars(-7.40200133400), "-7.402001334");
  tc.expect(Decimal::ToChars(-0.00000000000740200133400),
            "-0.000000000007402001334");
  tc.expect(Decimal::ToChars(-128.82), "-128.82");
}

void test_shift_value() {
  auto tc = Testcase("Decimal::ShiftValue");

  tc.expect(Decimal::ShiftValue(7.40200133400),
            std::make_pair(7.40200133400, 0));
  tc.expect(Decimal::ShiftValue(0.00000000000740200133400),
            std::make_pair(0.00000000000740200133400 * std::pow(10, 12), 12));
  tc.expect(Decimal::ShiftValue(128.82), std::make_pair(128.82, 0));

  tc.expect(Decimal::ShiftValue(-7.40200133400),
            std::make_pair(-7.40200133400, 0));
  tc.expect(Decimal::ShiftValue(-0.00000000000740200133400),
            std::make_pair(-7.40200133400, 12));
  tc.expect(Decimal::ShiftValue(-128.82), std::make_pair(-128.82, 0));
}

void test_shift_chars() {
  auto tc = Testcase("Decimal::ShiftChars");

  tc.expect(Decimal::ShiftChars("0", 3), "0");
  tc.expect(Decimal::ShiftChars("0.0", 3), "0");
  tc.expect(Decimal::ShiftChars("0", -3), "0");
  tc.expect(Decimal::ShiftChars("0.0", 3), "0");

  tc.expect(Decimal::ShiftChars("0.000010", 3), "0.010");
  tc.expect(Decimal::ShiftChars("0.00001", 5), "1");
  tc.expect(Decimal::ShiftChars("0.00001", 6), "10");

  tc.expect(Decimal::ShiftChars("100", 0), "100");
  tc.expect(Decimal::ShiftChars("100", 2), "10000");
  tc.expect(Decimal::ShiftChars("10100", -5), "0.101");
  tc.expect(Decimal::ShiftChars("10100", -6), "0.0101");

  tc.expect(Decimal::ShiftChars("123.456", 0), "123.456");
  tc.expect(Decimal::ShiftChars("123.456", 1), "1234.56");
  tc.expect(Decimal::ShiftChars("123.456", 4), "1234560");
  tc.expect(Decimal::ShiftChars("123.456", -2), "1.23456");
  tc.expect(Decimal::ShiftChars("123.456", -3), "0.123456");
  tc.expect(Decimal::ShiftChars("123.456", -4), "0.0123456");
}

void test_decimal() {
  auto tc = Testcase("Decimal");

  auto d = Decimal(M_PI);
  auto d1 = Decimal(M_PI, 0.001);
  auto d2 = Decimal(d.str());
  auto d3 = Decimal(d.str(), 0.001);

  tc.expect(d.value(), M_PI);
  tc.expect(d1.value(), Decimal::ToValue(M_PI, 0.001));
  tc.expect(d2.value(), d.value());
  tc.expect(d3.value(), d1.value());

  tc.expect(d.str(), Decimal::ToChars(M_PI));
  tc.expect(d1.str(), Decimal::ToChars(M_PI, 0.001));
  tc.expect(d2.str(), d.str());
  tc.expect(d3.str(), d1.str());

  auto r1 = d - d1;
  auto r2 = d - d2;
  auto r3 = d * d1;
  auto r4 = d / d2;

  tc.expect(r1.value(), M_PI - d1.value());
  tc.expect(r2.value(), 0.0);
  tc.expect(r3.value(), M_PI * d1.value());
  tc.expect(r4.value(), M_PI / d2.value());
}

void test_misc() {
  auto tc = Testcase("Decimal::Misc");
  auto d = Decimal(NaN);
  auto d1 = Decimal("123.123"sv);

  tc.expect(d.str(), "nan");
  tc.expect(d1.str(), "123.123");
  tc.expect(d1.interval(), 0.001);
  tc.expect(d1.size(), 3);
}

int main() {
  test_shift_chars();
  test_shift_value();
  test_specials();
  test_to_chars();
  test_dec_parts();
  test_int_parts();
  test_frac_parts();
  test_precision();
  test_to_chars_interval();
  test_decimal();
  test_misc();
}