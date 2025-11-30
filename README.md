# Ouroboros-Quantum-RFID-Honeypot
Tired of the chumps, come smell my butt.
# Ouroboros Quantum RFID Honeypot
∞-loop trap that feeds quantum/post-quantum skimmers infinite fake perfect credentials  
Costs attackers months of GPU time and petabytes of storage while your real key never leaves the secure element.

100 % free · No cloud · No subscription · Runs forever on a $25 ESP32 + PN532

Built by Slidewalker – 2025

## Features
- Real card is physically RF-disabled the moment a reader is detected
- Instantly serves millions of completely valid-looking but fake cards per second
- Works against MIFARE Classic, Ultralight, DESFire, NTAG, HID Prox, iClass
- Post-quantum safe seed (Kyber-style ratchet built from BLAKE3)
- Optional billy-club RF burn mode

## Hardware (≈ $22 total on AliExpress)
- ESP32-S3 dev board (any)
- PN532 NFC module (UART/HSU version preferred)
- Optional second PN532 or ATECC608B if you want the real key truly air-gapped

## Wiring (PN532 HSU ↔ ESP32)
PN532    →   ESP32
VCC      →   3.3 V
GND      →   GND
TX       →   GPIO16 (RX on ESP32)
RX       →   GPIO17 (TX on ESP32)

License: MIT – Do whatever you want with it, sell it, keep it, give it away.
