Moved to https://git.opendlv.org.

## OpenDLV.io - A modern software ecosystem to power autonomous systems

This repository provides a collection of microservices to interface with components
to power autonomous systems using OpenDLV.io software ecosystem.

[![License: GPLv3](https://img.shields.io/badge/license-GPL--3-blue.svg
)](https://www.gnu.org/licenses/gpl-3.0.txt)

## Table of Contents
* [Usage](#usage)
* [License](#license)


## Usage
The microservices for OpenDLV.io are created automatically on changes to this
repository via Docker's public registry for:
* [x86_64](https://hub.docker.com/r/chalmersrevere/opendlv.io-amd64/tags/)
* [armhf](https://hub.docker.com/r/chalmersrevere/opendlv.io-armhf/tags/)
* [aarch64](https://hub.docker.com/r/chalmersrevere/opendlv.io-aarch64/tags/)

The following microservices are available:

* [proxy-oxts](https://github.com/chalmers-revere/opendlv.io/tree/master/proxy-oxts): interface to [OXTS GPS/INSS](https://www.oxts.com) units:

    `docker run --rm --net=host chalmersrevere/opendlv.io-multi:proxy-oxts-latest`

* [signal-viewer](https://github.com/chalmers-revere/opendlv.io/tree/master/signal-viewer): view signals in your web-browser:

    `docker run --rm --net=host chalmersrevere/opendlv.io-amd64:signal-viewer-v0.0.1 111`

![screenshot from signal viewer](https://github.com/chalmers-revere/opendlv.io/raw/signal-viewer/signal-viewer/signal-viewer.png)

* [vehicle-view](https://github.com/chalmers-revere/opendlv.io/tree/master/vehicle-view): view selected vehicle onboard data in your web-browser:

    `docker run --rm --net=host chalmersrevere/opendlv.io-amd64:vehicle-view-v0.0.1 111`

![screenshot from vehicle view](https://github.com/chalmers-revere/opendlv.io/raw/vehicle-view/vehicle-view/vehicle-view.png)


## License

* This project is released under the terms of the GNU GPLv3 License

