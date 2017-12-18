#pragma once

#ifdef _WIN32
#define __LOGGER__FUNCTION_SIGNATURE__ __FUNCSIG__
#else
#define __LOGGER__FUNCTION_SIGNATURE__ __PRETTY_FUNCTION__
#endif


#define LOG_VERBOSE(str) ::Logger::log((str), (::Logger::VERBOSE), 0)
#define LOG_TRACE(str) ::Logger::log((str), (::Logger::TRACE), 0)
#define LOG_DEBUG(str) ::Logger::log((str), (::Logger::DEBUG), 0)
#define LOG_WARNING(str) ::Logger::log((str), (::Logger::WARNING), 0)
#define LOG_INFO(str) ::Logger::log((str), (::Logger::INFO), 0)
#define LOG_ERROR(str) ::Logger::log((str), (::Logger::_ERROR), 0)

#define LOG_FUNCTION(lvl) ::Logger::ScopeGuard logger_scope_guard((__LOGGER__FUNCTION_SIGNATURE__),(__FILE__),(__LINE__), (lvl))
#define LOG_FUNCTION_VERBOSE() ::Logger::ScopeGuard logger_scope_guard((__LOGGER__FUNCTION_SIGNATURE__),(__FILE__),(__LINE__), (::Logger::VERBOSE))
#define LOG_FUNCTION_TRACE()   ::Logger::ScopeGuard logger_scope_guard((__LOGGER__FUNCTION_SIGNATURE__),(__FILE__),(__LINE__), (::Logger::TRACE))
#define LOG_FUNCTION_DEBUG()   ::Logger::ScopeGuard logger_scope_guard((__LOGGER__FUNCTION_SIGNATURE__),(__FILE__),(__LINE__), (::Logger::DEBUG))
#define LOG_FUNCTION_INFO() ::Logger::ScopeGuard logger_scope_guard((__LOGGER__FUNCTION_SIGNATURE__),(__FILE__),(__LINE__), (::Logger::INFO))
#define LOG_FUNCTION_WARNING() ::Logger::ScopeGuard logger_scope_guard((__LOGGER__FUNCTION_SIGNATURE__),(__FILE__),(__LINE__), (::Logger::WARNING))
#define LOG_FUNCTION_ERROR()   ::Logger::ScopeGuard logger_scope_guard((__LOGGER__FUNCTION_SIGNATURE__),(__FILE__),(__LINE__), (::Logger::_ERROR))

#define LOG_SCOPE(str,lvl) ::Logger::ScopeGuard logger_scope_guard((str),(__FILE__),(__LINE__), (lvl))
#define LOG_SCOPE_VERBOSE(str) ::Logger::ScopeGuard logger_scope_guard((str),(__FILE__),(__LINE__), (::Logger::VERBOSE))
#define LOG_SCOPE_TRACE(str)   ::Logger::ScopeGuard logger_scope_guard((str),(__FILE__),(__LINE__), (::Logger::TRACE))
#define LOG_SCOPE_DEBUG(str)   ::Logger::ScopeGuard logger_scope_guard((str),(__FILE__),(__LINE__), (::Logger::DEBUG))
#define LOG_SCOPE_INFO(str) ::Logger::ScopeGuard logger_scope_guard((str),(__FILE__),(__LINE__), (::Logger::INFO))
#define LOG_SCOPE_WARNING(str) ::Logger::ScopeGuard logger_scope_guard((str),(__FILE__),(__LINE__), (::Logger::WARNING))
#define LOG_SCOPE_ERROR(str)   ::Logger::ScopeGuard logger_scope_guard((str),(__FILE__),(__LINE__), (::Logger::_ERROR))


#include <string>
#include <experimental/optional>

class Logger {
 public:
  enum LOG_LEVEL {
    _ERROR,
    WARNING,
    INFO,
    TRACE,
    DEBUG,
    VERBOSE,
  };


  static void init(std::experimental::optional<std::string> filename, LOG_LEVEL log_lvl, bool stderr_log=false);
  static void log(const std::string& log_info, const std::string& comment, LOG_LEVEL log_lvl, int indent_inc = 0);
  static void log(const std::string& log_info, LOG_LEVEL log_lvl, int indent_inc = 0);

  class ScopeGuard{
    const char* sign;
    LOG_LEVEL lvl;

   public:
    ScopeGuard(const char* _sign, const char* file, int line, LOG_LEVEL _lvl);
    ~ScopeGuard();
  };

 private:
  static Logger& instance();
  int indent=0;
  bool stderr_log;
  LOG_LEVEL log_level=_ERROR;
  std::experimental::optional<std::string> logfile;
  Logger(){};
};
