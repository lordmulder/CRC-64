MACHINE := $(shell $(CC) -dumpmachine || echo unknown)

ifneq (,$(firstword $(filter x86_64-%,$(MACHINE))))
  MARCH ?= x86-64
  MTUNE ?= nocona
else
  ifneq (,$(firstword $(filter i686-%,$(MACHINE))))
    MARCH ?= i586
    MTUNE ?= pentium2
  endif
endif

CFLAGS = -Wall -std=gnu99 -O3 -DNDEBUG

ifneq (,$(MARCH))
  CFLAGS += -march=$(MARCH)
endif
ifneq (,$(MTUNE))
  CFLAGS += -mtune=$(MTUNE)
endif

ifneq (,$(firstword $(filter %mingw32 %-windows-gnu %-cygwin %-cygnus,$(MACHINE))))
  OUTNAME := crc64.exe
else
  OUTNAME := crc64
endif

ifneq (,$(firstword $(filter %-w64-mingw32 %w64-windows-gnu,$(MACHINE))))
  CFLAGS += -D__USE_MINGW_ANSI_STDIO=0 -mconsole -municode
endif

CFLAGS += -s -static

.PHONY: all

all:
	$(CC) $(CFLAGS) -o $(OUTNAME) crc64.c
