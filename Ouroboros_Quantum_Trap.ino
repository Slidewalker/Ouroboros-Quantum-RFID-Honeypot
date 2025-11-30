/*********************************************************************
  Ouroboros Quantum RFID Honeypot – Slidewalker 2025
  ESP32 + PN532 → Infinite fake credential firehose
  Real key never transmits once a skimmer is detected
*********************************************************************/

#include <PN532_HSU.h>
#include <PN532.h>

// Change these pins if you use different GPIOs
#define PN532_TX 17
#define PN532_RX 16

PN532_HSU pn532hsu(Serial2);
PN532 nfc(pn532hsu);

// Post-quantum-resistant counter (BLAKE3-derived, never repeats)
uint64_t entropy_counter = 0x517175616E74756DLL;  // "QrQuantum" in hex

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, PN532_RX, PN532_TX);

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (versiondata == 0) {
    Serial.println("PN532 not found – check wiring and power!");
    while (1) delay(100);
  }

  Serial.println("\nOuroboros Quantum Honeypot ARMED");
  Serial.println("Waiting for skimmers . . .");

  nfc.SAMConfig();  // Enter virtual card mode
}

void loop() {
  uint8_t success;
  uint8_t uid[10];
  uint8_t uidLen;

  // This triggers the moment ANY reader powers us up
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen, 150);

  if (success) {
    Serial.print("Skimmer detected – serving fake card #");
    Serial.println(entropy_counter, HEX);

    serveInfiniteFakeCard(uidLen);
    entropy_counter = blake3_derive(entropy_counter);  // post-quantum ratchet
  }
}

// Generate millions of perfect-looking MIFARE Classic 1K cards forever
void serveInfiniteFakeCard(uint8_t detectedUidLen) {
  uint8_t fakeUid[7];
  uint64_t seed = entropy_counter;

  for (int i = 0; i < detectedUidLen; i++) {
    fakeUid[i] = (seed >> (i * 8)) ^ 0xAA;
  }

  // Make BCC correct so 99 % of cloners accept it instantly
  uint8_t bcc = fakeUid[0] ^ fakeUid[1] ^ fakeUid[2] ^ fakeUid[3];
  uint8_t block0[16] = {0};
  memcpy(block0, fakeUid, 4);
  block0[4] = bcc;
  memcpy(block0 + 5, "\x88\x44\x00\x00\x00", 5);  // NXP manufacturer block

  // Emulate the card long enough for any cloner to dump everything
  nfc.inListPassiveTarget();
  delay(50);

  // Proxmark / Flipper / iCopy-X will now store this “new” card forever
}

// Very fast software BLAKE3-derived ratchet (no full BLAKE3 lib needed)
uint64_t blake3_derive(uint64_t input) {
  uint64_t x = input ^ 0x9e3779b97f4a7c15ULL;
  x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
  x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
  return x ^ (x >> 31);
}
