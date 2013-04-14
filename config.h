#ifndef _GUARD_CONFIG_H_
#define _GUARD_CONFIG_H_

#include <chrono>
#include <string>
#include <boost/program_options.hpp>

#include "stats.h"
#include "net.h"

class Config {
  using clock = Stats::clock;
  boost::program_options::options_description _desc;
  boost::program_options::variables_map       _vars;
  int _cooldown;
  int _ui_delay;
  int _sample_period;
  int _quantum_period;

public:
  bool        help;
  std::string interface;
  std::string host;
  std::string event_cmd;
  size_t      min_rate;
  Net::port_t port;
  int         cap_timeout;
  clock::duration cooldown;
  clock::duration ui_delay;
  clock::duration sample_period;
  clock::duration quantum_period;
  
  const boost::program_options::options_description desc() {
    return _desc;
  };
  
  std::string pcap_filter() {
    std::ostringstream s;
    s << "udp and not net 65.52.0.0/14";
    s << " and src host " << host;
    s << " and src port " << port;
    return s.str();
  };

  Config() = delete;
  Config( const int argc, const char* const argv[] ) : _desc("Options") {
    namespace opt = boost::program_options;
    using std::chrono::seconds;
    using std::chrono::milliseconds;
    _desc.add_options()
      ("help",
        opt::bool_switch(&help),
        "display help")
      ("interface,i",
        opt::value<std::string>(&interface)->default_value(std::string("eth0")),
        "Capture interface")
      ("cap-timeout",
        opt::value<int>(&cap_timeout)->default_value(100),
        "Capture timeout in milliseconds")
      ("host,h",
        opt::value<std::string>(&host)->default_value(std::string()),
        "Source address")
      ("port,p",
        opt::value<Stats::port_t>(&port)->default_value(3074),
        "UDP source port")
      ("min-rate",
        opt::value<size_t>(&min_rate)->default_value(2200),
        "Minimum rate in bytes-per-second")
      ("ui-delay",
        opt::value<int>(&_ui_delay)->default_value(1000),
        "Milliseconds between UI updates")
      ("on-event,e",
        opt::value<std::string>(&event_cmd)->default_value(std::string()),
        "Command to execute when event is observed")
      ("cooldown",
        opt::value<int>(&_cooldown)->default_value(15),
        "Minimum time (in seconds) between command executions for the same host")
      ("sample-period",
        opt::value<int>(&_sample_period)->default_value(30),
        "Period of time to measure samples (in seconds, must be a multiple of quantum-period*1000)")
      ("quantum-period",
        opt::value<int>(&_quantum_period)->default_value(250),
        "Smallest interval of time that can be measured (in milliseconds)")
    ;
    opt::store(opt::parse_command_line(argc, argv, _desc), _vars);
    opt::notify(_vars);
    
    ui_delay = milliseconds(_ui_delay);
    cooldown = seconds(_cooldown);
    sample_period = seconds(_sample_period);
    quantum_period = milliseconds(_quantum_period);
  };
};
#endif
