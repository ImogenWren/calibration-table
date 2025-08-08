
#include "calibration.h"


void setup() {
  Serial.begin(115200);
  Serial.println("input, output");
  //print_cal_table(cal_map);

  float input = -5.0;


  for (int i = 0; i < 150; i++) {
    float output = do_table_calibration(input, cal_map);
    Serial.print(input);
    Serial.print(", ");
    Serial.println(output);
    input += 0.1;
    delay(1);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}
