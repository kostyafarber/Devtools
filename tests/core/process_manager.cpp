#include "core/process_manager.h"
#include <gtest/gtest.h>
#include <unistd.h>

using namespace core;

TEST(ProcessManager, CreateSimpleProcess)
{
  ProcessManager pm;
  auto ran = pm.run("helloProcess", "/bin/echo", {"hello world"});

  ASSERT_FALSE(ran.is_error());
}

TEST(ProcessManager, ValidateArguments)
{
  // should run process and if invalid argument should report error
  ProcessManager pm;

  auto ran = pm.run("helloProcess", "/bin/echo");
  ASSERT_FALSE(ran.is_error());

  ran = pm.run("invalidPath", "doesNot\0Exists");
  ASSERT_TRUE(ran.is_error());

  ran = pm.run("nonExistent", "doesNotExists");
  ASSERT_TRUE(ran.is_error());
}