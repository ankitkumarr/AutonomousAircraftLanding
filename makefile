all: flight flight_mutex

flight: flight.c
	gcc -g -o flight flight.c -pthread

flight_mutex: flight_mutex.c
	gcc -g -o flight_mutex flight_mutex.c -pthread

clean:
	rm -rf flight flight_mutex
