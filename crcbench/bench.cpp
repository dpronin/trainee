#include <cstdint>

#include <algorithm>
#include <random>
#include <vector>

#include <benchmark/benchmark.h>

#include <boost/crc.hpp>

#include "crc32c.h"

namespace
{

constexpr unsigned long long operator"" _KB(unsigned long long x) noexcept { return x << 10; }
constexpr unsigned long long operator"" _MB(unsigned long long x) noexcept { return x << 20; }
constexpr unsigned long long operator"" _GB(unsigned long long x) noexcept { return x << 30; }
constexpr unsigned long long operator"" _TB(unsigned long long x) noexcept { return x << 40; }
constexpr unsigned long long operator"" _PB(unsigned long long x) noexcept { return x << 50; }

std::vector<uint8_t> generate_buffer(size_t sz)
{
    std::vector<uint8_t> buf(sz);
    std::random_device d;
    std::default_random_engine generator{d()};
    std::generate(buf.begin(), buf.end(), generator);
    return buf;
}

void run_crc32c_bench(benchmark::State& state) {
    auto const buf = generate_buffer(state.range(0));
    decltype(crc32c(0u, buf.data(), buf.size())) crc = 0;
    for (auto _ : state) {
        crc = crc32c(crc, buf.data(), buf.size());
        benchmark::DoNotOptimize(crc);
    }
}

void BM_CRC32COptimal(benchmark::State& state) {
    crc32c_init();
    run_crc32c_bench(state);
}

void BM_CRC32CSoftware(benchmark::State& state) {
    crc32c_init_sw();
    run_crc32c_bench(state);
}

void BM_BoostCRC32Optimal(benchmark::State& state)
{
    // CRC-32/ISCSI
    using crc_type = boost::crc_optimal<32, 0x1edc6f41, 0xffffffff, 0xffffffff, true, true>;
    crc_type calculator;
    auto const buf = generate_buffer(state.range(0));
    decltype(calculator.checksum()) crc = 0;
    for (auto _ : state) {
        calculator.process_bytes(buf.data(), buf.size());
        crc = calculator.checksum();
        benchmark::DoNotOptimize(crc);
    }
}

} // anonymous namespace

BENCHMARK(BM_CRC32COptimal)->RangeMultiplier(2)->Range(1_MB, 1_GB)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_CRC32CSoftware)->RangeMultiplier(2)->Range(1_MB, 1_GB)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_BoostCRC32Optimal)->RangeMultiplier(2)->Range(1_MB, 1_GB)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
