#include "process_manager.h"
#include <format>
#include <sys/_types/_pid_t.h>
#include <sys/_types/_s_ifmt.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

namespace core
{

base::ErrorOr<void>
ProcessManager::m_validate_executable(const std::string &name)
{
  if (name.empty())
    return base::Error::from_string("executable name cannot be empty");

  struct stat sb;
  if (stat(name.c_str(), &sb) == -1)
    return base::Error::from_errno(
        format("error getting stats about: {}", name));

  // has execute
  if (!(sb.st_mode & S_IXUSR))
    return base::Error::from_string("no execute permissions");
  // is regular file
  if (!(sb.st_mode & S_IFREG))
    return base::Error::from_string("is not a regular file");

  return {};
}

base::ErrorOr<void> ProcessManager::run(const std::string &name,
                                        const std::string &executable,
                                        std::vector<std::string> args)
{
  if (auto validate = m_validate_executable(executable); validate.is_error())
    return validate;

  pid_t id = fork();

  // need to think about how to properly error out
  if (id == -1)
    return base::Error::from_errno("error forking");

  if (id == 0)
  {
    std::vector<char *> c_args;
    c_args.push_back(const_cast<char *>(executable.c_str()));

    for (const auto &arg : args)
    {
      c_args.push_back(const_cast<char *>(arg.c_str()));
    }
    c_args.push_back(nullptr);

    execvp(executable.c_str(), c_args.data());
    return base::Error::from_errno("error execvp");
  }

  Process p(name, executable, id, Process::ProcessState::Active, args);

  m_processes[id] = p;

  return {};
}

bool ProcessManager::is_running(const std::string &name)
{
  auto it = std::find_if(m_processes.begin(), m_processes.end(),
                         [&name](const auto &pair)
                         { return name == pair.second.m_name; });

  if (it == m_processes.end())
    return false;

  int status;
  pid_t result = waitpid(it->first, &status, WNOHANG);
  switch (result)
  {
  case 0:
    return true;

  case -1:
    return false;

  default:
    it->second.m_state = Process::ProcessState::Terminated;
    return false;
  }
}
} // namespace core