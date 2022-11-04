#pragma once

#include <algorithm>  // swap
#include <cmath>      // nearbyint, signbit, pow
#include <iomanip>    // setprecision
#include <iostream>
#include <limits>
#include <string>       // string, to_string, stod
#include <string_view>  // string_view
#include <utility>      // pair

#undef _DECIMAL_USE_STD_TO_CHARS

#ifdef _DECIMAL_USE_STD_TO_CHARS
#include <array>         // array
#include <charconv>      // to_chars, from_chars
#include <system_error>  // errc
#else
#include <sstream>  // stringstream
#endif

using namespace std;

class Decimal {
 private:
  static constexpr const auto kRangeMin = 1 + 1e-15;
  static constexpr const auto kRangeMax = 1e15;

  double interval_;
  double value_;

 public:
  static auto GetDecimalSize(std::string_view chars) -> int {
    return chars.size();
  }

  static auto GetIntegeralPartSize(std::string_view chars) -> int {
    auto dotp = chars.find_first_of('.');

    return dotp == std::string::npos ? chars.size() : dotp;
  }

  static auto GetFractionalPartSize(std::string_view chars) -> int {
    auto size = GetDecimalSize(chars) - GetIntegeralPartSize(chars);
    return size == 0 ? 0 : size - 1;
  }

  static auto GetPrecision(std::string_view chars) -> int {
    return GetFractionalPartSize(chars);
  }

  static auto ShiftValue(double value) -> std::pair<double, int> {
    auto s = std::signbit(value);
    auto v = s ? -value : value;
    auto r = std::make_pair(v, 0);

    if (v > 0) {
      while (v < kRangeMin) {
        v *= 1e1;
        ++r.second;
      }

      while (v > kRangeMax) {
        v *= 1e-1;
        --r.second;
      }
    }

    r.first *= std::pow(1e1, r.second);

    if (s) r.first = -r.first;

    return r;
  }

  static auto MaxDigits(double value) -> int {
    auto [v, shift] = ShiftValue(value);
    auto w = 0;
    auto e = std::numeric_limits<double>::infinity();
    auto p = 1.0;
    auto absv = std::abs(v);

    for (auto i = 0; i <= 15; ++i, p *= 10) {
      auto d =
          std::abs((std::fmod(std::round(absv * p), kRangeMax) / p) - absv);

      if (d < e) {
        e = d;
        w = i;
      }
    }

    return std::llround(std::log10(std::llround(
                            std::fmod(absv * std::pow(10.0, w), kRangeMax))) +
                        1) +
           shift;
  }

  static auto ShiftChars(std::string const &chars, const int pow)
      -> std::string {
    const auto signbit = chars.front() == '-';
    auto s = (signbit && pow != 0) ? chars.substr(1) : chars;

    if (!s.empty() && pow != 0) {
      if (s.find_first_of("123456789") == std::string::npos) {  // if all zero
        s = "0";
      } else {
        // add dot if necessary
        auto add_dot_if = [&]() -> void {
          if (auto pos = s.find_first_of('.'); pos == std::string::npos)
            s.append(1, '.');
        };

        // add leading or trailing zeros if necessary
        auto add_zeros_if = [&]() -> void {
          if (pow > 0) {
            auto nz =
                pow - static_cast<int>(s.size() - s.find_first_of('.') - 1);

            if (nz > 0) s.append(nz, '0');
          } else if (pow < 0) {
            auto nz = -pow - static_cast<int>(s.find_first_of('.')) + 1;

            if (nz > 0) s = std::string(nz, '0') + s;
          }
        };

        // swap dot if necessary
        auto swap_dot_if = [&]() -> void {
          // swap dot
          auto pos = s.find_first_of('.');
          auto n = pow;

          if (pow > 0) {
            while (n-- > 0) {
              std::swap(s[pos], s[pos + 1]);
              ++pos;
            }
          } else if (pow < 0) {
            while (n++ < 0) {
              std::swap(s[pos], s[pos - 1]);
              --pos;
            }
          }
        };

        // remove leading zeros if necessary
        auto remove_leading_zeros_if = [&]() {
          auto dotp = s.find_first_of('.');
          auto nump = s.find_first_of("123456789");

          if (dotp > nump)
            s.erase(0, nump);
          else if (dotp > 1)
            s.erase(0, dotp - 1);
        };

        // removing trailing zeros if necessary
        auto remove_trailing_zeros_if = [&]() {
          auto odotp = chars.find_first_of('.');
          auto ndotp = s.find_first_of('.');
          auto ofrac = odotp == std::string::npos
                           ? 0
                           : static_cast<int>(chars.size() - odotp - 1);
          auto nfrac = ndotp == std::string::npos
                           ? 0
                           : static_cast<int>(s.size() - ndotp - 1);
          auto n = nfrac - ofrac;

          if (n > 0) {
            if (ofrac == 0)
              s.erase(s.find_last_not_of('0') + 1);
            else
              s.erase(std::max(s.find_last_not_of('0') + 1, s.size() - n));
          }
        };

        // remove dot if
        auto remove_dot_if = [&]() -> void {
          if (s.back() == '.') s.pop_back();
        };

        // add signbit back
        auto add_signbit_if = [&]() -> void {
          if (signbit) s = "-" + s;
        };

        add_dot_if();
        add_zeros_if();
        swap_dot_if();
        remove_leading_zeros_if();
        remove_trailing_zeros_if();
        remove_dot_if();
        add_signbit_if();
      }
    }

    return s;
  }

  static auto ToChars(double value) -> std::string {
    auto s = std::string();

#ifdef _DECIMAL_USE_STD_TO_CHARS
    auto [v, p] = ShiftValue(value);
    auto b = std::array<char, 128>();
    auto [ptr, ec] = std::to_chars(b.data(), b.data() + b.size(), v);

    if (ec == std::errc{}) s = std::string(b.data(), ptr);

    s = ShiftChars(s, -p);
#else
    auto ss = std::stringstream();

    if (std::signbit(value)) ss << "-";

    ss << std::fixed << std::setprecision(MaxDigits(value)) << std::abs(value);

    s = ss.str();

    if (auto nzp = s.find_last_not_of("0-"); nzp != std::string::npos)
      s = s.substr(0, s.find_last_not_of('0') + 1);

    if (s.back() == '.') s.pop_back();
#endif  // _DECIMAL_USE_STD_TO_CHARS

    return s;
  }

  static auto GetDecimalSize(double value) -> int {
    return GetDecimalSize(ToChars(value));
  }

  static auto GetIntegeralPartSize(double value) -> int {
    return GetIntegeralPartSize(ToChars(value));
  }

  static auto GetFractionalPartSize(double value) -> int {
    return GetFractionalPartSize(ToChars(value));
  }

  static auto GetPrecision(double value) -> int {
    return GetFractionalPartSize(value);
  }

  static auto ToValue(double value, double interval) -> double {
    auto vi = value / interval;

    return std::isnormal(vi) ? std::nearbyint(value / interval) * interval
                             : value;
  }

  static auto ToChars(double value, double interval) -> std::string {
    auto s = ToChars(ToValue(value, interval));

    if (auto nz = GetPrecision(interval) - GetFractionalPartSize(s); nz > 0) {
      if (s.find_first_of('.') == std::string::npos) s.append(1, '.');
      s.append(nz, '0');
    }

    return s;
  }

  static auto FromChars(std::string_view str) -> double {
    auto value = 0.0;

#ifdef _DECIMAL_USE_STD_TO_CHARS
    auto [ptr, ec] =
        std::from_chars(str.data(), str.data() + str.size(), value);

    if (ec != std::errc()) value = std::numeric_limits<double>::quiet_NaN();
#else
    try {
      value = std::stod({str.data(), str.size()});
    } catch (...) {
      value = std::numeric_limits<double>::quiet_NaN();
    }
#endif  // _DECIMAL_USE_STD_TO_CHARS

    return value;
  }

 public:
  enum class Parts {
    kFraction,
    kInteger,
    kDecimal,
  };

  explicit Decimal(double value, double interval = 0)
      : interval_(interval), value_(value) {}

  explicit Decimal(std::string_view str, double interval = 0)
      : interval_(interval), value_(FromChars(str)) {
    if (interval_ == 0) {
      auto fsz = size(Parts::kFraction);

      if (fsz == 0) {
        auto isz = size(Parts::kInteger);

        if (isz > 0) interval_ = std::pow(10.0, isz - 1);
      } else {
        interval_ = std::pow(10.0, -fsz);
      }
    }
  }

  auto add(const Decimal &rhs) -> Decimal & {
    value_ += rhs.value();
    return *this;
  }

  auto minus(const Decimal &rhs) -> Decimal & {
    value_ -= rhs.value();
    return *this;
  }

  auto multiple(const Decimal &rhs) -> Decimal & {
    value_ *= rhs.value();
    return *this;
  }

  auto divide(const Decimal &rhs) -> Decimal & {
    value_ /= rhs.value();
    return *this;
  }

  auto operator+=(const Decimal &rhs) -> Decimal & { return add(rhs); }

  auto operator-=(const Decimal &rhs) -> Decimal & { return minus(rhs); }

  auto operator*=(const Decimal &rhs) -> Decimal & { return multiple(rhs); }

  auto operator/=(const Decimal &rhs) -> Decimal & { return divide(rhs); }

  auto size(Parts const part = Parts::kFraction) const -> int {
    if (part == Parts::kFraction)
      return GetFractionalPartSize(value_);
    else if (part == Parts::kInteger)
      return GetIntegeralPartSize(value_);
    else
      return GetDecimalSize(value_);
  }

  auto value() const -> double { return ToValue(value_, interval_); }

  auto str() const -> std::string { return ToChars(value_, interval_); }

  auto interval() const -> double { return interval_; }
};

static inline auto operator==(const Decimal &lhs, const Decimal &rhs) -> bool {
  return lhs.value() == rhs.value();
}

static inline auto operator<(const Decimal &lhs, const Decimal &rhs) -> bool {
  return lhs.value() < rhs.value();
}

static inline auto operator>=(const Decimal &lhs, const Decimal &rhs) -> bool {
  return !(lhs.value() < rhs.value());
}

static inline auto operator+(const Decimal &lhs, const Decimal &rhs)
    -> const Decimal {
  return Decimal(lhs) += rhs;
}

static inline auto operator-(const Decimal &lhs, const Decimal &rhs)
    -> const Decimal {
  return Decimal(lhs) -= rhs;
}

static inline auto operator*(const Decimal &lhs, const Decimal &rhs)
    -> const Decimal {
  return Decimal(lhs) *= rhs;
}

static inline auto operator/(const Decimal &lhs, const Decimal &rhs)
    -> const Decimal {
  return Decimal(lhs) /= rhs;
}
