# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(rizz_hall_sensor)

FILE(GLOB app_sources
 src/main.c
 src/hall_sensor.c
 src/led_ctl.c
 src/ble.c
 src/bluetooth_adv.c
 #src/logger.c
)

target_sources(app PRIVATE
  ${app_sources}
  )

zephyr_library_include_directories(${ZEPHYR_BASE}/samples/bluetooth)
