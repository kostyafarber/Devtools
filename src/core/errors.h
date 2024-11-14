namespace core
{
enum class ExecutableError
{
  None,
  NotFound,
  NotExecutable,
  NoPermissions,
  InvalidPath,
  SystemError
};
}