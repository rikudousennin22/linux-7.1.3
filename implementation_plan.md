# Fix RTD1619 Link Flapping in r8169soc_rtd16xx.c

The user reported intermittent link flapping (link down/up every few seconds) on the RTD1619 gigabit ethernet port.
This is a known issue often caused by Energy Efficient Ethernet (EEE) putting the PHY to sleep and causing renegotiation loops with certain switches.

## Proposed Changes
### drivers/net/ethernet/realtek/r8169soc_rtd16xx.c
Disable EEE by default by modifying the initialization logic.
- We will force 	p->eee_enable = false; regardless of device tree settings.

## Open Questions
- Did you see the link flapping continuously in dmesg, or just once when restarting the network? If it only happened once, it's normal behavior. But disabling EEE is a good safety measure regardless.

## Verification Plan
Compile the kernel and monitor dmesg to ensure eth0 link remains stable.
