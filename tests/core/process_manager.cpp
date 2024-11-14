#include "core/process_manager.h"
#include <gtest/gtest.h>
#include <unistd.h>

using namespace core;

TEST(ProcessManager, CreateSimpleProcess)
{
  ProcessManager pm;
  auto ran = pm.run("helloProcess", "echo", {"hello world"});

  ASSERT_EQ(ran, ExecutableError::None);

  sleep(1);
}

TEST(ProcessManager, ValidateArguments)
{
  // should run process and if invalid argument should report error
  ProcessManager pm;

  auto ran = pm.run("helloProcess", "echo");
  ASSERT_EQ(ran, ExecutableError::None);

  ran = pm.run("invalidPath", "doesNot\0Exists");
  ASSERT_EQ(ran, ExecutableError::InvalidPath);

  ran = pm.run("nonExistent", "doesNotExists");
  ASSERT_EQ(ran, ExecutableError::NotFound);
}