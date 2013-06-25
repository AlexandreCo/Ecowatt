all: getEcowatt emoncms3
getEcowatt: getEcowatt.c
	gcc -o getEcowatt getEcowatt.c `mysql_config --cflags --libs`

emoncms3: emoncms3.c
	gcc -o emoncms3 emoncms3.c `mysql_config --cflags --libs`
	
clean:
	rm getEcowatt

