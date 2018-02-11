/*
 * Copyright (C) 2018  Christian Berger
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "cluon-complete.hpp"
#include "opendlv-standard-message-set.hpp"

#include "oxts-decoder.hpp"

#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

int32_t main(int32_t argc, char **argv) {
    int32_t retCode{0};
    auto commandlineArguments = cluon::getCommandlineArguments(argc, argv);
    if ( (0 == commandlineArguments.count("oxts_port")) || (0 == commandlineArguments.count("cid")) ) {
        std::cerr << argv[0] << " decodes latitude/longitude/heading from an OXTS GPS/INSS unit and publishes it to a running OpenDaVINCI session using the OpenDLV Standard Message Set." << std::endl;
        std::cerr << "Usage:   " << argv[0] << " [--oxts_ip=<IPv4-address>] --oxts_port=<port> --cid=<OpenDaVINCI session> [--id=<Identifier in case of multiple OxTS units>] [--verbose]" << std::endl;
        std::cerr << "Example: " << argv[0] << " --oxts_ip=0.0.0.0 --oxts_port=3000 --cid=111" << std::endl;
        retCode = 1;
    } else {
        const uint32_t ID{(commandlineArguments["id"].size() != 0) ? static_cast<uint32_t>(std::stoi(commandlineArguments["id"])) : 0};
        const bool VERBOSE{commandlineArguments.count("verbose") != 0};

        // Interface to a running OpenDaVINCI session (ignoring any incoming Envelopes).
        cluon::OD4Session od4{static_cast<uint16_t>(std::stoi(commandlineArguments["cid"])),
            [](auto){}
        };

        // Interface to OxTS.
        const std::string OXTS_ADDRESS((commandlineArguments.count("oxts_ip") == 0) ? "0.0.0.0" : commandlineArguments["oxts_ip"]);
        const std::string OXTS_PORT(commandlineArguments["oxts_port"]);
        OxTSDecoder oxtsDecoder;
        cluon::UDPReceiver fromOXTS(OXTS_ADDRESS, std::stoi(OXTS_PORT),
            [&od4Session = od4, &decoder=oxtsDecoder, senderStamp = ID, VERBOSE](std::string &&d, std::string &&/*from*/, std::chrono::system_clock::time_point &&tp) noexcept {
            auto retVal = decoder.decode(d);
            if (retVal.first) {
                cluon::data::TimeStamp sampleTime = cluon::time::convert(tp);

                opendlv::proxy::GeodeticWgs84Reading msg1 = retVal.second.first;
                od4Session.send(msg1, sampleTime, senderStamp);

                opendlv::proxy::GeodeticHeadingReading msg2 = retVal.second.second;
                od4Session.send(msg2, sampleTime, senderStamp);

                // Print values on console.
                if (VERBOSE) {
                    std::stringstream buffer;
                    msg1.accept([](uint32_t, const std::string &, const std::string &) {},
                               [&buffer](uint32_t, std::string &&, std::string &&n, auto v) { buffer << n << " = " << v << '\n'; },
                               []() {});
                    std::cout << buffer.str() << std::endl;

                    std::stringstream buffer2;
                    msg2.accept([](uint32_t, const std::string &, const std::string &) {},
                               [&buffer2](uint32_t, std::string &&, std::string &&n, auto v) { buffer2 << n << " = " << v << '\n'; },
                               []() {});
                    std::cout << buffer2.str() << std::endl;
                }
            }
        });

        // Just sleep as this microservice is data driven.
        using namespace std::literals::chrono_literals;
        while (od4.isRunning()) {
            std::this_thread::sleep_for(1s);
        }
    }
    return retCode;
}
