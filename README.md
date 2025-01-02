# Coreboot Support for Lenovo ThinkCentre M93p
The ThinkCentre M93p uses the ITE IT8733F Super I/O chip. Documentation is behind a NDA. For Coreboot, the only needed functions are disabling the watchdog timer and serial port. I’ve repurposed the IT8728F support since it works, though I’m unsure of the differences between the IT8728F and IT8733F.  

Tested on Debian 12.8.0 (kernel 6.1.0-28)

**Working Features:**  
- USB  
- Sound (Headphone and Line Out. Internal speaker not working)
- Ethernet  
- SATA  
- Suspend  
- Serial Port  
- iGPU with 3D acceleration
- Intel VT
- Internal flashing after coreboot is installed
- CPU Temp (coretemp)
- Poweroff
- Intel UEFI PEI System Agent (mrc.bin)
- Haswell Native raminit


**Untested:**  
- PCIe  
- PCI-to-PCIe bridge  
- Parallel port  
- TPM
- Windows
- Intel ME (I have it purposely disabled)

**Not Working:**
- Automatic Fan Control (Lenovo Intelligent Cooling Engine)
- HDD Activity Light

- This may work on the M73 and M83, as they seem similar, but I have not tested. You may need to check for the flash size since the M93p supports AMT and therefore has a 8+4MB config to support the extended Intel ME partition.
  
To use haswell-nri (one less blob!)
```bash
git fetch https://review.coreboot.org/coreboot refs/changes/48/81948/2
git checkout -b change-81948 FETCH_HEAD
```

## Build Configuration

Start by running `make menuconfig` and configure the following settings:

```
Mainboard
  Mainboard Vendor Name -> Lenovo
  Mainboard part number > ThinkCentre M93p
  Size of CBFS filesystem in ROM -> 0x00680000

Chipset
  [*] Use native raminit
  [*] Add Intel descriptor.bin file
  [*] Add Intel ME/TXE firmware 
  [*] Strip down the Intel ME/TXE firmware
  [*] Add gigabit ethernet configuration

Devices
  Graphics initialization -> Use libgfxinit
  [*] Use onboard VGA as primary video device
  Display -> Framebuffer mode -> Linear "high-resolution" framebuffer
  [*] Add a Video BIOS Table (VBT) binary to CBFS
  [ ] Allocate resources from top down
  [ ] Support for flash based event log
  
Generic Drivers
  [*] Serial port on SuperIO

Console
  [*] Enable early (bootblock) console output
  [*] Enable console output during postcar  
  [*] Squelch AP CPUs from early console
  [ ] SPI Flash console output
```

### Payload
Both edk2 and seabios have been tested successfully. Note that edk2 requires "Allocate resources from top down" to be disabled.

## Flashing Instructions

This board uses a split ROM configuration (8MB + 4MB) for AMT-enabled platforms.

1. Split the ROM file:
```bash
dd if=coreboot.rom of=primary.bin bs=1M count=8
dd if=coreboot.rom of=secondary.bin bs=1M skip=8 count=4
```

2. Flash the primary ROM (U4 - 8MB):
```bash
flashrom --programmer=(your programmer here) -w primary.bin
```

3. Flash the secondary ROM (U5 - 4MB):
```bash
flashrom --programmer=(your programmer here) -w secondary.bin
```
