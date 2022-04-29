#pragma once

#include <kernel/types.h>
#include <kernel/io/fdo.h>
#include "../pic.h"
#include "interrupts.h"

void ps2kbd_handler(interrupt_ctx_t regs);
