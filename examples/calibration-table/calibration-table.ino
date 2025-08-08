/* Calibration-Table

    Example of how to use the calibration.h calibration table library

    
*/

/*
Sketch uses 4540 bytes (14%) of program storage space. Maximum is 30720 bytes.
Global variables use 319 bytes (15%) of dynamic memory, leaving 1729 bytes for local variables. Maximum is 2048 bytes.

Compare this to a cut down & modified version of sensorObject that uses STD::<map> variables, doesnt work as well and uses
Sketch uses 8518 bytes (27%) of program storage space. Maximum is 30720 bytes.
Global variables use 516 bytes (25%) of dynamic memory, leaving 1532 bytes for local variables. Maximum is 2048 bytes.
WAY MORE MEMORY!

I think this libary is winner winner chicken dinner
*/


#include "calibration.h"

// Create Calibration object
calibration cal;


// This calibration table should be filled in with the voltage offset measured at the sensing pin of the microcontroller.
// This list is arbritary length, limited only by microcontroller memory.
// To avoid dynamic memory allocation, array size is defined in this header, but can be expanded to allow for longer arrays,
// Or limit removed entirely to allow dynamic memory allocation> -> maybe at your own risk but you still need to pass the array size

// Create an array of key value pairs to fill with calibration data
// Enter the key, the raw value measured & calculated for V_at_pin, then enter the offset from true value measured by external measurement device
// NOTE: this example is not accurate, but is deliberatly all over the place to show correct behaviour of interpolation and further extrapolation

calibration::cal_data cal_map[MAX_ENTRIES] = {
  { 0.00, 0.00 },
  { 0.50, -0.022 },
  { 1.00, -0.035 },
  { 1.50, -2.063 },
  { 2.00, -1.100 },
  { 2.50, -4.124 },
  { 3.00, -3.143 },
  { 3.50, -1.180 },
  { 4.00, -2.213 },
  { 4.50, -3.250 },
  { 5.00, -3.270 }
};


/*  Can be used to test sorting function
calibration::cal_data cal_map[MAX_ENTRIES] = {
  { 0, 0.00 },
  { 1, 1 },
  { 3, 3 },
  { -2, 2 },
  { 4, 4 },
  { -5, 5 },
  { 7, 7 },
  { 8, -8 },
  { 6, 6 },
  { 9, 9 },
  { 10, 10 }
};
*/



void setup() {
  Serial.begin(115200);
  Serial.println("input, output");
  cal.print_cal_table(cal_map, MAX_ENTRIES);

  cal.sort_cal_table(&cal_map[0]);  // cal_map must be passed by pointer to the first element

  cal.print_cal_table(cal_map, MAX_ENTRIES);

  float input = -1.0;

  uint32_t startTime = millis();

  while (true) {
    ;
  }

  for (int i = 0; i < 500; i++) {
    float output = cal.do_table_calibration(input, cal_map);
    Serial.print(input);
    Serial.print(", ");
    Serial.println(output);
    input += 0.03;
  }
  uint32_t finishTime = millis();
  Serial.print("time-taken: ");
  Serial.println(finishTime - startTime);
}

void loop() {
  // put your main code here, to run repeatedly:
}
