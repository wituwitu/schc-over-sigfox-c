# SCHC-over-Sigfox: Emulator in C

This project aims to develop a complete emulation of the [SCHC Framework](https://datatracker.ietf.org/doc/html/rfc8724)
when employed over the Sigfox
network ([SCHC-over-Sigfox](https://datatracker.ietf.org/doc/html/draft-ietf-lpwan-schc-over-sigfox)), written in the C
language.
The code is developed
using [the official Python implementation and emulation](https://github.com/schc-over-sigfox/schc-over-sigfox) as a
base.
The C language is prefered to perform model checking and formal verification of the protocol.

## About SCHC

The Static Context Header Compression and fragmentation (SCHC) framework provides both a header compression mechanism
and an optional fragmentation mechanism over LPWAN transmissions.
The fragmentation and reassembly mechanism can be used to support the IPv6 MTU requirement over the LPWAN technologies.
Fragmentation is needed for IPv6 datagrams that, after SCHC compression or when such compression was not possible, still
exceed the Layer 2 maximum payload size.
The SCHC header compression and fragmentation mechanisms are independent of the specific LPWAN technology over which
they are used.
Settings and choices specific to a technology or a product are expected to be grouped into profiles.
The SCHC profile of the Sigfox network uses no compression mechanism, and employs the
extended [SCHC Compound ACK](https://datatracker.ietf.org/doc/draft-ietf-lpwan-schc-compound-ack/).

([Source](https://datatracker.ietf.org/doc/html/rfc8724))

## Structure

This project has a similar structure to that of
the [Python implementation](https://github.com/schc-over-sigfox/schc-over-sigfox).
Classes and objects are replaced with C `struct`s and functions that operate over allocated memory for said structures.
The communication between sender and received is performed over a socket interface within the local link of the computer
the emulation is run in.
The `tests` module contains test cases for the functions defined in the modules.
To follow the TDD scheme, these tests should display the expected working of the functions and verified after making any
changes, updating the test cases if needed.

## TODO

* Implement the `schc_recv()` function of the `SCHCSession` structure, which executes the receiver algorithm of
  SCHC-over-Sigfox.
    * Reference:
      the [`SCHCReceiver.schc_recv()` method](https://github.com/schc-over-sigfox/schc-over-sigfox/blob/main/Entities/SCHCReceiver.py#L321).
    * Write test cases accordingly.
* Implement the `schc_receiver` module, which should provide an interface for incoming SCHC Fragments and execute the
  functions of the `schc_session` module to process a SCHC transmission at the receiver side.
    * Reference:
      the [`schcfox_server.receive()` function](https://github.com/schc-over-sigfox/schc-over-sigfox/blob/main/schcfox_server.py#L22).
    * Write test cases accordingly.
* Any ideas to optimize the Python implementation that may arise from developing this code are welcome as pull requests
  at the [official repository](https://github.com/schc-over-sigfox/schc-over-sigfox).

## Authors

* **Javier Bustos:** Project director
* **Diego Wistuba:** Profile coauthor, developer
* **Yareli Rojas:** Intern (Niclabs), developer