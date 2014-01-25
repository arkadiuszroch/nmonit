CC=gcc
CXX=g++
RM=rm
RMFLAGS=-f
CFLAGS=-Wall -g
SQLCFLAGS=-Wall -DSQLITE_OMIT_LOAD_EXTENSION -DSQLITE_THREADSAFE=2 -c
LIBS=-lpthread -lpcap -lncurses
SQLLIBS=-lpthread 
SOURCES=nmonit.cpp classes/iniParser/iniVar.cpp classes/iniParser/iniVarList.cpp classes/iniParser/iniSection.cpp classes/iniParser/iniSectionList.cpp classes/iniParser.cpp functions/global.cpp classes/exception.cpp classes/config.cpp classes/stream.cpp classes/debug.cpp classes/objects/devices/device.cpp classes/objects/devices/deviceList.cpp classes/objects/users/user.cpp classes/objects/users/userList.cpp classes/objects/programs/program.cpp classes/objects/programs/programList.cpp classes/objects/remotes/remote.cpp classes/objects/remotes/remoteList.cpp classes/connection.cpp classes/protocol.cpp classes/protocols/tcp.cpp classes/protocols/udp.cpp classes/ui.cpp classes/database.cpp functions/mainFunctions.cpp functions/packetCapture.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=nmonit
USER=$(shell whoami)
NCURSES=$(shell locate libncurses.so | head -n 1)
PCAP=$(shell locate libpcap.so | head -n 1)
LIB_PATH=/usr/lib
ETC_PATH=/etc/nmonit
BIN_PATH=/usr/bin
NMONIT_PATH=$(LIB_PATH)/nmonit
DATA_PATH=$(NMONIT_PATH)/data
TEST_PATH=$(NMONIT_PATH)/test
TEST_DATA_PATH=$(TEST_PATH)/data
WEB_PATH=$(NMONIT_PATH)/web
NMONIT_FILE=$(NMONIT_PATH)/nmonit
INIT_FILE=/etc/init.d/nmonit
CONFIG_FILE=$(NMONIT_PATH)/.config
TEST_CONFIG_FILE=$(TEST_PATH)/.config
PID_FILE=/var/run/nmonit.pid

all: main_requiments compile_sqlite $(SOURCES) $(EXECUTABLE)
	@echo 
	@echo "build complete"
	@echo

compile_sqlite:
	$(CC) $(SQLCFLAGS) classes/sqlite/shell.c classes/sqlite/sqlite3.c $(SQLLIBS)

$(EXECUTABLE): $(OBJECTS) 
	$(CXX) $(CFLAGS) -o $@ $(OBJECTS) sqlite3.o $(LIBS)

clean: 
	$(RM) $(RMFLAGS) $(OBJECTS)
	$(RM) $(RMFLAGS) sqlite3.o shell.o
	@echo
	@echo "clean complete"
	@echo

install: check_root check_compiled requiments_daemon create_dirs copy_files

	@echo "NMonit instalation complete"

check_root: 
	@if [ "$(USER)" != "root" ] ; then \
	    echo "You must be the root to install NMonit"; \
	    false; \
	fi
	
check_compiled:
	@if [ ! -f "nmonit" ] ; then \
	    echo "Please use make first"; \
	    false; \
	fi
	
create_dirs:
	
	@echo "Cheking $(LIB_PATH)";
	@if [ ! -d "$(LIB_PATH)" ];  then \
	    echo "Error: Directory $(LIB_PATH) doesn't exists"; \
	    false; \
	fi
	
	@echo "Create $(NMONIT_PATH)";
	@if [ ! -d "$(NMONIT_PATH)" ]; then \
	    mkdir "$(NMONIT_PATH)"; \
	fi
	
	@echo "Create $(DATA_PATH)";
	@if [ ! -d "$(DATA_PATH)" ]; then \
	    mkdir "$(DATA_PATH)"; \
	fi 
	
	@echo "Create $(TEST_PATH)";
	@if [ ! -d "$(TEST_PATH)" ]; then \
	    mkdir -p "$(TEST_PATH)"; \
	fi 
	
	@echo "Create $(TEST_DATA_PATH)";
	@if [ ! -d "$(TEST_DATA_PATH)" ]; then \
	    mkdir "$(TEST_DATA_PATH)"; \
	fi 
	
copy_files:
	@echo "Copy $(NMONIT_FILE)";
	@if [ ! -f "$(NMONIT_FILE)" ]; then \
	    cp "./nmonit" "$(NMONIT_FILE)"; \
	    chmod +x "$(NMONIT_FILE)"; \
	fi
	
	@echo "Linking nmonit to $(BIN_PATH)";
	@ln -s "$(NMONIT_FILE)" "$(BIN_PATH)/nmonit";
	
	@echo "Copy $(WEB_PATH)";
	@if [ ! -d "$(WEB_PATH)" ]; then \
	    cp -R "./web" "$(WEB_PATH)"; \
	fi
	
	@chmod -R 0755 "$(WEB_PATH)";
	@chmod 0777 "$(WEB_PATH)/data";
	@chmod 0777 "$(WEB_PATH)/cache";
	
	@echo "Copy init file";
	@if [ ! -f "$(INIT_FILE)" ]; then \
	    cp -R "./nmonit.init.sh" "$(INIT_FILE)"; \
	    chmod +x "$(INIT_FILE)"; \
	    update-rc.d nmonit defaults; \
	fi
	
	@echo "Setting configuration";	
	@if [ ! -d "$(ETC_PATH)" ]; then \
	    mkdir "$(ETC_PATH)"; \
	fi
	
	@/usr/lib/nmonit/nmonit -ci -a
	
	@echo "Setting webservice configuration"; 
	@ln -s "$(ETC_PATH)/presenter.conf" "/etc/apache2/sites-available/nmonit.host";
	@ln -s "$(ETC_PATH)/presenter.conf" "/etc/apache2/sites-enabled/nmonit.host";
	
	@echo "Restarting apache";
	@/etc/init.d/apache2 reload
	
	@echo "Starting NMonit";
	@/etc/init.d/nmonit start
	
uninstall:
	@echo "Stopping NMonit if running"
	@if [ -e "$(PID_FILE)" ]; then \
	    /etc/init.d/nmonit stop; \
	fi;
	
	@echo "Delete init file";
	@rm -R "$(INIT_FILE)";
	@update-rc.d nmonit remove
	
	@echo "Delete $(NMONIT_PATH)"; 
	@rm -R "$(NMONIT_PATH)";
	
	@echo "Delete $(BIN_PATH)/nmonit";
	@rm -R "$(BIN_PATH)/nmonit";
	
	@echo "Delete $(ETC_PATH)"; 
	@rm -R "$(ETC_PATH)";
	
	@echo "Deleting webservice configuration"; 
	@rm -R "/etc/apache2/sites-available/nmonit.host";
	@rm -R "/etc/apache2/sites-enabled/nmonit.host";
	
	@echo "Restarting apache";
	@/etc/init.d/apache2 reload;
	
	@echo "NMonit was removed";
	
	
requiments:
	@echo "Cheking main requiments: ";
	@echo " checking libpcap: ";
	@if [ "$(PCAP)" = "" ]; then \
	    echo " * libpcap - FAIL"; \
	else \
	    echo " * libpcap - OK"; \
	fi
	
	@echo " checking libncurses:";
	@if [ "$(NCURSES)" = "" ]; then \
	    echo " * libncurses - FAIL";\
	else \
	    echo " * libncurses - OK"; \
	fi
	
	@echo "Cheking webservice requiments";
	@echo " checking apache2: ";
	@if [ ! -d "/etc/apache2/sites-available" ] || [ ! -d "/etc/apache2/sites-enabled" ]; then \
	    echo " * apache2 - FAIL"; \
	else \
	    echo " * apache2 - OK"; \
	fi
	
	@echo " checking php5 ";
	@if [ ! -d "/etc/php5" ]; then \
	    echo " * php5 - FAIL"; \
	else \
	    echo " * php5 - OK"; \
	fi
	
main_requiments:
	@if [ "$(PCAP)" = "" ]; then \
	    echo "pcap library is required"; \
	    false; \
	fi
	
	@if [ "$(NCURSES)" = "" ]; then \
	    echo "ncurses library is required"; \
	    false; \
	fi
requiments_daemon: 
	@if [ ! -d "/etc/apache2/sites-available" ] || [ ! -d "/etc/apache2/sites-enabled" ]; then \
	    echo "Apache2 is required"; \
	    false; \
	fi
	@if [ ! -d "/etc/php5" ]; then \
	    echo "PHP 5.3 is required"; \
	    false; \
	fi
	
