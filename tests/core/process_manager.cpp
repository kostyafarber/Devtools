#include "core/process_manager.h"
#include <gtest/gtest.h>
#include <unistd.h>

TEST(ProcessManager, CreateSimpleProcess) {
  ProcessManager p;
  auto ran = p.run("sleepProcess", "echo", {"hello world"});
  ASSERT_EQ(ran, true);

  sleep(1);

  ASSERT_FALSE(p.is_running("sleepProcess"));
}
