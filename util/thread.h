#pragma once
#include <future>
#include <thread>
#include <vector>

namespace util {

template<typename It, typename Func>
void for_each(It begin, It end, Func func)
{
  auto nThreads = std::thread::hardware_concurrency();
  if (!nThreads)
    nThreads = 1;

  std::vector<std::future<void>> futures(nThreads);

  auto nElems = std::distance(begin, end) / nThreads;

  for (auto& f : futures) {
    auto eRange = std::next(begin, nElems);
    f = async(std::launch::async, [=] {
      for (auto pos = begin; pos != eRange; ++pos)
        func(*pos);
    });
    begin = eRange;
  }

  while (begin != end)
    func(*begin++);

  for (auto& f : futures)
    f.get();
}

} // namespace util

