/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <cstdint>

#include <folly/CPortability.h>
#include <folly/lang/Bits.h>

namespace folly {
namespace hash {

namespace detail {

FOLLY_ALWAYS_INLINE constexpr std::uint64_t shiftMix(std::uint64_t v) {
  constexpr std::uint64_t kShift = 47;
  return v ^ (v >> kShift);
}

} // namespace detail

/*
 * Implementation of MurmurHash2 hashing algorithm for 64-bit
 * platforms.
 *
 * https://en.wikipedia.org/wiki/MurmurHash
 */
inline std::uint64_t murmurHash64(
    const char* key, std::size_t len, std::uint64_t seed) noexcept {
  constexpr std::uint64_t kMul = 0xc6a4a7935bd1e995UL;

  std::uint64_t hash = seed ^ (len * kMul);

  const char* beg = key;
  const char* end = beg + (len & ~0x7);

  for (const char* p = beg; p != end; p += sizeof(std::uint64_t)) {
    const std::uint64_t k = loadUnaligned<std::uint64_t>(p);
    hash = (hash ^ detail::shiftMix(k * kMul) * kMul) * kMul;
  }

  if ((len & 0x7) != 0) {
    const std::uint64_t k = partialLoadUnaligned<std::uint64_t>(end, len & 0x7);
    hash ^= k;
    hash *= kMul;
  }

  hash = detail::shiftMix(hash) * kMul;
  hash = detail::shiftMix(hash);

  return hash;
}

} // namespace hash
} // namespace folly
