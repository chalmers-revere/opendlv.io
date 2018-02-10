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
    const std::string PROGRAM(argv[0]);
    if (4 != argc) {
        std::cerr << PROGRAM << " decodes latitude/longitude/heading from an OXTS GPS/INSS unit and publishes it to a running OpenDaVINCI session using the OpenDLV Standard Message Set." << std::endl;
        std::cerr << "Usage:   " << PROGRAM << " <IPv4-address> <port> <OpenDaVINCI session>" << std::endl;
        std::cerr << "Example: " << PROGRAM << " 0.0.0.0 3000 111" << std::endl;
        retCode = 1;
    } else {
        // Interface to a running OpenDaVINCI session (ignoring any incoming Envelopes).
        cluon::OD4Session od4{static_cast<uint16_t>(std::stoi(std::string{argv[3]})),
            [](auto){}
        };

        // Interface to OxTS.
        const std::string OXTS_ADDRESS(argv[1]);
        const std::string OXTS_PORT(argv[2]);
        OxTSDecoder oxtsDecoder;
        cluon::UDPReceiver fromOXTS(OXTS_ADDRESS, std::stoi(OXTS_PORT),
            [&od4Session = od4, &decoder=oxtsDecoder](std::string &&d, std::string &&/*from*/, std::chrono::system_clock::time_point &&tp) noexcept {
            auto retVal = decoder.decode(d);
            if (retVal.first) {
                cluon::data::TimeStamp sampleTime = cluon::time::convert(tp);

                opendlv::proxy::GeodeticWgs84Reading msg1 = retVal.second.first;
                od4Session.send(msg1, sampleTime);

                opendlv::proxy::GeodeticHeadingReading msg2 = retVal.second.second;
                od4Session.send(msg2, sampleTime);

                // Print values on console.
                {
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
