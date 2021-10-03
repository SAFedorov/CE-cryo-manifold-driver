# ColdEdge Stinger auto manifold driver

The project implements a driver for a computerized helium valve manifold of a closed-cycle Stinger cryostat from [ColdEdge](https://www.coldedgetech.com/). The driver runs on an Arduino board; it controls responses to button presses, reads and displays helium pressure, and can communicate with a host computer via the COM port using text commands with SCPI (Standard Commands for Programmable Instruments) syntax. The software has been successfully operated in a lab at EPFL since 2018.

The list of remote control commands:

|Command	| Description	| Access |
|---------|-------------|--------|
|\*IDN?| Queries the identification string of the instrument. |	query only |
|:VALVE\<i\> |	Sets or reads the state of valve number *i*, where *i*=1-5 or 7 (no VALVE6 in the hardware). The argument can be 0 (the valve is closed) or 1 (the valve is open). |	write/query |
|:RECirculator |	Sets or reads the state of the recirculator. The argument can be 0 (the recirculator is off) or 1 (the recirculator is on). |	write/query |
|:COOLer |	Sets or reads the state of the cryocooler. The argument can be 0 (the cryocooler is off) or 1 (the cryocooler is on).	| write/query |
|:PRESsure? |	Queries helium supply pressure (returned in PSI).	| query only |

For example, sending a command string `:REC 1;:VALVE3 0` will start the recirculator and close the 3rd valve, and for querying `:VALVE2?;:PRES?` the response will be a string containing the state of the 2nd valve and the pressure separated by ;. A MATLAB class that implements communication with this driver on the host computer side exists as part of [Instrument-control](https://github.com/engelsen/Instrument-control/tree/master/Instrument%20classes/%40MyColdEdgeCryo).

This project is based on an extended version of Lachlan Gunn' [Open Instrument Control](https://github.com/SAFedorov/oic)(included in the repository) and ColdEdge AutoCool_v1.35

