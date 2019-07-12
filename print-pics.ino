/*
 * Print some pictures on a thermal receipt printer.
 *
 * For M5Stack (ESP32) with USB host module (max3421e). This is a good
 * solution because the M5Stack has an internal battery. When used with a
 * portable printer (battery inside), only a USB cable is required to
 * connect the two.
 *
 * For Arduino boards MKR WiFi 1010, MKR WiFi 1000, and Adafruit Feather M0
 * with ATWINC15000. Only tested on MKR WiFi 1010 so far. Does not use WiFi
 * so SAMD21 boards without WiFi (for example, MKR Zero) may also work.
 *
 * When using the MKR micro USB port in USB host mode, debug/serial console
 * output must go out via UART TX (Serial1).
 *
 * A USB OTG to USB host cable must be used to connect a USB printer.
 *
 * When using USB host on USB OTG, double click the RESET button to upload.
 */

/*
 * MIT License
 *
 * Copyright (c) 2019 gdsports625@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#if defined(ARDUINO_M5Stack_Core_ESP32)
#include <M5Stack.h>
#endif

#include "USBPrinter.h"
#include "ESC_POS_Printer.h"
#if defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_ESP32)
#include "qrcode.h"
#include "wave.h"
#include "uno.h"
#include "mkr1010.h"
#include "hackaday.h"
#include "octocat.h"
#else
#include "qrcode.h"
#include "uno.h"
#endif

#if defined(ADAFRUIT_FEATHER_M0)
// This board is similar to the MKR WiFi 1000. Let's assume if the
// board type is Adafruit Feather M0, it has the ATWINC1500 WiFi module.
#define ADAFRUIT_FEATHER_ATWINC1500
#endif

#if defined(ARDUINO_ARCH_SAMD)
#define USBHOSTCLASS USBHost
#ifdef ARDUINO_SAMD_ZERO
#define DBSerial  Serial
#else
#define DBSerial  Serial1
#endif
#endif

#ifndef USBHOSTCLASS
#define USBHOSTCLASS USB
#endif

#ifndef DBSerial
#define DBSerial Serial
#endif

class PrinterOper : public USBPrinterAsyncOper
{
  public:
    uint8_t OnInit(USBPrinter *pPrinter);
};

uint8_t PrinterOper::OnInit(USBPrinter *pPrinter)
{
  DBSerial.println(F("USB Printer OnInit"));
  DBSerial.print(F("Bidirectional="));
  DBSerial.println(pPrinter->isBidirectional());
  return 0;
}

USBHOSTCLASS myusb;
PrinterOper AsyncOper;
USBPrinter uprinter(&myusb, &AsyncOper);
ESC_POS_Printer printer(&uprinter);

void setup() {
#if defined(ARDUINO_M5Stack_Core_ESP32)
  M5.begin();
  M5.Lcd.print("pics.ino");
#endif
  //Initialize serial and wait for port to open:
  DBSerial.begin(115200);
  if (myusb.Init())
    Serial.println(F("USB host failed to initialize"));
  delay(100);
  DBSerial.println();
}

bool printed = false;

void loop() {
  myusb.Task();

  if (!printed && uprinter.isReady()) {
    printer.begin();
#if defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_ESP32)
    printer.printBitmap(qrcode_width, qrcode_height, qrcode_data, qrcode_density);
    printer.printBitmap(mkr1010_width, mkr1010_height, mkr1010_data, mkr1010_density);
    printer.printBitmap(uno_width, uno_height, uno_data, uno_density);
    printer.printBitmap(wave_width, wave_height, wave_data, wave_density);
    printer.printBitmap(octocat_width, octocat_height, octocat_data, octocat_density);
    printer.printBitmap(hackaday_width, hackaday_height, hackaday_data, hackaday_density);
#else
    printer.printBitmap_P(qrcode_width, qrcode_height, qrcode_data, qrcode_density);
    printer.printBitmap_P(uno_width, uno_height, uno_data, uno_density);
#endif
    printer.feed(3);
    printed = true;
  }
}
