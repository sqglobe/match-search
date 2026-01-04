#include "WorkerThread.h"

namespace gui {
WorkerThread::~WorkerThread() {
  quit();
  wait();
}
}  // namespace gui