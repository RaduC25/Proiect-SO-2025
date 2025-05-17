# Compiler & Flags
CC = clang
CFLAGS = -Wall -Wextra -std=c99

# Directories
SRC_DIR = src
BIN_DIR = bin
TREASURE_HUB_SRC = $(SRC_DIR)/treasure_hub
TREASURE_MANAGER_SRC = $(SRC_DIR)/treasure_manager
UTILS_SRC = $(SRC_DIR)/directories_utils
LOGS_SRC = $(TREASURE_MANAGER_SRC)/write_logs

# Output binaries
TREASURE_HUB = $(BIN_DIR)/treasure_hub
TREASURE_MANAGER = $(BIN_DIR)/treasure_manager
MONITOR = $(BIN_DIR)/monitor

# Object files
UTILS_OBJS = $(BIN_DIR)/directories_utils.o
HUB_OBJS = $(BIN_DIR)/treasure_hub.o 
MANAGER_OBJS = $(BIN_DIR)/treasure_manager.o
MONITOR§_OBJS = $(BIN_DIR)/monitor.o
LOGS_OBJS = $(BIN_DIR)/write_logs.o

# Default build
all: $(TREASURE_HUB) $(TREASURE_MANAGER) $(MONITOR)

# Alias for build
build: all

# Compile source files
$(BIN_DIR)/directories_utils.o: $(UTILS_SRC)/directories_utils.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/treasure_hub.o: $(TREASURE_HUB_SRC)/treasure_hub.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/monitor.o: $(TREASURE_HUB_SRC)/monitor.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/treasure_manager.o: $(TREASURE_MANAGER_SRC)/treasure_manager.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/write_logs.o: $(LOGS_SRC)/write_logs.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link executables
$(TREASURE_HUB): $(HUB_OBJS) $(UTILS_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(TREASURE_MANAGER): $(MANAGER_OBJS) $(UTILS_OBJS) $(LOGS_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(MONITOR): $(BIN_DIR)/monitor.o
	$(CC) $(CFLAGS) -o $@ $^

# Run targets
run_monitor: $(MONITOR)
	@echo "Running monitor..."
	./$(MONITOR)
run_hub: $(TREASURE_HUB)
	@echo "Running treasure_hub..."
	./$(TREASURE_HUB)

run_manager: $(TREASURE_MANAGER)
	@echo "Running treasure_manager..."
	./$(TREASURE_MANAGER) $(ARGS)

# Clean
clean:
	rm -f $(BIN_DIR)/*.o $(TREASURE_HUB) $(TREASURE_MANAGER) $(MONITOR)

.PHONY: all build clean run_hub run_manager
