#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <mraa/aio.h>
#include <unistd.h>

const int ADC_PIN = 0;
const float ADC_MAX = 1024.0;
const int DELAY_SEC = 1;

//Calculate analog temperature readings to digital ones. From: http://wiki.seeed.cc/Grove-Temperature_Sensor_V1.2/
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

int main(int argc, char **argv){
	mraa_aio_context adcPin;   /* ADC pin context */
    float adcValue;  /* Read ADC value */

    mraa_init();
    adcPin = mraa_aio_init(ADC_PIN);

	// check if adc is NULL  
	while (1) {  
		adcValue = mraa_aio_read(adcPin);
		float temperature = convertReading(adcValue);
		fprintf(stdout, "Temperature Value = %f\n", temperature); 
		sleep(DELAY_SEC);
	} 
}
