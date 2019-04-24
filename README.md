# packet-sniffer

## Compilation

Library **libpcap-dev** should be installed. Running 'make' will create two executables:
* **cli** - Command Line Interface;
* **sniffer** - sniffing app;

Both of them require **sudo**.

## Usage

All manipulations are done through CLI
```
Usage: start                - sniffed packets starts counting (default iface)
       stop                 - sniffed packets stops counting
       show [ip] count      - shows the number of packets, received from [ip]
       select iface [iface] - select interface for sniffing
       stat [iface]         - show statistic for [iface]
       run                  - starts the sniffer process
       kill                 - kills the sniffer process
       --help               - this message is shown
```

## TODO

* Make normal Makefile;
* Add balancing for IP trees;
