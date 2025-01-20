#include <cstdint>

#include <algorithm>
#include <random>
#include <vector>

#include <boost/crc.hpp>

#include <benchmark/benchmark.h>

#include <xroost/crc/crc_optimal.hpp>

template <typename CRCAlgo, void (CRCAlgo::*f)(void const *, size_t)>
void CRCBench(benchmark::State &state) {
  std::vector<uint8_t> data;
  data.resize(state.range(0));
  std::random_device d;
  std::default_random_engine e{d()};
  std::generate(data.begin(), data.end(), e);

  for (auto _ : state) {
    CRCAlgo algo;
    (algo.*f)(data.data(), data.size());
    ::benchmark::ClobberMemory();
    decltype(algo.checksum()) crc;
    ::benchmark::DoNotOptimize(crc);
    crc = algo.checksum();
  }

  state.SetBytesProcessed(state.iterations() * data.size());
}

using crc1 = xroost::crc::crc_optimal<8, 0x7, 0, 0, true, true>;
using crc2 = xroost::crc::crc_optimal<16, 0x7, 0, 0, true, true>;
using crc3 = xroost::crc::crc_optimal<32, 0x7, 0, 0, true, true>;
using crc4 = xroost::crc::crc_optimal<64, 0x7, 0, 0, true, true>;
using crc5 = boost::crc_optimal<8, 0x7, 0, 0, true, true>;
using crc6 = boost::crc_optimal<16, 0x7, 0, 0, true, true>;
using crc7 = boost::crc_optimal<32, 0x7, 0, 0, true, true>;
using crc8 = boost::crc_optimal<64, 0x7, 0, 0, true, true>;

BENCHMARK_TEMPLATE2(CRCBench, crc1, &crc1::operator())
    ->RangeMultiplier(2)
    ->Range(2 << 20, 2 << 24)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE2(CRCBench, crc2, &crc2::operator())
    ->RangeMultiplier(2)
    ->Range(2 << 20, 2 << 24)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE2(CRCBench, crc3, &crc3::operator())
    ->RangeMultiplier(2)
    ->Range(2 << 20, 2 << 24)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE2(CRCBench, crc4, &crc4::operator())
    ->RangeMultiplier(2)
    ->Range(2 << 20, 2 << 24)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE2(CRCBench, crc5, &crc5::process_bytes)
    ->RangeMultiplier(2)
    ->Range(2 << 20, 2 << 24)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE2(CRCBench, crc6, &crc6::process_bytes)
    ->RangeMultiplier(2)
    ->Range(2 << 20, 2 << 24)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE2(CRCBench, crc7, &crc7::process_bytes)
    ->RangeMultiplier(2)
    ->Range(2 << 20, 2 << 24)
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE2(CRCBench, crc8, &crc8::process_bytes)
    ->RangeMultiplier(2)
    ->Range(2 << 20, 2 << 24)
    ->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
