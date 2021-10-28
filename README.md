# Xbox 360 Controller (XInput) Polling Rate Checker
[![GitHub Release](https://img.shields.io/github/release/chrizonix/XInputTest.svg)](https://github.com/chrizonix/XInputTest/releases/tag/v1.2.0.0)
[![Github Downloads](https://img.shields.io/github/downloads/chrizonix/XInputTest/total.svg)](https://github.com/chrizonix/XInputTest/releases/tag/v1.2.0.0)
[![Github Commits (since latest release)](https://img.shields.io/github/commits-since/chrizonix/XInputTest/latest.svg)](https://github.com/chrizonix/XInputTest/compare/v1.2.0.0...master)
[![GitHub Repo Size in Bytes](https://img.shields.io/github/repo-size/chrizonix/XInputTest.svg)](https://github.com/chrizonix/XInputTest)
[![Github License](https://img.shields.io/github/license/chrizonix/XInputTest.svg)](LICENSE.md)

Simple Tool to Check the Latency and Update Rate of a Xbox 360 Controller, or Virtual XInput Device (DualShock 3/4)

## Changelog v1.2.0.0 ([Latest Version](https://github.com/chrizonix/XInputTest/releases/tag/v1.2.0.0))
* Added Filter for Latencies below 0.5 ms
* Added 64-bit Binaries

## Changelog v1.0.0.2 ([Download](https://github.com/chrizonix/XInputTest/releases/tag/v1.0.0.2))
* Added Rapid Outlier Detection (thanks to Mahito Sugiyama)
* Added XInput Controller Class (thanks to Minalien)
* Added Jitter Calculation

## Why?
I wanted to measure the Latency of my DualShock 3/4 Controllers via USB and Bluetooth, but haven't found a Tool which let me do this. Although you can find Videos and Articles about the Update Rates, I wanted to check them by myself.

I'm using the XInput Library, because most of the Games are using it for Xbox 360 Controllers.
And I wanted to measure the Latency that the Game is "seeing". Furthermore it allowed me to test various Controllers, without rewriting Code.

## How Does It Work?
This Tool polls the Gamepad Status several thousand Times a Second, and measures the Time between two distinct Gamepad Reports, aggregates the Data (Average, Min, Max), calculates the Jitter and detects Hiccups (Outliers).

Although you might get slightly different Results every Time you run the Test (because of the Distance to your Dongle, Hardware, etc) it's generally a good way to compare the Performance of your Gamepad and/or Bluetooth Devices.

## Credits
* [Xbox 360 Controller Icon by Mark Davis](https://thenounproject.com/term/xbox-360-controller/195040/)
* [Xbox 360 Controller Input in C++ with XInput by Minalien](https://www.codeproject.com/Articles/26949/Xbox-Controller-Input-in-C-with-XInput)
* [Rapid distance-based Outlier Detection via Sampling by Mahito Sugiyama](https://github.com/BorgwardtLab/sampling-outlier-detection)
