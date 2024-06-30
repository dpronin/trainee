#include <cassert>
#include <cstdint>
#include <cstdlib>

#include <algorithm>
#include <chrono>
#include <print>
#include <queue>
#include <vector>

#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/system/detail/error_code.hpp>

using std::chrono_literals::operator""s;

namespace {

boost::asio::io_context io_ctx;

std::chrono::seconds incoming_period;
boost::asio::steady_timer incoming_timer{io_ctx};

std::chrono::seconds processing_period;
std::vector<std::unique_ptr<boost::asio::steady_timer>> processors;
std::queue<
    std::pair<uint32_t, std::chrono::time_point<std::chrono::system_clock>>>
    frames_pending;

void frame_processed(
    uint32_t frame_nr,
    std::chrono::time_point<std::chrono::system_clock> frame_incoming_time,
    uint32_t processor_slot_nr, boost::system::error_code const &ec);

void frame_submit(
    uint32_t frame_nr,
    std::chrono::time_point<std::chrono::system_clock> frame_incoming_time,
    size_t processor_slot_nr) {
  assert(processor_slot_nr < processors.size());
  assert(nullptr == processors[processor_slot_nr]);

  auto const frame_submit_time{std::chrono::system_clock::now()};

  std::println("frame #{}: submit, slot {}, time point {}, delay {}", frame_nr,
               processor_slot_nr, frame_submit_time,
               std::chrono::duration_cast<std::chrono::milliseconds>(
                   frame_submit_time - frame_incoming_time));

  auto processing_timer{std::make_unique<boost::asio::steady_timer>(io_ctx)};

  processing_timer->expires_after(processing_period);
  processing_timer->async_wait(
      [frame_nr = frame_nr, frame_incoming_time = frame_incoming_time,
       slot_nr = processor_slot_nr](boost::system::error_code const &ec) {
        frame_processed(frame_nr, frame_incoming_time, slot_nr, ec);
      });

  processors[processor_slot_nr] = std::move(processing_timer);
}

void frame_processed(
    uint32_t frame_nr,
    std::chrono::time_point<std::chrono::system_clock> frame_incoming_time,
    uint32_t processor_slot_nr, boost::system::error_code const &ec) {
  if (ec)
    return;

  auto const frame_processed_time{std::chrono::system_clock::now()};

  std::println("frame #{}: processed, slot {}, time point {}, delay {}",
               frame_nr, processor_slot_nr, frame_processed_time,
               std::chrono::duration_cast<std::chrono::milliseconds>(
                   frame_processed_time - frame_incoming_time));

  assert(processor_slot_nr < processors.size());
  assert(nullptr != processors[processor_slot_nr]);

  processors[processor_slot_nr] = {};

  while (!frames_pending.empty()) {
    if (auto pit = std::ranges::find_if(
            processors,
            [](auto const &processing_timer) { return !processing_timer; });
        processors.end() != pit) {
      frame_submit(frames_pending.front().first, frames_pending.front().second,
                   pit - processors.begin());
      frames_pending.pop();
    } else {
      break;
    }
  }
}

void frame_receive(boost::system::error_code const &ec) {
  static uint32_t frame_nr{0};

  if (ec)
    return;

  auto const frame_incoming_time{std::chrono::system_clock::now()};

  std::println("frame #{}: received, time point {}", frame_nr,
               frame_incoming_time);

  if (auto pit = std::ranges::find_if(
          processors,
          [](auto const &processing_timer) { return !processing_timer; });
      processors.end() != pit) {
    frame_submit(frame_nr, frame_incoming_time, pit - processors.begin());
  } else {
    frames_pending.push({frame_nr, frame_incoming_time});
    std::println("frame #{}: placed into pending queue, "
                 "pending queue size #{}, time point {}",
                 frame_nr, frames_pending.size(), frame_incoming_time);
  }

  ++frame_nr;

  incoming_timer.expires_after(incoming_period);
  incoming_timer.async_wait(frame_receive);
}

} // namespace

/* Example: ./pipeline_model 3 8 3
 *
 * OR
 *
 * Example: ./pipeline_model 3 10 3
 *
 * @argv[1]: frame incoming period in seconds, i.e. period of time a new frame
 * is received
 * @argv[2]: frame processing period in seconds, i.e. period of time a frame is
 * being processed and holding a slot of a processor that the frame is occupying
 * @argv[3]: number of processors possible to use
 *
 */

int main(int argc, char const *argv[]) {
  if (argc < 4)
    return EXIT_FAILURE;

  incoming_period =
      std::chrono::seconds{boost::lexical_cast<uint32_t>(argv[1])};
  processing_period =
      std::chrono::seconds{boost::lexical_cast<uint32_t>(argv[2])};
  processors.resize(boost::lexical_cast<int32_t>(argv[3]));

  incoming_timer.expires_at(std::chrono::steady_clock::now());
  incoming_timer.async_wait(frame_receive);

  io_ctx.run();

  return 0;
}
