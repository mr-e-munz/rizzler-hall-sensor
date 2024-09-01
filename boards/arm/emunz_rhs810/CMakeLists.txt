# Copyright (c) 2024 Mr E Munz
# All rights reserved

# The emunz_rhs811 board mirrors the nRF52 / nRF52810 DK hardware. This
# needs to be considered by certain system initialization functionality
# residing in system_nrf52811.c and SoC dependent routines in nrfx_coredep.h.
zephyr_compile_definitions(DEVELOP_IN_NRF52810)
zephyr_compile_definitions(NRFX_COREDEP_DELAY_US_LOOP_CYCLES=3)
