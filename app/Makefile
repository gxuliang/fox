#
#  "$Id: Makefile 17 2011-11-07 14:54:16Z xul $"
#
#  Copyright (c)1992-2007,  CO.LTD.
#  All Rights Reserved.
#
#	Description:	
#	Revisions:		Year-Month-Day  SVN-Author  Modification
#

include ../CONFIG

GCC = $(CROSS)g++

EXEC = ../Bin/app

SRC = ./src/
INC = ./inc/
COM_INC = ../
OBJ = ./obj/

ifeq ($(CROSS), )
	PLAT = pc
else
	PLAT = arm
endif
LIBS = ./libs/$(PLAT)/libjson.a	\
		./libs/$(PLAT)/libjsonrpc.a	\
		
		
OBJS = 	$(OBJ)sy_semaphore.o \
		$(OBJ)sy_debug.o	\
		$(OBJ)sy_net.o	\
		$(OBJ)sy_configManager.o	\
		$(OBJ)sy_file.o		\
		$(OBJ)sy_mutex.o	\
		$(OBJ)sy_user.o		\
		$(OBJ)sy_netrpc.o	\
		$(OBJ)sy_rpcmethod.o	\
		$(OBJ)sy_thread.o		\
		$(OBJ)sy_printer.o		\
		$(OBJ)sy_netService.o		\
		$(OBJ)sy_device.o		\
		$(OBJ)sy_upgrade.o		\
		$(OBJ)sy_main.o	\
				
				
$(OBJ)%.o: $(SRC)/%.cpp
	$(GCC) -g -I$(INC) -I$(COM_INC) -c -o $(OBJ)$*.o $<
	
default:	$(OBJS)
	$(GCC) -O0 -o $(EXEC) $(OBJS) $(LIBS) -lpthread 
	cp $(EXEC) ~/sybin/app
	
clean:
	rm -rf $(EXEC) $(OBJ)*.o
