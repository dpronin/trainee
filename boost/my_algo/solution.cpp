#include <iostream>
#include <random>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <boost/timer/timer.hpp>

int singleNonDuplicate_On_simple_search(std::vector<int> const &nums) {
  if (nums.size() == 1) {
    return nums[0];
  }
  for (int i = 0; i < nums.size() - 1; i = i + 2) {
    if (nums[i] != nums[i + 1]) {
      return nums[i];
    }
  }
  return nums[nums.size() - 1];
}

int singleNonDuplicate_On_XOR(std::vector<int> const &nums) {
  int result = 0;
  for (auto cur : nums) {
    result ^= cur;
  }
  return result;
}

int singleNonDuplicate_Ologn_binary_search(std::vector<int> const &nums) {
  int first = 0;
  for (int last = nums.size(), mid = first + (last - first) / 2;
       last - first > 1; mid = first + (last - first) / 2) {
    if (nums[mid] == nums[mid + 1]) {
      // 'mid' index is even, go right
      if (0 == mid % 2)
        first = mid + 2;
      // 'mid' index is odd, go left
      else
        last = mid;
    } else if (nums[mid - 1] == nums[mid]) {
      // 'mid' index is even, go left
      if (0 == mid % 2)
        last = mid - 1;
      // 'mid' index is odd, go right
      else
        first = mid + 1;
    } else {
      first = mid;
      break;
    }
  }
  return nums[first];
}

std::vector<int> getNums(size_t qty) {
  std::vector<int> nums(qty % 2 ? qty : qty + 1);
  std::random_device d;
  std::default_random_engine e{d()};

  // int const random_number = e() % nums.size() / 2;
  int const random_number = nums.size() - 1;
  // std::cout << "random number is " << random_number << std::endl;

  for (int i = 0; i + 1 < random_number; i += 2) {
    nums[i + 1] = nums[i] = i;
  }

  nums[random_number] = random_number;

  for (int i = random_number + 1; i < nums.size() - 1; i += 2) {
    nums[i + 1] = nums[i] = i;
  }

  return nums;
}

int main(int argc, char const *argv[]) {
  auto cycles = boost::lexical_cast<size_t>(argv[2]);
  boost::timer::cpu_timer tm;

  auto nums = getNums(boost::lexical_cast<size_t>(argv[1]));
  std::cout << "vector is generated\n" << std::endl;
  // std::copy(nums.begin(), nums.end(), std::ostream_iterator<int>(std::cout,
  // ", ")); std::cout << std::endl;

  tm.start();
  for (int i = 0; i < cycles; ++i) {
    singleNonDuplicate_On_simple_search(nums);
  }
  std::cout << "O(n) algo simple search -> " << tm.format() << std::endl;

  tm.start();
  for (int i = 0; i < cycles; ++i) {
    singleNonDuplicate_On_XOR(nums);
  }
  std::cout << "O(n) algo XOR -> " << tm.format() << std::endl;

  tm.start();
  for (int i = 0; i < cycles; ++i) {
    singleNonDuplicate_Ologn_binary_search(nums);
  }
  std::cout << "O(log(n)) algo -> " << tm.format() << std::endl;

  return 0;
}
