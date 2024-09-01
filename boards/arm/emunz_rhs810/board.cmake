# Copyright (c) 2024 Mr E Munz
# All rights reserved

set(OPENOCD_NRF5_SUBFAMILY "nrf52")
board_runner_args(jlink "--device=nRF52810_xxAA" "--speed=4000")
board_runner_args(nrfjprog "--softreset")
include(${ZEPHYR_BASE}/boards/common/nrfjprog.board.cmake)
include(${ZEPHYR_BASE}/boards/common/nrfutil.board.cmake)
include(${ZEPHYR_BASE}/boards/common/jlink.board.cmake)
include(${ZEPHYR_BASE}/boards/common/openocd-nrf5.board.cmake)

