### makefile to build test application for Sitara ARM Processor AM3352
##COMPILATOR for USE
CC=/opt/ti/gcc-linaro-arm-linux-gnueabihf-4.7-2013.03-20130313_linux/bin/arm-linux-gnueabihf-gcc
#application name
APPNAME = abarmapp


#//////////////////////////////////////DEBUGGING FLAGS////////////////////////////
DEBUG = -g2

#//////////////////////////////////////OPTIMIZATION FLAGS/////////////////////////
OPT = -fexpensive-optimizations	 -frename-registers

C_CPFLAGS = $(OPT) $(DEBUG) -fPIC 
ARFLAGS   = -shared


###########################Директории для объектных файлов.#######################
BASE_DIR     =  /mnt/SHARE/SITARA3552_Ab/target_board/app/linux/
LIBSIP_DIR   =  /mnt/SHARE/SITARA3552_Ab/target_board/app/linux/sip/src
LIBSIPSO_DIR =  /mnt/SHARE/SITARA3552_Ab/target_board/app/linux/sip/lib


#LIBS =  $(LIBSIP_DIR)/Abarm_sip_test_app.o  $(LIBSIPSO_DIR)/libsip_call.so
LIBS =  $(BASE_DIR)Abarm_app_main.o  $(LIBSIPSO_DIR)/libsip_call.so


#Файлы .SO библиотеки libsip_call.so
SO_LIBGENERIC =  $(LIBSIPSO_DIR)/libsip_call.so
#Файлы o для сборки библиотеки libsip_call.so
SRO_LIBGENERIC = $(LIBSIP_DIR)/sip_call.o  $(LIBSIP_DIR)/digcalc.o $(LIBSIP_DIR)/md5.o




#Файлы СИ для сборки библиотеки libsip_call.so
SRC_LIBGENERIC = $(LIBSIP_DIR)/sip_call.c  $(LIBSIP_DIR)/digcalc.c  $(LIBSIP_DIR)/md5.c 


SRO_LIBIN = $(LIBSIP_DIR)/Abarm_sip_test_app.o
SRC_LIBIN = $(LIBSIP_DIR)/Abarm_sip_test_app.c


SRO_MAIN  = $(BASE_DIR)Abarm_app_main.o
SRC_MAIN  = $(BASE_DIR)Abarm_app_main.o


#Работает собираем библиотеки нормально вроде
#binary: test.o libsip_call.so
#	gcc -o binary test.o -L. -lsip_call -Wl,-rpath,.

#test.o: test.c
#	gcc -c test.c

#Нужен ещё один подуровень для этой библиотеки сейчас сделаю его.





#Цель              Реквизит
Abarm_app_main:$(LIBS)
	$(CC) -o $(BASE_DIR)Abarm_app_main $(SRO_MAIN) -L/mnt/SHARE/SITARA3552_Ab/target_board/app/linux/sip/lib/ -lsip_call -Wl,-rpath,.
$(SRO_MAIN):$(SRC_MAIN)
	$(CC) $(C_CPFLAGS) -o $(BASE_DIR)Abarm_app_main.o $(BASE_DIR)Abarm_app_main.c -c	
$(SO_LIBGENERIC):$(SRO_LIBGENERIC)	
		$(CC) $(ARFLAGS) -o $(LIBSIPSO_DIR)/libsip_call.so $(SRO_LIBGENERIC)
$(SRO_LIBGENERIC) : $(SRC_LIBGENERIC)		
		$(CC) $(C_CPFLAGS) -o $(LIBSIP_DIR)/sip_call.o $(LIBSIP_DIR)/sip_call.c -c
		$(CC) $(C_CPFLAGS) -o $(LIBSIP_DIR)/digcalc.o $(LIBSIP_DIR)/digcalc.c -c
		$(CC) $(C_CPFLAGS) -o $(LIBSIP_DIR)/md5.o $(LIBSIP_DIR)/md5.c -c















#Работает эта штука промежуточный вариант для меня пока.
#Цель              Реквизит
#libsip_call.so:$(SRO_LIBGENERIC)	
#		$(CC) $(ARFLAGS) -o $(LIBSIPSO_DIR)/libsip_call.so $(SRO_LIBGENERIC)
#$(SRO_LIBGENERIC) : $(SRC_LIBGENERIC)		
#		$(CC) $(C_CPFLAGS) -o $(LIBSIP_DIR)/sip_call.o $(LIBSIP_DIR)/sip_call.c -c
#		$(CC) $(C_CPFLAGS) -o $(LIBSIP_DIR)/digcalc.o $(LIBSIP_DIR)/digcalc.c -c
#		$(CC) $(C_CPFLAGS) -o $(LIBSIP_DIR)/md5.o $(LIBSIP_DIR)/md5.c -c
