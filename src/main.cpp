#include <iostream>
#include <windows.h>
#include <thread>
#include <chrono>

#include "SwapBuffer.h"

int globalCount = 0;
bool run = true;

typedef struct DATA_A_T {
  int val = 0;
} data_A_t;

typedef struct DATA_B_T {
  float val = 0.0f;
} data_B_t;

typedef struct DATA_C_T {
  double val = 0;
} data_C_t;

float getDeltaTime() {
  static std::chrono::time_point<std::chrono::steady_clock> t1, t0 = std::chrono::high_resolution_clock::now();
  t1 = std::chrono::high_resolution_clock::now();
  float delta = (t1 - t0).count() * 1e-6f;  // ms
  t0 = t1;
  return delta;
}

///////////////////////////////////////////////////////////////////////////////

void threadA(SwapBuffer<data_A_t>* out) {
  while (run) {
    data_A_t* dataOut = out->getWriteBuffer();
    Sleep(100);
    dataOut->val = globalCount;
    out->finishWrite();
  }
}

void threadB(SwapBuffer<data_A_t>* in, SwapBuffer<data_B_t>* out) {
  while (run) {
    data_A_t* dataIn = in->getReadBuffer();
    data_B_t* dataOut = out->getWriteBuffer();
    Sleep(100);
    dataOut->val = dataIn->val + 0.5f;
    in->finishRead();
    out->finishWrite();
  }
}

void threadC(SwapBuffer<data_B_t>* in, SwapBuffer<data_C_t>* out) {
  while (run) {
    data_B_t* dataIn = in->getReadBuffer();
    data_C_t* dataOut = out->getWriteBuffer();
    Sleep(100);
    dataOut->val = dataIn->val + 0.025;
    in->finishRead();
    out->finishWrite();
  }
}

void threadD(SwapBuffer<data_C_t>* in) {
  while (run) {
    data_C_t* dataIn = in->getReadBuffer();
    Sleep(100);
    std::cout << "val = " << dataIn->val << "\ttempo = " << getDeltaTime() << std::endl;
    if (GetKeyState(VK_ESCAPE) & 0x8000) run = false;
    globalCount++;
    in->finishRead();
  }
}

///////////////////////////////////////////////////////////////////////////////

int main() {
  SwapBuffer<data_A_t> bufAB(true);
  SwapBuffer<data_B_t> bufBC(true);
  SwapBuffer<data_C_t> bufCD(true);

  getDeltaTime();  // iniciar timer

  std::thread th_A([&bufAB]() { threadA(&bufAB); });
  std::thread th_B([&bufAB, &bufBC]() { threadB(&bufAB, &bufBC); });
  std::thread th_C([&bufBC, &bufCD]() { threadC(&bufBC, &bufCD); });
  std::thread th_D([&bufCD]() { threadD(&bufCD); });

  if (th_D.joinable()) th_D.join();
  bufAB.killThreads();
  bufBC.killThreads();
  bufAB.killThreads();
  if (th_C.joinable()) th_C.join();
  if (th_B.joinable()) th_B.join();
  if (th_A.joinable()) th_A.join();

  std::cout << "Final das threads" << std::endl;
  std::cout << "globalCount = " << globalCount << std::endl;

  return EXIT_SUCCESS;
}