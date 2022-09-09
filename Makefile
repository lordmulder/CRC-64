MACHINE := $(shell $(CC) -dumpmachine || echo unknown)

ifneq (,$(firstword $(filter x86_64-%,$(shell $(CC) -dumpmachine))))
  MARCH ?= x86-64
  MTUNE ?= nocona
else
  MARCH ?= i586
  MTUNE ?= pentium2
endif

CFLAGS = -Wall -std=gnu99 -O3 -march=$(MARCH) -mtune=$(MTUNE) -DNDEBUG

ifneq (,$(firstword $(filter %mingw32 %-windows-gnu %-cygwin %-cygnus,$(MACHINE))))
  SUFFIX := .exe
else
  SUFFIX :=
endif

ifneq (,$(firstword $(filter %-w64-mingw32 %w64-windows-gnu,$(MACHINE))))
  CFLAGS += -D__USE_MINGW_ANSI_STDIO=0 -mconsole -municode
endif

CFLAGS += -s -static

.PHONY: all

all:
	$(CC) $(CFLAGS) -o crc64$(SUFFIX) crc64.c
