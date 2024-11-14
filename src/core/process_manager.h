#pragma once
#include "errors.h"
#include <map>
#include <sys/_types/_pid_t.h>
#include <vector>

namespace core
{
// at the end of the day the process does something, how does it do it?
struct Process
{
  std::string m_name;
  std::string m_executable;
  pid_t m_process_id;
  std::vector<std::string> m_args;

  time_t m_timestamp;

  enum class ProcessState
  {
    Active,
    Terminated,
    Error
  };
  ProcessState m_state;

  Process() = default;
  Process(std::string name, std::string executable, pid_t process_id,
          ProcessState state, std::vector<std::string> args)
      : m_name(name), m_executable(executable), m_process_id(process_id),
        m_state(state), m_args(args)
  {
    time(&m_timestamp);
  };
};

class ProcessManager
{
private:
  //  perhaps we need something to manage all the processes internally
  // this will store all processes
  std::map<pid_t, Process> m_processes;
  ExecutableError m_validate_executable(const std::string &name);

public:
  ProcessManager() {};

  // destructor for cleaning up
  ~ProcessManager() {};

  // delete copy constructors
  ProcessManager(ProcessManager &&);
  ProcessManager &operator=(const ProcessManager &&);

  // only want move semantics because we don't know how long a process will live
  ProcessManager(const ProcessManager &) = delete;
  ProcessManager &operator=(const ProcessManager &) = delete;

  // maybe run should return something
  // run is returning a boolean
  ExecutableError run(const std::string &name, const std::string &path,
                      std::vector<std::string> args = {});
  bool is_running(const std::string &name);
  void stop(pid_t process_id);
};
} // namespace core