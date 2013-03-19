all: getEcowatt
getEcowatt: getEcowatt.c
	gcc -o getEcowatt getEcowatt.c `mysql_config --cflags --libs`
			
clean:
	rm getEcowatt

