# Anybus CompactCom SPI Protocol Analyzer Plugin

## Copyright &copy; 2015-2018 HMS Industrial Networks, Inc.

THE CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. HMS DOES NOT
WARRANT THAT THE FUNCTIONS OF THE CODE WILL MEET YOUR REQUIREMENTS, OR
THAT THE OPERATION OF THE CODE WILL BE UNINTERRUPTED OR ERROR-FREE, OR
THAT DEFECTS IN IT CAN BE CORRECTED.

---

## [Table of Contents](#table-of-contents)

1. [Description](#description)
2. [System Requirements](#system-requirements)
3. [Installation](#installation)
    * [Windows](#windows)
    * [Linux](#linux)
    * [OS X](#os-x)
4. [Documentation](#documentation)
5. [Changelog](#changelog)
6. [Licenses](#licenses)

---

## [Description](#table-of-contents)

This plugin in conjunction with the Saleae 'Logic' hardware and software provides
an easy-to-use interface for analyzing bi-directional (full-duplex) ABCC SPI
communication. This analyzer plugin parses SPI communication according to the
ABCC SPI protocol. Each field within an SPI telegram is added as a multi-layered
bubble-text within the Logic software. This means basic markup is displayed
when zoomed-out and very detailed information when zoomed-in on the capture.
By coupling this protocol analyzer with other physical signals from the target
device (such as CAN, Serial, GPIO, etc.) this tool can help solve
difficult-to-diagnose issues that arise during initial development efforts. In
spirit, this plugin is intended as an educational tool that equips developers
with an accessible interface into host and module communication following the
SPI communication thus allowing them to observe the real world startup and
operational behavior of a device communicating with the ABCC over the SPI
protocol.

![Overview of Plugin](https://github.com/HMSAB/AbccSpiAnalyzer/wiki/overview.gif "Overview of Plugin")

## [System Requirements](#table-of-contents)

* Saleae Logic Software (version 1.2.7)
  * Other versions will work so long as **Analyzer SDK version 1.1.32** is
    compatible
* Saleae Logic Hardware
  * While any of Saleae Logic hardware offerings is expected to be compatible,
  it is important to consider sample rate limitations of the Logic hardware and
  what SPI data rate the ABCC host-module communication is using. The absolute
  minimum requirement for sufficiently sampling the communication is 2x the
  frequency of the SPI SCLK. The ABCC is able to work over a wide range of
  clock rates where the maximum is specified at 20MHz. So to be able to support
  this would require at a minimum the ability to sample at 40MSamples/second.
  Keep in mind that these analyzers may reduce sample rate depending on how many
  channels on the analyzer are active, so it is imperative that the hardware
  is able to sufficiently sample the SPI bus for the number of channels required.
* PC running:
  * Windows 7 or later
  * Ubuntu 12.04.2+
  * Mac OSX 10.7 Lion+
* USB 2.0 or 3.0 port
  * Using the Saleae Logic on a dedicated USB controller is recommended for
  ensuring the best sampling performance of the hardware will be possible.

## [Installation](#table-of-contents)

### [Windows](#table-of-contents)

To compile the project please ensure the **additional** requirements are met:

* Visual Studio 2012 or later *(retargetting project may be necessary)*

> NOTE: **Express** or **Community** versions of Visual Studio may require
additional steps to replace occurances of `#include "afxres.h"` with `#include "windows.h"`.

When compiling the project, please ensure that the correct library is linked
for your Windows OS. The VS solutions provided are setup with the expectation
that the host system will be x64. The solutions are also setup for batch build
such that an x64 system can compile all supported build configurations with
one request. To access this, right-click the **Solution 'AbccSpiAnalyzer'** item
in the **Solution Explorer** subwindow to access the context emnu and select
**Batch Build...**.

#### Custom Visual Studio Projects

If you decide to make your own Visual Studio project (perhaps to use some other
version), ensure that you configure the linker to correctly select the proper
library

* If you are using **Win64**, specify `./sdk/release/Analyzer64.lib` in your linker input.
  * The configuration manager has this setup as x64.
* If you are using **Win32**, specify `./sdk/release/Analyzer.lib` in your linker input.
  * The configuration manager has this setup as Win32.

Once compiled, a file called `AbccSpiAnalyzer.dll` or `AbccSpiAnalyzer64.dll`
in the either ./plugins/Win32/ or ./plugins/Win64 folder, respectively. Copy
this DLL to your Saleae Logic software installation in the "Analyzers" folder:

* Example: `C:\Program Files\Saleae LLC\Analyzers\AbccSpiAnalyzer.dll`

The analyzer is now ready to be used and can be found in the same way that
other protocol analyzers are added to the Logic software.

### [Linux](#table-of-contents)

* To compile simply run the `build_analyzer.py` python script. This script will
  compile the appropriate shared object library based on the host system.
> DEPENDANCIES: **Python**, **G++**

Once compiled, a file called `AbccSpiAnalyzer.so` or `AbccSpiAnalyzer64.so`
will reside in the either `./plugins/Linux/` or `./plugins/Linux64` folder,
respectively. Copy this shared object to your Saleae Logic software
installation in the "Analyzers" folder.

### [OS X](#table-of-contents)

* To compile simply run the `build_analyzer.py` python script.
> DEPENDANCIES: **Python**, **G++**

Once compiled, a dynamic library called `AbccSpiAnalyzer.dylib` will reside
in the `./plugins/OSX/` folder. Copy this dynamic object to your Saleae Logic
software installation in the "Analyzers" folder.

## [Documentation](#table-of-contents)

For complete details regarding this plugin's functionality please consult the
[Quick Start Guide](doc/AbccSpiAnalyzer_Plugin_QSG.pdf) included in this repository's documentation folder.

For details on the Logic SDK's API please refer to the Saleae's [Protocol Analyzer SDK](https://support.saleae.com/hc/en-us/articles/115005987726-Protocol-Analyzer-SDK) page.

## [Changelog](#table-of-contents)

Please see [CHANGELOG.md](CHANGELOG.md) provided within this repository for details on the changelog history.

## [Licenses](#table-of-contents)

Please see [LICENSE.md](LICENSE.md) provided within this repository for details on the software licensing.

