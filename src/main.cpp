#include <iostream>
#include <windows.h>
#include <thread>
#include <chrono>

#include "SwapBuffer.h"

typedef struct DATA_T {
  int val1 = 0;
  float val2 = 0.0f;
  int v[5] = {100, 200, 300, 400, 500};
} data_t;

float getDeltaTime() {
  static std::chrono::time_point<std::chrono::steady_clock> t1, t0 = std::chrono::high_resolution_clock::now();
  t1 = std::chrono::high_resolution_clock::now();
  float delta = (t1 - t0).count() * 1e-6f;  // ms
  t0 = t1;
  return delta;
}

void threadWrite(SwapBuffer<data_t>* buf) {
  while (true) {
    // pegar buffer
    data_t* data = buf->getWriteBuffer();
    // escrever dados
    Sleep(500);
    data->val1++;
    data->val2 += 0.1f;
    for (int i = 0; i < 5; i++) data->v[i]--;
    // sinalizar final da escrita
    buf->finishWrite();
  }
}

void threadRead(SwapBuffer<data_t>* buf) {
  while (true) {
    // pegar buffer
    data_t* data = buf->getReadBuffer();
    // ler dados
    Sleep(1200);
    std::cout << "val1=" << data->val1 << "\t" << std::ends;
    std::cout << "val2=" << ++(data->val2) << "\t" << std::ends;
    std::cout << "v=[" << std::ends;
    for (int i = 0; i < 5; i++) std::cout << data->v[i] << "\t" << std::ends;
    std::cout << "]" << std::ends;
    std::cout << " tempo=" << getDeltaTime() << std::endl;
    // sinalizar final da leitura do buffer
    buf->finishRead();
  }
}

int main() {
  SwapBuffer<data_t> buf(true);

  getDeltaTime();  // iniciar timer

  std::thread th_r([&buf]() { threadRead(&buf); });
  std::thread th_w([&buf]() { threadWrite(&buf); });

  if (th_r.joinable()) th_r.join();
  if (th_w.joinable()) th_w.join();

  return EXIT_SUCCESS;
}