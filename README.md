# avr-component-tester

ATmega328p-based component tester with 16x2 HD44780 LCD output — firmware, schematic, partlist and PCB sources mirrored from the original Google Code SVN project.

## What it does

A "transistor tester" style instrument that identifies and characterizes through-hole components (transistors, diodes, resistors, capacitors, etc.) and shows the result on a 16x2 character LCD. The repository preserves the full original project tree:

- AVR firmware targeting an ATmega328p at 1 MHz (`trunk/firmware/`, project `TransistorTestNew`).
- PCB / schematic sources for both Eagle and Altium Designer (`trunk/firmware/avr-tester/avr-tester/hardware/`), Gerbers, and CAMtastic camera files.
- Documents: schematic PDF, board-layout PDF, partlist (`trunk/Documents/`).
- LCD helper routines (`f/lcd-routines.[ch]`) and example test programs for custom characters and `lcd_puts` (`examples/`).
- Original SVN metadata under `.svn/` and a top-level `Last Loaded avr-tester-1602.DBK` plus DSN/PWI files (Proteus capture).

The original `trunk/README` is by **BrentBXR** and notes the firmware was specifically designed for ATmega328p but should run on any 28-pin ATmega ≥ 8 KB with WinAVR; this repository is a mirror/fork.

## Stack

- Language: C (avr-gcc / avr-libc).
- Target: ATmega328p, `F_CPU = 1 MHz`, fuses `LFUSE=0x62`, `HFUSE=0xD1`.
- Build: GNU `make` against `trunk/firmware/Makefile` (output `TransistorTestNew.hex`).
- Programmer: USBasp via avrdude (configured in the Makefile).
- Hardware: HD44470-compatible 16x2 LCD (`TUXGR_16X2_R2`), BC547/BC557 BJTs, 78L05 LDO, USB / 9V DC input.

## Build / Run

```
cd trunk/firmware
make                # build TransistorTestNew.hex / .elf
make program        # flash via avrdude + USBasp (see Makefile targets)
```

Atmel Studio users can also open `trunk/firmware/avr-tester/avr-tester.atsln`.

## Layout

- `trunk/firmware/` — main Makefile, build artifacts, Atmel Studio project (`avr-tester/`).
- `trunk/firmware/avr-tester/avr-tester/` — sources, `chars.h`, `VALUES.xlsx` calibration data, `examples/`, `f/` LCD routines, `hardware/` (Eagle + Altium + Gerbers).
- `trunk/Documents/` — `SCHEMATIC.pdf`, `BOARD LAYOUT.pdf`, `PARTLIST.TXT`.
- `Last Loaded avr-tester-1602.DBK`, `avr-tester-1602.DSN`, `avr-tester-1602.PWI` — Proteus simulation files at the repo root.
- `finished/avr-tester.rar` — packaged build.
- `.svn/` — preserved Subversion metadata from the original `code.google.com/p/avr-component-tester` project.

## Status

Archived in spirit (last commit December 2012). Mirror of an external SVN project by BrentBXR / MickM; kept for reference.
