CC = arm-linux-gnueabihf-gcc
TARGET = intent_ayah.elf

$(TARGET): intent_ayah.c
	$(CC) -Wall intent_ayah.c cJSON.c -L. -lgxdnn -lm -o $@

clean:
	-rm $(TARGET)
