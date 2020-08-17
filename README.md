# AirTight

A C implementation of the AirTight protocol from the Real-Time Systems group at the University of York.

## Introduction

AirTight is a protocol for critical wireless communication based on the [IEEE 802.15.4](https://en.wikipedia.org/wiki/IEEE_802.15.4) MAC and physical layers.

AirTight is described in the paper:
> A. Burns, J. Harbin, L. Indrusiak, I. Bate, R. Davis and D. Griffin, "AirTight: A Resilient Wireless Communication Protocol for Mixed-Criticality Systems," 2018 IEEE 24th International Conference on Embedded and Real-Time Computing Systems and Applications (RTCSA), Hakodate, 2018, pp. 65-75, doi: 10.1109/RTCSA.2018.00017.

An existing implementation of AirTight, for TinyOS using NESC, is available at: [https://github.com/RTSYork/airtight](https://github.com/RTSYork/airtight).

This implementation itends to be more platform independent than the existing implementation and more extensively documented. For the purposes of development, testing, and as an example, the [Pi-Puck](https://www.york.ac.uk/robot-lab/pi-puck/) platform was used for testing, and the XBee module on the platform was targeted for the example radio port.

## Updating & Building the XBee Library

To build the XBee library:

```sh
cd xbee
make
```

If you wish to make sure you are using the latest version:

```sh
git clone https://github.com/digidotcom/xbee_ansic_library.git
cd xbee
./import_library.sh
```

Then build as before.

## Building Docs

To build the docs you will need [Doxygen](https://www.doxygen.nl/index.html).

With doxygen installed simply run:

```sh
make doc
```

## Bulding the Project

The XBee library is a dependency and therefore must already be built.

From the project root simply run:

```sh
make
```

## Configuring AirTight

At it's core AirTight features a Priority Critical Queue (PCQ) which stores packets due to be sent or forwarded. To configure the PCQ use the following:
- To set the number of priority levels edit `AIRTIGHT_PRIORITIES` in `src/airtight_types.h`.
- To set the depth of each queue edit `PRIORITY_CRITICAL_QUEUE_SIZE` in `src/airtight_priority_critical_queue.h`.
- To set the criticalities of each queue edit `AT_CONF_CRITICALITIES` in `src/airtight_mac_config.h`.

The other core structure is the scheduling table used to organise transmissions and receptions on the AirTight network. To configure the schedule table use the following:
- To set the number of rows and columns in the schedule table edit `AT_CONF_SLOT_TABLE_ROWS` and `AT_CONF_SLOT_TABLE_COLUMNS` respectively in `src/airtight_slots_config.h`.
- To set the contents of the table edit the file `src/airtight_slot_table.txt`.

The full network of connections between nodes and the hops required to route packets around the network is specified in the file `src/airtight_routes_config.txt` with each `HOP` rule having a node ID to which it applied, a final packet destination, and a "hop" destination. Unspecified routes will be assumed to be direct with a single hop.

Further configuration parameters can be found in `src/airtight_mac_config.h` and are documented there.

## Debugging

Debugging is enabled by default but can be disabled by undefining `AIRTIGHT_DEBUG` in `src/airtight_utilities.h`.

## Logging

Logging is enabled and prefixed with "LOG" by default. Logging can be disabled by undefining `AIRTIGHT_LOGGING` in `src/airtight_logging.h`.

## License

This project is licensed under the BSD 3-Clause license, see [LICENSE](LICENSE) for details.

The XBee ANSI C library is Copyright Digi International, 2013 and licensed under the Mozilla Public License, v. 2.0. See [https://github.com/digidotcom/xbee_ansic_library](https://github.com/digidotcom/xbee_ansic_library) for further details.
