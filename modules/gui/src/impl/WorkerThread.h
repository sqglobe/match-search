#pragma once
#include <QThread>
namespace gui {
class WorkerThread : public QThread {
 public:
  using QThread::QThread;

 public:
  WorkerThread(const WorkerThread&) = delete;
  WorkerThread(WorkerThread&&) = delete;
  WorkerThread& operator=(const WorkerThread&) = delete;
  WorkerThread& operator=(WorkerThread&&) = delete;

 public:
  ~WorkerThread();
};
}  // namespace gui