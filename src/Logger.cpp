#include "json_input_app/Logger.h"

#include "console_style/ConsoleSyle.h"

#include <iostream>
#include <map>
#include <ctime>
#include <fstream>

Logger& Logger::instance() {
  static Logger logger;
  return logger;
}

using namespace std::string_literals;
namespace cs=ConsoleStyle;

void Logger::init(std::experimental::optional<std::string> filename, LOG_LEVEL log_lvl, bool stderr_log) {
  instance().stderr_log = stderr_log;
  instance().log_level = log_lvl;
  instance().logfile = filename;
  instance().indent = 0;
}

void Logger::log(const std::string& log_info, LOG_LEVEL log_lvl, int indent_inc) {
  log(log_info,"",log_lvl,indent_inc);
}

void Logger::log(const std::string& log_info, const std::string& comment, LOG_LEVEL log_lvl, int indent_inc) {
  static const std::map<Logger::LOG_LEVEL,std::string> level_names{
      {_ERROR,  "[ ERROR ]"},
      {WARNING, "[WARNING]"},
      {INFO,    "[ INFO  ]"},
      {TRACE,   "[ TRACE ]"},
      {DEBUG,   "[ DEBUG ]"},
      {VERBOSE, "[VERBOSE]"},
  };
  static auto& logger = instance();
  if (log_lvl <= logger.log_level && (logger.logfile||logger.stderr_log)) {

    if (indent_inc < 0) logger.indent += indent_inc;
    std::string sindent(std::max(0, logger.indent), ' ');
    if (indent_inc > 0) logger.indent += indent_inc;

    std::time_t t = std::time(NULL);
    char mbstr[100];

    if (std::strftime(mbstr, sizeof(mbstr), "%F %T", std::gmtime(&t))) {
    }else{
      mbstr[0]='\0';
    }
    if (logger.logfile){
      std::fstream log(logger.logfile.value(), std::ios::out | std::ios::app);
      log << "["<<  mbstr <<"]" << level_names.at(log_lvl) + sindent + log_info + comment << std::endl;
    }
    if (logger.stderr_log){
      auto color_spec = [](Logger::LOG_LEVEL lvl){
        switch (lvl){
          case _ERROR:
            return cs::red();
        case WARNING:
          return cs::yellow();
        case INFO:
          return cs::green();
        default:
          return cs::default_style();
        }
      };
      std::cerr <<"["<<  mbstr <<"]";
      std::cerr << color_spec(log_lvl) << level_names.at(log_lvl);
      std::cerr << cs::bright() << sindent + log_info;
      std::cerr << cs::italic() << comment << std::endl;
    }
  }
}

Logger::ScopeGuard::ScopeGuard(const char* _sign, const char* file, int line, LOG_LEVEL lvl) :lvl(lvl), sign(_sign) {
  if (lvl <= instance().log_level) {
    log(std::string(sign) + " {",  "  // file:" + std::string(file)+":"+std::to_string(line), (lvl), +2);
  }
}

Logger::ScopeGuard::ScopeGuard(const std::string& _sign, const char* file, int line, LOG_LEVEL lvl) :lvl(lvl), sign(_sign.c_str()) {
  if (lvl <= instance().log_level) {
    log(std::string(sign) + " {", "  // file:" + std::string(file) + ":" + std::to_string(line), (lvl), +2);
  }
}

Logger::ScopeGuard::ScopeGuard(std::string& _sign, const char* file, int line, LOG_LEVEL lvl) :lvl(lvl), sign(_sign.c_str()) {
  if (lvl <= instance().log_level) {
    log(std::string(sign) + " {",  "  // file:" + std::string(file)+":"+std::to_string(line), (lvl), +2);
  }
}

Logger::ScopeGuard::~ScopeGuard() {
  if (lvl <= instance().log_level) {
    log("}"," // " +std::string(sign) + "", lvl, -2);
  }
}