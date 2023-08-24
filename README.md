# SPM4650 for RPI Pico
The SPM4650 needs uses one wire for RX/TX (UART Half Duplex). I used a diode to bridge the RX/TX on the pi.
RX -------
          |--- SPM4650 Signal
TX --<|---



## Helpful links
* https://www.raspberrypi.com/documentation/microcontrollers/debug-probe.html
* https://www.analog.com/en/technical-articles/using-a-uart-to-implement-a-1wire-bus-master.html
* SPM4650 - https://github.com/SpektrumRC/SRXL2