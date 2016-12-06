#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <mraa/aio.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <string.h>

const int ADC_PIN = 0;
const float ADC_MAX = 1024.0;
const int DELAY_SEC = 3;

const char *TEMP_STRINGS[2] = {"C", "F"};

const char *SERVER_NAME = "lever.cs.ucla.edu";
const int SERVER_PORT = 16000;
const char *PORT_REQUEST_MESSAGE = "Port request 303652195";

//Convert analog temperature readings to digital ones. From: http://wiki.seeed.cc/Grove-Temperature_Sensor_V1.2/
//Make sure to have base shield set to 5V!
float convertReading(float adcValue) {
	//B value of the thermistor
	const int B = 4275; 
	// R0 = 100k   
	//const int R0 = 100000;    

    float R = 1023.0/((float)adcValue)-1.0;
    R = 100000.0 * R;

    //convert to temperature via datasheet
    float temperature=1.0/(log(R/100000.0)/B+1/298.15)-273.15;

    return temperature;
}

float celciusToFarenheit(float tempC){
	return (tempC * 1.8) + 32;
}

float farenheitToCelcius(float tempF){
	return (tempF - 32) / 1.8;
}

int connectToServer(){
	 //Connect to lever.cs.ucla.edu
    int sock_fd = -1;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		fprintf(stderr, "Error opening socket");
	}

	server = gethostbyname(SERVER_NAME);
	if (server == NULL){
		fprintf(stderr, "ERROR: no such host %s\n", SERVER_NAME);
		exit(1);
	}

	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(SERVER_PORT);

	fprintf(stdout, "Connecting to lever.cs.ucla.edu on port 16000...\n");
	if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		fprintf(stderr, "Error connecting to lever.cs.ucla on port 16000\n");
		exit(1);
	}
	fprintf(stdout, "Connected to lever.cs.ucla.edu on port 16000\n");

	fprintf(stdout, "Writing port request message to lever.cs.ucla.edu on port 16000\n");
	write(sock_fd, PORT_REQUEST_MESSAGE, strlen(PORT_REQUEST_MESSAGE));
	
	int new_port;
	read(sock_fd, &new_port, sizeof(int));
	fprintf(stdout, "Got new port: %d from lever.cs.ucla.edu\n", new_port);
	serv_addr.sin_port = htons(new_port);

	int newsock_fd = -1;
	if ((newsock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		fprintf(stderr, "Error opening new socket");
	}
	//done with old socket now
	close(sock_fd);

	fprintf(stdout, "Connecting to lever.cs.ucla.edu on port %d...\n", new_port);
	if (connect(newsock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
		fprintf(stderr, "Error connecting to lever.cs.ucla on port %d\n", new_port);
		exit(1);
	}
	fprintf(stdout, "Connected to lever.cs.ucla.edu on port %d, FIRE AWAY!\n", new_port);

	return newsock_fd;
}

int main(int argc, char **argv){
	mraa_aio_context adcPin;   /* ADC pin context */
    float adcValue;  /* Read ADC value */

    mraa_init();
    adcPin = mraa_aio_init(ADC_PIN);

    int runningFlag = 1;
    int farenheitFlag = 1;
    int frequency = DELAY_SEC;

    time_t seconds;
	struct tm *tm;

    FILE *log_file = fopen("log.txt", "a");
    if (log_file == NULL) fprintf(stderr, "Failed to create log_file\n");

   
    int server_socket = connectToServer();
    char reading_str[50];
	bzero(reading_str, 50);
	fd_set read_fds;
	char read_buf[32];
	bzero(read_buf, 32);
	int n_bytes;

	struct timeval timeout;
	timeout.tv_sec = 0;
	//100 milliseconds
	timeout.tv_usec = 100000;

	while (1) { 
		//Handle commands from server
		FD_ZERO(&read_fds);
		FD_SET(server_socket, &read_fds);
		select(server_socket + 1, &read_fds, NULL, NULL, &timeout);

		bzero(read_buf, 32);
		if (FD_ISSET(server_socket, &read_fds)){
			n_bytes = read(server_socket, read_buf, 31);
			fprintf(stdout, "Got update: %s from lever.cs.ucla.edu\n", read_buf);

			//If the update is to turn off
			if (strlen(read_buf) == strlen("OFF") && strcmp(read_buf,"OFF") == 0){
				fprintf(log_file, "OFF\n");
				fclose(log_file);
				close(server_socket);
				exit(0);
			}
			else if (strlen(read_buf) == strlen("STOP") && strcmp(read_buf,"STOP") == 0){
				fprintf(log_file, "STOP\n");
				fflush(log_file);
				runningFlag = 0;
			}
			else if (strlen(read_buf) == strlen("START") && strcmp(read_buf, "START") == 0){
				fprintf(log_file, "START\n");
				fflush(log_file);
				runningFlag = 1;
			}
			else if (strncmp(read_buf, "SCALE=", strlen("SCALE=")) == 0){
				char new_scale;
				sscanf(read_buf, "SCALE=%c", &new_scale);
				if (new_scale != 'F' && new_scale != 'C'){
					fprintf(log_file, "%s I\n", read_buf);
					fflush(log_file);
				}
				else {
					farenheitFlag = (new_scale == 'F') ? 1 : 0;
					fprintf(log_file, "SCALE=%s\n", TEMP_STRINGS[farenheitFlag]);
					fflush(log_file);
				}
			}
			else if (strncmp(read_buf, "FREQ=", strlen("FREQ=")) == 0){
				int new_frequency;
				int scanned_ints = sscanf(read_buf, "FREQ=%d", &new_frequency);
				if (scanned_ints != 1 || new_frequency < 1 || new_frequency > 3600){
					fprintf(log_file, "FREQ=%d I\n", new_frequency);
					fflush(log_file);
				}
				else {
					fprintf(log_file, "FREQ=%d \n", new_frequency);
					fflush(log_file);
					frequency = new_frequency;
				}
			}
			else {
				fprintf(log_file, "%s I\n", read_buf);
				fflush(log_file);
			}
		}

		if (runningFlag == 1) {
			adcValue = mraa_aio_read(adcPin);
			float temp = convertReading(adcValue);
			if (farenheitFlag == 1) temp = celciusToFarenheit(temp);

			
			seconds = time(&seconds);
			//Use utc time
		    if ((tm = gmtime(&seconds)) == NULL) {
		        fprintf(stderr, "Getting time\n");
		        return 1;
		    }

		    //Write reading update to socket
		    bzero(reading_str, 50);
			sprintf(reading_str, "303652195 TEMP=%0.1f", temp);
			write(server_socket, reading_str, strlen(reading_str));

			//Log reading locally
		    fprintf(stdout, "Timestamp %02d:%02d:%02d Temperature %0.1f F/C %s\n", tm->tm_hour, tm->tm_min, tm->tm_sec, temp, TEMP_STRINGS[farenheitFlag]);	
			fprintf(log_file, "Timestamp %02d:%02d:%02d Temperature %0.1f F/C %s\n", tm->tm_hour, tm->tm_min, tm->tm_sec, temp, TEMP_STRINGS[farenheitFlag]);
			fflush(log_file);
		}

		sleep(frequency);
	}

	fclose(log_file);
	exit(0);
}
