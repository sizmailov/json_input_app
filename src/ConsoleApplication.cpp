#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filter/counter.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include "json_input_app/ConsoleApplication.h"
#include "console_style/ConsoleSyle.h"

using namespace JSON;
namespace cs = ConsoleStyle;

namespace bio = boost::iostreams;
namespace cs = ConsoleStyle;

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

ConsoleApplication::ConsoleApplication() { is = &std::cin; }

void ConsoleApplication::setup(int argc, char** argv) {
  if (argc == 1) {
    std::cerr << "Wrong usage" << std::endl;
    setup_rcode = 1;
  } else {
    std::string s(argv[1]);
    if (tasks.count(s)) {
      task = tasks.at(s).get();
      setup_rcode = 0;
    } else {
      std::cerr << "Wrong task name " << std::endl;
      std::cerr << "<print help here>" << std::endl;
      setup_rcode = 1;
    }
  }
}

int ConsoleApplication::exec() {
  if (setup_rcode != 0) {
    return 1;
  }

  Json json;
  bio::filtering_istream in;
  in.push(my_counter{1, 1});
  in.push(*is, 1, 1);

  try {
    assert(task);
    try {
      in >> json;
    }catch(JsonException&e){

      int lines = in.component<0, my_counter>()->lines();
      int characters = in.component<0, my_counter>()->characters();
      auto last_line = in.component<0, my_counter>()->last_line();
      //read rest of line
      for (int i=0;i<10;i++){
        char c;
        in.read(&c,1);
        if (!in.good() || c=='\n'){
          break;
        }else{
          last_line+=c;
        }
      }
      auto filler_size = characters-(characters>0 ? 2 : 0);

      std::cerr << cs::cyan() << "Bad input:" << std::endl;
      std::cerr << "Error at line ";

      std::cerr << cs::yellow() << lines;
      std::cerr << ":";
      std::cerr << cs::yellow() << characters << ":" << std::endl;
      std::cerr << last_line << std::endl;
      std::cerr << cs::red()<<std::string(filler_size, ' ') << "^" << std::string(10, '~') << " " << e.what() << std::endl;
      return 1;
    }
    task->parse(json);
    return task->run();
  } catch (const SchemaMatchResult::MatchError& e) {
    task->print_schema();
//    std::cout << std::flush;
    std::cerr << std::endl;
    std::cerr << cs::cyan() << "Bad input:" << std::endl;
    std::cerr << cs::red() << e << std::endl;
    e.pretty_wordy_print(std::cerr);
    std::cerr << std::endl;
    return 1;
  } catch (const std::exception& e) {
    std::cerr << cs::cyan() << "Unhandled exception:";
    std::cerr << cs::red() << "   what(): " << e.what();
    return 1;
  } catch (...) {
    std::cerr << "Unknown exception" << std::endl;
    return 1;
  }
}
