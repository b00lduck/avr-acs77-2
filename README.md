# ACS77-2 DCF-77 clock based on ATmega32

A rebuild of the old "Renkforce ACS-77" with modern components.

Built with AVR Studio 6.

BOM (incomplete):
- 1x custom PCB (sorry, no layout available)
- 1x DCF-77 receiver board from reichelt
- 1x ATmega32
- 1x 16MHz crystal
- 2x 22pF ceramic capacitor (crystal load)
- 2x 1uF capacitor
- 1x 100uF electrolyte capacitor
- 1x 47uF electrolyte capacitor
- 1x 1N4004
- 1x 100nF capacitor
- 8x current limiting resistor for LEDs (depends on LED current, be aware of multiplexing)
- 1x current limiting resistor for single LED (about 1k)
- 1x 5mm red LED
- 6-Pin header for ICSP
- 2x tactile button
- optional: sound generator board for chime

[[https://github.com/b00lduck/avr-acs77-2/blob/master/oshw.svg|Open source hardware]]