#pragma once

#define SWAPBUFFER_COUNT 2

template <typename T>
class SwapBuffer {
public:
  SwapBuffer(bool waitReadToSawpBuffers);
  ~SwapBuffer();

  T* getWriteBuffer();
  void finishWrite();

  T* getReadBuffer();
  void finishRead();

private:
  void swapBuffers();
  T* buf[SWAPBUFFER_COUNT];    // buffers
  int readIdx;                 // indice de leitura
  int writeIdx;                // indice de escrita
  bool readEnable;             // liberado para leitura
  bool swapEnable;             // liberado para trocar buffers
  bool waitReadToSawpBuffers;  // agaurdar liberacao para  troca de buffers
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <typename T>
inline SwapBuffer<T>::SwapBuffer(bool waitReadToSawpBuffers) {
  this->waitReadToSawpBuffers = waitReadToSawpBuffers;
  for (int i = 0; i < SWAPBUFFER_COUNT; i++) buf[i] = new T();
  writeIdx = 0;
  readEnable = false;
  swapEnable = true;
}

template <typename T>
inline SwapBuffer<T>::~SwapBuffer() {
  for (int i = 0; i < SWAPBUFFER_COUNT; i++) delete buf[i];
}

template <typename T>
inline T* SwapBuffer<T>::getWriteBuffer() {
  return buf[writeIdx];
}

template <typename T>
inline void SwapBuffer<T>::finishWrite() {
  if (waitReadToSawpBuffers) {
    while (!swapEnable)
      ;
    swapBuffers();
  } else {
    if (swapEnable) swapBuffers();
  }
}

template <typename T>
inline T* SwapBuffer<T>::getReadBuffer() {
  while (!readEnable)
    ;
  readEnable = false;
  return buf[readIdx];
}

template <typename T>
inline void SwapBuffer<T>::finishRead() {
  swapEnable = true;
}

template <typename T>
inline void SwapBuffer<T>::swapBuffers() {
  swapEnable = false;
  readIdx = writeIdx;
  writeIdx++;
  if (writeIdx >= SWAPBUFFER_COUNT) writeIdx = 0;
  readEnable = true;
}
