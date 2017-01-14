#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filter/counter.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/program_options.hpp>
#include <iomanip>
#include "json_input_app/ConsoleApplication.h"
#include "console_style/ConsoleSyle.h"
#include "json_input_app/Logger.h"

using namespace JSON;
namespace cs = ConsoleStyle;

namespace bio = boost::iostreams;
namespace cs = ConsoleStyle;
using namespace std::string_literals;

namespace {
class my_counter {
public:
  typedef char char_type;
  struct category : bio::dual_use, bio::filter_tag, bio::multichar_tag, bio::optimally_buffered_tag {};
  explicit my_counter(int first_line = 0, int first_char = 0) : lines_(first_line), chars_(first_char) {}
  std::string last_line() const { return m_last_line; }
  int lines() const { return lines_; }
  int characters() const { return chars_; }
  std::streamsize optimal_buffer_size() const { return 0; }

  template <typename Source>
  std::streamsize read(Source& src, char_type* s, std::streamsize n) {
    std::streamsize result = bio::read(src, s, n);
    if (result == -1) return -1;
    absorb_chars(s, s + result);
    return result;
  }

  template <typename Sink>
  std::streamsize write(Sink& snk, const char_type* s, std::streamsize n) {
    std::streamsize result = bio::write(snk, s, n);
    absorb_chars(s, s + result);
    return result;
  }

private:
  void absorb_chars(const char* begin, const char* end) {
    while (begin != end) {
      absorb_char(*begin);
      ++begin;
    }
  }
  void absorb_char(const char c) {
    if (c == '\n') {
      lines_++;
      chars_ = 0;
    } else {
      if (chars_ == 0) {
        m_last_line.clear();
      }
      chars_++;
      m_last_line += c;
    }
  }
  int lines_;
  std::string m_last_line;
  int chars_;
};
}

ConsoleApplication::ConsoleApplication(int argc, char** const argv): argc(argc),argv(argv) { is = &std::cin; }

ConsoleApplication::SetupStatus ConsoleApplication::setup() {

  namespace po = boost::program_options;
  po::options_description options("Options");
  po::options_description no_exec_options("No-exec options");
  po::options_description task_option;
  po::options_description all_options;

  po::positional_options_description positional_options;

  positional_options.add("task", 1);

  no_exec_options.add_options()
      ("help,h","print help message")
      ("version","print program version")
      ("credits","print authors list")
      ;
  task_option.add_options()
      ("task",po::value<std::string>()->value_name("<str>"),"task name");

  options.add_options()
      ("logfile",po::value<std::string>()->value_name("<filename>"),"creates logfile is specified")
      ("logstderr", "prints log messages in stderr")
      ("verbose",po::value<int>()->default_value(Logger::WARNING),"set verbosity level")
      ("color",po::value<std::string>()->default_value("auto"s)->value_name("[auto|on|off]"),"set console coloring mode")
      ;

  all_options.add(options).add(no_exec_options).add(task_option);


  auto print_usage = [&](std::ostream& out){
    out << cs::bright() << "Usage:" << std::endl;
    out << "    "<< cs::green()<<cs::bright() << argv[0];
    out << cs::magenta() << " [no-exec-options]" << std::endl;
    out << "    "<< cs::green()<<cs::bright() << argv[0];
    out << cs::yellow() << " [options]" << cs::cyan()<< " {task_name}" << std::endl;
    out << cs::bright() << "Tasks:" <<std::endl;
    for (auto&x: tasks){
      auto docs = x.second->schema.doc_strings();
      auto doc = docs.empty()? "<do description>"s:docs.front();
      out << "    "<< cs::cyan()<<std::setw(15) << std::left << x.first;
      out << doc << std::endl;
    }
    out << cs::yellow() << options << "\n";
    out << cs::magenta() << no_exec_options << "\n";
  };

  po::variables_map vm;

  try {
    po::store(po::command_line_parser(argc, argv).options(all_options)
      .positional(positional_options)
      .run(), vm);
    po::notify(vm);
  } catch (std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
    print_usage(std::cerr);
    return SetupStatus::BadOptions;
  }



  if (vm.count("color")){
    auto color = vm["color"].as<std::string>();
    if (color=="auto"){
      cs::set_cerr_style_mode(cs::ConsoleMode::AUTO);
    }else if (color=="on"){
      cs::set_cerr_style_mode(cs::ConsoleMode::FORCE_COLORS);
    } else if (color=="off"){
      cs::set_cerr_style_mode(cs::ConsoleMode::FORCE_NO_COLORS);
    } else {
      std::cerr << cs::red() << "Bad `color` option: "<< color << std::endl;
      print_usage(std::cerr);
      return SetupStatus::BadOptions;
    }
  }
  if (vm.count("help")) {
    print_usage(std::cout);
    return SetupStatus::NoExecOptions;
  }
  if (vm.count("credits")){
    std::cout << cs::bright() <<"Authors: "<< std::endl;
    for (auto& name_email: std::vector<std::pair<std::string,std::string>>{
        {"sizmailov", "sergei.a.izmailov@gmail.com"}
    }){
      std::cout <<"   "<<cs::magenta() << name_email.first << " " << cs::white() << cs::italic() << name_email.second << std::endl;
    }
    return SetupStatus::NoExecOptions;
  }

  if (vm.count("task")){
    if (vm.count("logfile") || vm.count("logstderr")){
      auto verbosity = vm["verbose"].as<int>();
      std::experimental::optional<std::string> logfile;
      if (vm.count("logfile")) logfile = vm["logfile"].as<std::string>();
      bool stderr_log = vm.count("logstderr");
      if (verbosity>Logger::VERBOSE) verbosity= Logger::VERBOSE;
      if (verbosity<Logger::_ERROR) verbosity= Logger::_ERROR;
      Logger::init(logfile, static_cast<Logger::LOG_LEVEL>(verbosity),stderr_log);
    }

    auto task_name = vm["task"].as<std::string>();
    if (tasks.count(task_name)){
      task = tasks[task_name].get();
      return SetupStatus::GoodOptions;
    }else{
      std::cerr << cs::red() << "Unknown task name `" << task_name<<"`"<<std::endl;
      print_usage(std::cerr);
    }
  }
  print_usage(std::cerr);
  return SetupStatus::BadOptions;
}

int ConsoleApplication::exec() {
  auto setup_rcode = setup();
  switch (setup_rcode){
    case SetupStatus::BadOptions:
      std::cerr << cs::red() << "Execution was not performed due to an error in program arguments" << std::endl;
      return 1;
    case SetupStatus::NoExecOptions:
      return 0;
    case SetupStatus::GoodOptions:
      break;
  }

  Json json;
  bio::filtering_istream in;
  in.push(my_counter{1, 1});
  in.push(*is, 1, 1);

  LOG_SCOPE_INFO("Starting task processing");
  try {
    assert(task);
    {
      LOG_SCOPE_TRACE("Reading input");
      try {
        in >> json;
      }
      catch (JsonException& e) {

        int lines = in.component<0, my_counter>()->lines();
        int characters = in.component<0, my_counter>()->characters();
        auto last_line = in.component<0, my_counter>()->last_line();

        LOG_ERROR("Error at line "+std::to_string(lines)+":"+std::to_string(characters)+": "+e.what());
        //read rest of line if available
        for (int i = 0; i<10; i++) {
          char c;
          in.read(&c, 1);
          if (!in.good() || c=='\n') {
            break;
          }
          else {
            last_line += c;
          }
        }
        auto filler_size = std::max(characters-2,0);

        std::cerr << cs::cyan() << "Bad input:" << std::endl;
        std::cerr << "Error at line ";

        std::cerr << cs::yellow() << lines;
        std::cerr << ":";
        std::cerr << cs::yellow() << characters << ":" << std::endl;
        std::cerr << last_line << std::endl;
        std::cerr << cs::red() << std::string(filler_size, ' ') << "^" << std::string(10, '~') << " " << e.what()
                  << std::endl;
        return 1;
      }
    }
    LOG_INFO("Input is a valid JSON");
    {
      LOG_SCOPE_INFO("Checking input");
      try {
        task->parse(json);
      }
      catch (const SchemaMatchResult::MatchError& e) {
        LOG_ERROR("Json does not match schema: "s+e.what());
        task->print_schema();
        std::cerr << std::endl;
        std::cerr << cs::cyan() << "Bad input:" << std::endl;
        std::cerr << cs::red() << e << std::endl;
        e.pretty_wordy_print(std::cerr);
        std::cerr << std::endl;
        return 1;
      }
    }

    LOG_INFO("Input is valid");

    LOG_SCOPE_INFO("Running task");
    return task->run();
  } catch (const std::exception& e) {
    std::cerr << cs::cyan() << "Unhandled exception:";
    std::cerr << cs::red() << "   what(): " << e.what();
    return 1;
  } catch (...) {
    std::cerr << "Unknown exception" << std::endl;
    return 1;
  }
}
