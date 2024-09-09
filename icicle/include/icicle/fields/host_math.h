// Note: this optimization generates invalid code (using gcc) when storage class has a union for both u32 and u64 so disabling it.
// #if defined(__GNUC__) && !defined(__NVCC__) && !defined(__clang__)
//   #pragma GCC optimize("no-strict-aliasing")
// #endif

#pragma once

#include <cstdint>
#include <cstring> 
#include <bit>
#include "icicle/utils/modifiers.h"
#include "icicle/fields/storage.h"
namespace host_math {

  // return x + y with T operands
  template <typename T>
  static constexpr __host__ T add(const T x, const T y)
  {
    return x + y;
  }

  // return x + y + carry with T operands
  template <typename T>
  static constexpr __host__ T addc(const T x, const T y, const bool carry)
  {
    return x + y + (carry? 1 : 0);
  }

  // return x + y and carry out with T operands
  template <typename T>
  static constexpr __host__ T add_cc(const T x, const T y, bool& carry)
  {
    T result = x + y;
    carry = x > result;
    return result;
  }

  // return x + y + carry and carry out  with T operands
  template <typename T>
  static constexpr __host__ T addc_cc(const T x, const T y, bool& carry)
  {
    const T result = x + y + (carry? 1 : 0);
    carry = carry && x >= result || !carry && x > result;
    return result;
  }

  // return x - y with T operands
  template <typename T>
  static constexpr __host__ T sub(const T x, const T y)
  {
    return x - y;
  }

  //    return x - y - borrow with T operands
  template <typename T>
  static constexpr __host__ T subc(const T x, const T y, const bool borrow)
  {
    return x - y - (borrow? 1 : 0);
  }

  //    return x - y and borrow out with T operands
  template <typename T>
  static constexpr __host__ T sub_cc(const T x, const T y, bool& borrow)
  {
    T result = x - y;
    borrow = x < result;
    return result;
  }

  //    return x - y - borrow and borrow out with T operands
  template <typename T>
  static constexpr __host__ T subc_cc(const T x, const T y, bool& borrow)
  {
    const T result = x - y - (borrow? 1 : 0);
    borrow = borrow && x <= result || !borrow && x < result;
    return result;
  }

  // return x * y + z + carry and carry out with uint32_t operands
  static constexpr __host__ uint32_t madc_cc(const uint32_t x, const uint32_t y, const uint32_t z, uint32_t& carry)
  {
    uint64_t r = static_cast<uint64_t>(x) * y + z + carry;
    carry = (uint32_t)(r >> 32);
    uint32_t result = r & 0xffffffff;
    return result;
  }

  static constexpr __host__ uint64_t madc_cc_64(const uint64_t x, const uint64_t y, const uint64_t z, uint64_t& carry)
  {
    __uint128_t r = static_cast<__uint128_t>(x) * y + z + carry;
    carry = (uint64_t)(r >> 64);
    uint64_t result = r & 0xffffffffffffffff;
    return result;
  }

  template <unsigned OPS_COUNT = UINT32_MAX, bool CARRY_IN = false, bool CARRY_OUT = false>
  struct carry_chain {
    unsigned index;

    constexpr HOST_INLINE carry_chain() : index(0) {}

    template <typename T>
    constexpr HOST_INLINE T add(const T x, const T y, bool& carry)
    {
      index++;
      if (index == 1 && OPS_COUNT == 1 && !CARRY_IN && !CARRY_OUT)
        return host_math::add(x, y);
      else if (index == 1 && !CARRY_IN)
        return host_math::add_cc(x, y, carry);
      else if (index < OPS_COUNT || CARRY_OUT)
        return host_math::addc_cc(x, y, carry);
      else
        return host_math::addc(x, y, carry);
    }

    template <typename T>
    constexpr HOST_INLINE T sub(const T x, const T y, bool& carry)
    {
      index++;
      if (index == 1 && OPS_COUNT == 1 && !CARRY_IN && !CARRY_OUT)
        return host_math::sub(x, y);
      else if (index == 1 && !CARRY_IN)
        return host_math::sub_cc(x, y, carry);
      else if (index < OPS_COUNT || CARRY_OUT)
        return host_math::subc_cc(x, y, carry);
      else
        return host_math::subc(x, y, carry);
    }
  };

  template <unsigned NLIMBS, bool SUBTRACT, bool CARRY_OUT>
  static constexpr HOST_INLINE bool add_sub_limbs_32(const uint32_t* x, const uint32_t* y, uint32_t* r)
  {
    // printf("NLIMBS %u\n", NLIMBS);
    bool carry = false;
    carry_chain<NLIMBS, false, CARRY_OUT> chain;
    for (unsigned i = 0; i < NLIMBS; i++)
      r[i] = SUBTRACT ? chain.sub(x[i], y[i], carry) : chain.add(x[i], y[i], carry);
      // printf("i %u xi %u yi %u r %u\n", i, x[i],y[i],r[i]);}
    // printf("carry %d\n",carry);
    return CARRY_OUT ? carry : false;
  }

  template <unsigned NLIMBS, bool SUBTRACT, bool CARRY_OUT>
  static constexpr HOST_INLINE bool add_sub_limbs_64(const uint64_t* x, const uint64_t* y, uint64_t* r)
  {
    bool carry = 0;
    carry_chain<NLIMBS, false, CARRY_OUT> chain;
    for (unsigned i = 0; i < NLIMBS / 2; i++)
      r[i] = SUBTRACT ? chain.sub(x[i], y[i], carry) : chain.add(x[i], y[i], carry);
    return CARRY_OUT ? carry : 0;
  }

  template <
    unsigned NLIMBS,
    bool SUBTRACT,
    bool CARRY_OUT,
    bool USE_32 = true> // for now we use only the 32 add/sub because the casting of the carry causes problems when
                        // compiling in release. to solve this we need to entirely split the field functions between a
                        // host version and a device version.
  static constexpr HOST_INLINE bool // 32 is enough for the carry
  add_sub_limbs(const storage<NLIMBS>& xs, const storage<NLIMBS>& ys, storage<NLIMBS>& rs)
  {
    if constexpr (USE_32 || NLIMBS < 2) {
      // uint32_t x[NLIMBS/4];
      // uint32_t y[NLIMBS/4];
      // uint32_t r[NLIMBS/4];
      // std::memcpy(x, xs.bytes, sizeof(xs.bytes));
      // std::memcpy(y, ys.bytes, sizeof(ys.bytes));
      // std::memcpy(r, rs.bytes, sizeof(rs.bytes));
      // const uint32_t* x = reinterpret_cast<const uint32_t*>(xs.bytes);
      // const uint32_t* y = reinterpret_cast<const uint32_t*>(ys.bytes);
      // uint32_t* r = reinterpret_cast<uint32_t*>(rs.bytes);
      // const uint32_t* x = std::bit_cast<const uint32_t*>(xs.bytes);
      // const uint32_t* y = bit_cast<const uint32_t*>(ys.bytes);
      // uint32_t* r = std::bit_cast<uint32_t*>(rs.bytes);
      return add_sub_limbs_32<NLIMBS/4, SUBTRACT, CARRY_OUT>(reinterpret_cast<const uint32_t*>(xs.bytes),
      reinterpret_cast<const uint32_t*>(ys.bytes), reinterpret_cast<uint32_t*>(rs.bytes));
      // return add_sub_limbs_32<NLIMBS/4, SUBTRACT, CARRY_OUT>(x, y, r);
      // bool carry = add_sub_limbs_32<NLIMBS/4, SUBTRACT, CARRY_OUT>(x, y, r);
      // std::memcpy(rs.bytes, r, sizeof(rs.bytes));
      // return carry;
    } else {
      const uint64_t* x = xs.limbs64;
      const uint64_t* y = ys.limbs64;
      uint64_t* r = rs.limbs64;
      // Note: returns uint64 but uint 32 is enough.
      uint64_t result = add_sub_limbs_64<NLIMBS/4, SUBTRACT, CARRY_OUT>(x, y, r);
      uint32_t carry = result == 1;
      return carry;
    }
  }

  template <unsigned NLIMBS_A, unsigned NLIMBS_B = NLIMBS_A>
  static constexpr HOST_INLINE void
  multiply_raw_32(const storage<NLIMBS_A*4>& as, const storage<NLIMBS_B*4>& bs, storage<NLIMBS_A*4 + NLIMBS_B*4>& rs)
  {
    // printf("NLIMBS_A %u\n", NLIMBS_A);
    // printf("NLIMBS_B %u\n", NLIMBS_B);
    const uint32_t* a = reinterpret_cast<const uint32_t*>(as.bytes);
    const uint32_t* b = reinterpret_cast<const uint32_t*>(bs.bytes);
    uint32_t* r = reinterpret_cast<uint32_t*>(rs.bytes);
    // uint32_t a[NLIMBS_A];
    // uint32_t b[NLIMBS_B];
    // uint32_t r[NLIMBS_A+NLIMBS_B];
    // std::memcpy(a, as.bytes, sizeof(as.bytes));
    // std::memcpy(b, bs.bytes, sizeof(bs.bytes));
    // std::memcpy(r, rs.bytes, sizeof(rs.bytes));
    for (unsigned i = 0; i < NLIMBS_B; i++) {
      uint32_t carry = 0;
      for (unsigned j = 0; j < NLIMBS_A; j++)
        r[j + i] = host_math::madc_cc(a[j], b[i], r[j + i], carry);
        // printf("i %u ai %u bi %u ri %u\n", i, a[i],b[i],r[i+j]);}
      r[NLIMBS_A + i] = carry;
    }
    // std::memcpy(rs.bytes, r, sizeof(rs.bytes));
  }

  template <unsigned NLIMBS_A, unsigned NLIMBS_B = NLIMBS_A>
  static HOST_INLINE void multiply_raw_64(const uint64_t* a, const uint64_t* b, uint64_t* r)
  {
    for (unsigned i = 0; i < NLIMBS_B / 2; i++) {
      uint64_t carry = 0;
      for (unsigned j = 0; j < NLIMBS_A / 2; j++)
        r[j + i] = host_math::madc_cc_64(a[j], b[i], r[j + i], carry);
      r[NLIMBS_A / 2 + i] = carry;
    }
  }

  template <unsigned NLIMBS_A, unsigned NLIMBS_B = NLIMBS_A>
  static HOST_INLINE void
  multiply_raw_64(const storage<NLIMBS_A*4>& as, const storage<NLIMBS_B*4>& bs, storage<NLIMBS_A*4 + NLIMBS_B*4>& rs)
  {
    // const uint64_t* a = as.limbs64;
    // const uint64_t* b = bs.limbs64;
    // uint64_t* r = rs.limbs64;
    // const uint64_t* a = reinterpret_cast<const uint64_t*>(as.bytes);
    // const uint64_t* b = reinterpret_cast<const uint64_t*>(bs.bytes);
    // uint64_t* r = reinterpret_cast<uint64_t*>(rs.bytes);
    // uint64_t a[NLIMBS_A/2];
    // uint64_t b[NLIMBS_B/2];
    // uint64_t r[(NLIMBS_A+NLIMBS_B)/2];
    // std::memcpy(a, as.bytes, sizeof(as.bytes));
    // std::memcpy(b, bs.bytes, sizeof(bs.bytes));
    // std::memcpy(r, rs.bytes, sizeof(rs.bytes));
    multiply_raw_64<NLIMBS_A, NLIMBS_B>(reinterpret_cast<const uint64_t*>(as.bytes), reinterpret_cast<const uint64_t*>(bs.bytes), reinterpret_cast<uint64_t*>(rs.bytes));
    // multiply_raw_64<NLIMBS_A, NLIMBS_B>(a, b, r);
    // std::memcpy(rs.bytes, r, sizeof(rs.bytes));
  }

  template <unsigned NLIMBS_A, unsigned NLIMBS_B = NLIMBS_A, bool USE_32 = false>
  static constexpr HOST_INLINE void
  multiply_raw(const storage<NLIMBS_A>& as, const storage<NLIMBS_B>& bs, storage<NLIMBS_A + NLIMBS_B>& rs)
  {
    static_assert(
      ((NLIMBS_A/4) % 2 == 0 || (NLIMBS_A/4) == 1) && ((NLIMBS_B/4) % 2 == 0 || (NLIMBS_B/4) == 1),
      "odd number of limbs is not supported\n");
    if constexpr (USE_32) {
      multiply_raw_32<(NLIMBS_A/4), (NLIMBS_B/4)>(as, bs, rs);
      return;
    } else if constexpr (((NLIMBS_A/4) == 1 && (NLIMBS_B/4) == 2) || ((NLIMBS_A/4) == 2 && (NLIMBS_B/4) == 1)) {
      multiply_raw_32<(NLIMBS_A/4), (NLIMBS_B/4)>(as, bs, rs);
      return;
    } else if constexpr ((NLIMBS_A/4) == 1 && (NLIMBS_B/4) == 1) {
      rs.limbs[1] = 0;
      rs.limbs[0] = host_math::madc_cc(as.limbs[0], bs.limbs[0], 0, rs.limbs[1]);
      return;
    } else if constexpr ((NLIMBS_A/4) == 2 && (NLIMBS_B/4) == 2) {
      const uint64_t* a = as.limbs64; // nof limbs should be even
      const uint64_t* b = bs.limbs64;
      uint64_t* r = rs.limbs64;
      r[1] = 0;
      r[0] = host_math::madc_cc_64(a[0], b[0], 0, r[1]);
      return;
    } else {
      multiply_raw_64<(NLIMBS_A/4), (NLIMBS_B/4)>(as, bs, rs);
    }
  }

  template <unsigned NLIMBS, unsigned BITS>
  static constexpr HOST_INLINE storage<NLIMBS> left_shift(const storage<NLIMBS>& xs)
  {
    if constexpr (BITS == 0)
      return xs;
    else {
      constexpr unsigned BITS8 = BITS % 8;
      constexpr unsigned LIMBS_GAP = BITS / 8;
      storage<NLIMBS> out{};
      // const uint32_t* xs_limbs32 = reinterpret_cast<const uint32_t*>(xs.bytes);
      // uint32_t* out_limbs32 = reinterpret_cast<uint32_t*>(out.bytes);
      if constexpr (LIMBS_GAP < NLIMBS) {
        out.bytes[LIMBS_GAP] = xs.bytes[0] << BITS8;
        for (unsigned i = 1; i < NLIMBS - LIMBS_GAP; i++)
          out.bytes[i + LIMBS_GAP] = (xs.bytes[i] << BITS8) | (xs.bytes[i - 1] >> (8 - BITS8));
      }
      return out;
    }
  }

  template <unsigned NLIMBS, unsigned BITS>
  static constexpr HOST_INLINE storage<NLIMBS> right_shift(const storage<NLIMBS>& xs)
  {
    if constexpr (BITS == 0)
      return xs;
    else {
      constexpr unsigned BITS8 = BITS % 8;
      constexpr unsigned LIMBS_GAP = BITS / 8;
      storage<NLIMBS> out{};
      if constexpr (LIMBS_GAP < NLIMBS - 1) {
        for (unsigned i = 0; i < NLIMBS - LIMBS_GAP - 1; i++)
          out.bytes[i] = (xs.bytes[i + LIMBS_GAP] >> BITS8) | (xs.bytes[i + LIMBS_GAP + 1] << (8 - BITS8));
      }
      if constexpr (LIMBS_GAP < NLIMBS) out.bytes[NLIMBS - LIMBS_GAP - 1] = (xs.bytes[NLIMBS - 1] >> BITS8);
      return out;
    }
  }

  template <
    unsigned NLIMBS_NUM,
    unsigned NLIMBS_DENOM,
    unsigned NLIMBS_Q = (NLIMBS_NUM - NLIMBS_DENOM),
    bool USE_32 = false>
  static constexpr HOST_INLINE void integer_division(
    const storage<NLIMBS_NUM>& num, const storage<NLIMBS_DENOM>& denom, storage<NLIMBS_Q>& q, storage<NLIMBS_DENOM>& r)
  {
    storage<NLIMBS_DENOM> temp = {};
    for (int limb_idx = NLIMBS_NUM - 1; limb_idx >= 0; limb_idx--) {
      for (int bit_idx = 31; bit_idx >= 0; bit_idx--) {
        r = left_shift<NLIMBS_DENOM, 1>(r);
        r.limbs[0] |= ((num.limbs[limb_idx] >> bit_idx) & 1);
        bool c = add_sub_limbs<NLIMBS_DENOM, true, true, USE_32>(r, denom, temp);
        if (limb_idx < NLIMBS_Q & !c) {
          r = temp;
          q.limbs[limb_idx] |= 1 << bit_idx;
        }
      }
    }
  }
} // namespace host_math

// #if defined(__GNUC__) && !defined(__NVCC__) && !defined(__clang__)
//   #pragma GCC reset_options
// #endif
