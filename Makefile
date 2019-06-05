PCANDRV_DIR := ./peak/driver
PCANLIB_DIR := ./peak/lib
SRC := peak
CJSON_SRC := cjson
CAN_SRC := can
TIMER_SRC := timer
LOG_SRC := log
EVENT_SRC := event
INC := -I. -I$(PCANLIB_DIR) -I$(PCANDRV_DIR)
LDLIBS := -L$(PCANLIB_DIR)/lib
DBGFLAGS := -g



CFLAGS := -D$(RT) $(DBGFLAGS) $(INC) #$(USERAPP_CFLAGS) $(OPTS_CFLAGS) $(COMMON_FLAGS)
LDFLAGS := $(LDLIBS) #$(OPTS_LDFLAGS) $(COMMON_FLAGS)

TARGET := nm
FILES := main.c $(SRC)/peakcanrx.c $(CAN_SRC)/canasc.c $(CAN_SRC)/canrxbuffer.c $(CAN_SRC)/canrxthread.c \
		$(CAN_SRC)/cansignalparse.c $(CAN_SRC)/candatabase.c $(CJSON_SRC)/cJSON.c $(TIMER_SRC)/nmtime.c \
		$(LOG_SRC)/ascname.c $(LOG_SRC)/nmsyslog.c $(EVENT_SRC)/eventdatabase.c $(EVENT_SRC)/eventmonitor.c \
		$(EVENT_SRC)/eventbuffer.c $(LOG_SRC)/nmsysdiag.c\

ALL = $(TARGET)

all: $(ALL)

obj = $(FILES:%.c=%.o)

$(TARGET): $(obj)
	$(CC) $(CFLAGS) $^ -lpcanfd -lpthread -lstdc++ -lm $(LDFLAGS) -o $(TARGET)

%.o: %.c
	$(CC) -c $< -o $@

clean:
	-rm -f $(SRC)/*~ $(SRC)/*.o *~ $(CJSON_SRC)/*~ $(CJSON_SRC)/*.o *~ $(ALL) \
	*~ $(CAN_SRC)/*~ $(CAN_SRC)/*.o *~ $(EVENT_SRC)/*~ $(EVENT_SRC)/*.o \
	*~ $(LOG_SRC)/*~ $(LOG_SRC)/*.o *~ $(TIMER_SRC)/*~ $(TIMER_SRC)/*.o \
	*~ $(SHELL_SRC)/*~ $(SHELL_SRC)/*.o *~ main.o

