#include "core/process_manager.h"
#include <gtest/gtest.h>
#include <unistd.h>

using namespace core;

TEST(Process, TriggerPlayError)
{
  Process p("audio", "/bin/echo", 1, Process::ProcessState::Terminated, {});

  ASSERT_TRUE(p.trigger_playing().is_error());
}

TEST(Process, TriggerPlay)
{
  Process p("audio", "/bin/echo", 1, Process::ProcessState::Active, {});

  p.trigger_playing();

  ASSERT_EQ(p.m_state, Process::ProcessState::Playing);
}

TEST(ProcessManager, CreateSimpleProcess)
{
  ProcessManager pm;
  auto ran = pm.run("helloProcess", "/bin/echo", {"hello world"});

  ASSERT_FALSE(ran.is_error());
}

TEST(ProcessManager, TriggerPlay)
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