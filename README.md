# RTSP Proxy Server for ESP32-C3

This is an implementation of a RTSP-to-WebSocket proxy that is optimimized for a single core ESP32-C3 running at 240MHz.

This is **not a full implementation** of the RTSP client spec, ***neither is it production-ready code***. Your mileage ***will*** vary.

## Library Patching

This codebase patches the ESPAsyncWebServer library with custom constants to make it compile with PlatformIO. It runs a Python script to do string pattern replacement, and is therefore extremely sensitive to library changes.

## Watchdog Timer

If the camera fails to connect in time, the RTOS watchdog timer trips and resets the ESP32. This is typically caused be a RTSP protocol error, please check if your camera supports UDP streams.

## Camera Support

This library was built with the express purpose of proxying the TP-Link Tapo C216 camera stream. It will probably require tweaks to make it work with other cameras.
