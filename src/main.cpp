
#include <algorithm>
#include <future>
#include <iostream>
#include <mutex>
#include <numeric>
#include <string>
#include <thread>
#include <vector>
#include <SDL2/SDL.h>

int main(int argc, char **argv) {
(void)argc;
(void)argv;
  SDL_SetMainReady();
  
  //SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);

  std::vector<std::future<void>> futures;
  for (size_t i = 0; i < 3; ++i) {
    futures.push_back(std::async(std::launch::deferred, [i]() {
      std::cout << i << ":" << std::endl;
      std::cout << "boop" << std::endl;
    }));
  }

  for (auto &fut : futures) {
    fut.wait();
  }

  SDL_Quit();

  return 0;
}