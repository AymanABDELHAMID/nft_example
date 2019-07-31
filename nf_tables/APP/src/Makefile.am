AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = $(ACLOCAL_FLAGS)

bin_PROGRAMS = nft
nft_SOURCES = \
	Main.c \
	cli.c \ 
	nft_opt.c \ 
	packetClassification.c \
	../../Common/Logger/console_log.c \
	../../Common/Logger/time_logger.c \
	../../Common/Logger/BufferSerializer.c \
	../../Common/Logger/MmsLogHeaderGenerator.c \
	../../Common/Logger/Logger.c \
	../../Common/Logger/mms_socket_rx.c \
	../../Common/Logger/mms_socket.c \
	../../Common/Logger/helper_functions.c \
	./common.h \
	../../Common/Logger/mms_socket.h \
	../../Common/Logger/MmsLogHeaderGenerator.h \
	../../Common/Logger/time_logger.h \
	../../Common/Logger/BufferSerializer.h \
	../../Common/Logger/Logger.h \
	../../Common/Logger/helper_functions.h \
	../../Common/Logger/console_log.h \
	../../Common/Logger/mms_socket_rx.h \
	../../Common/helper_macros.h \
	../../Common/types.h 

nft_LDADD = \
	/usr/local/lib/libmnl.so \ 
	/usr/local/lib/libnftnl.so 




