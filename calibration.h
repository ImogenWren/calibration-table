

#pragma once

#ifndef calibration_h
#define calibration_h

#include <debugging.h>

#define DEBUG_LEVEL ERROR

// This calibration table should be filled in with the voltage offset measured at the sensing pin of the microcontroller.
// This list is arbritary length, limited only by microcontroller memory.
// To avoid dynamic memory allocation, array size is defined in this header, but can be expanded to allow for longer arrays,
// Or limit removed entirely to allow dynamic memory allocation


// Previous implmentation was done with std::map, howver as this is not available across all arduino implementions, this version will attempt to do the same with simple arrays
// I think the best option is to use an 1-dimensional array of 2 dimensional arrays or data structures.

#define MAX_ENTRIES 11

// Create a structure to hold key-value pairs
struct cal_data {
  float key;
  float value;
};

// Create an array of key value pairs to fill with calibration data

// Enter the key, the raw value measured & calculated for V_at_pin, then enter the offset from true value measured by external measurement device
static cal_data cal_map[MAX_ENTRIES] = {
  { 0.00, 0.00 },
  { 0.50, -0.022 },
  { 1.00, -0.035 },
  { 1.50, -0.063 },
  { 2.00, -0.100 },
  { 2.50, -0.124 },
  { 3.00, -0.143 },
  { 3.50, -0.180 },
  { 4.00, -0.213 },
  { 4.50, -0.250 },
  { 5.00, -0.270 }
};


static void print_cal_table(const cal_data *_cal_map, int16_t size = MAX_ENTRIES) {
  for (int i = 0; i < size; i++) {
    Serial.print("key: ");
    Serial.print(_cal_map[i].key);
    Serial.print(" val: ");
    Serial.println(_cal_map[i].value);
  }
}

/*
 Float comparisons can be imprecise, so consider tolerances (e.g., fabsf(a - b) < EPSILON) if needed.
 - This returns the absolute value of the difference (i,e, always positive) to allow comparason to a tolerance figure EPSILON
 (fabsf(cal_map[i].key - inputVal) < EPSILON )
*/

// Define the tolerance of float comparasons
#define EPSILON 0.1

#define LOWER_KEY_ENTERED 0b00000001
#define UPPER_KEY_ENTERED 0b00000010
#define BOTH_KEYS_ENTERED 0b00000011

static float do_table_calibration(float inputVal, const cal_data *_cal_map) {
#if DEBUG_LEVEL >= INFO
  Serial.print("inputVal: ");
  Serial.println(inputVal, 3);
#endif
  float lowerKey;
  float upperKey;
  float lowerVal = 0;
  float upperVal = 0;
  float lastLowerKey;
  float lastUpperKey;
  float lastLowerVal = 0;
  float lastUpperVal = 0;
  uint8_t matchState = 0b00000000;  // Used to track what has been matched,
                                    // can use bitmasks to set and check individual bits,
                                    //00 = no match, starting sort  01 = lower value found, 10 = upper value found 11 = both values found 100 = exact match (concept, actual implemention might differ)
                                    //1000 = sort started, -> its important to differentiate because the floats are undefined at the start so useless to compare against themselves
                                    // This method tracks 4 independent bool states (4 bytes) with 1 8bit variable (1 byte)

  // Search Algorithm
  for (int i = 0; i < MAX_ENTRIES; ++i) {  //try to find "exact" match - doing some float magic
    if (fabsf(_cal_map[i].key - inputVal) < EPSILON) {
      float returnVal = inputVal + _cal_map[i].value;
#if DEBUG_LEVEL >= INFO
      Serial.print("key-found: ");
      Serial.print(_cal_map[i].key, 3);
      Serial.print(" value: ");
      Serial.print(_cal_map[i].value, 3);
      Serial.print(" return-val: ");
      Serial.println(returnVal, 3);
#endif
      return returnVal;
    } else {                                                        // "exact match" was not found so finding upper and lower values to interpolate
      if ((matchState & LOWER_KEY_ENTERED) != LOWER_KEY_ENTERED) {  //if any lower key has not been entered
        if (_cal_map[i].key < inputVal) {                           // get the first key that is lower than input value
          lowerKey = _cal_map[i].key;
          lowerVal = _cal_map[i].value;
          lastLowerKey = lowerKey;
          lastLowerVal = lowerVal;

#if DEBUG_LEVEL >= INFO
          Serial.print("initial low key found: ");
          Serial.print(lowerKey, 3);
          Serial.print(" lowerVal: ");
          Serial.println(lowerVal, 3);
#endif
          matchState |= LOWER_KEY_ENTERED;  // flag that some data has been captured (though might not be correct yet!)
        }
      } else {  // lower key has been entered, so now need to only update if it is HIGHER than existing key, but less than inputVal
        if (_cal_map[i].key > lowerKey && _cal_map[i].key < inputVal) {
          lastLowerKey = lowerKey;
          lowerKey = _cal_map[i].key;
          lastLowerVal = lowerVal;  // used later if input is offscale
          lowerVal = _cal_map[i].value;
#if DEBUG_LEVEL >= INFO
          Serial.print("low key found: ");
          Serial.print(lowerKey, 3);
          Serial.print(" lowerVal: ");
          Serial.println(lowerVal, 3);
#endif
        }
      }
      if ((matchState & UPPER_KEY_ENTERED) != UPPER_KEY_ENTERED) {  //if any upper key has not been entered
        if (_cal_map[i].key > inputVal) {
          upperKey = _cal_map[i].key;  // get the first key that is higher than input value
          upperVal = _cal_map[i].value;
          lastUpperKey = upperKey;
          lastUpperVal = upperVal;
#if DEBUG_LEVEL >= INFO
          Serial.print("initial upper key found: ");
          Serial.print(upperKey, 3);
          Serial.print(" upperVal: ");
          Serial.println(upperVal, 3);
#endif
          matchState |= UPPER_KEY_ENTERED;
        }
      } else {  // some upper key has been found, so only update if new key is LESS than existing upper key, but higher than inputVal
        if (_cal_map[i].key < upperKey && _cal_map[i].key > inputVal) {
          lastUpperKey = upperKey;
          upperKey = _cal_map[i].key;
          lastUpperVal = upperVal;
          upperVal = _cal_map[i].value;
#if DEBUG_LEVEL >= INFO
          Serial.print("upper key found: ");
          Serial.print(upperKey, 3);
          Serial.print(" upperVal: ");
          Serial.println(upperVal, 3);
#endif
        }
      }
    }
  }  // end of for loop, if function has not returned already, then we must now have an upper and lower key and upper and lower value to do some interpolation with, unless floats are still empty
     // first check flags again, as an inputVal that is out of bounds of the list should not have been picked up
  if ((matchState & BOTH_KEYS_ENTERED) != BOTH_KEYS_ENTERED) {
#if DEBUG_LEVEL >= WARNING
    Serial.println("inputVal out of range of cal table");
    Serial.print("matchState: ");
    Serial.println(matchState, BIN);
#endif
    if ((matchState & LOWER_KEY_ENTERED) == LOWER_KEY_ENTERED) {  // Only lower value has been found, therefore inputVal was OVERSCALE, interpolate using highest two values to estimate slope (ALMOST WORKS, need to get the difference)
#if DEBUG_LEVEL >= WARNING
      Serial.println("Input OVERSCALE, interpolating using uppermost values to estimate slope");  // and use that to estimate a new high point, the current high point becomes the low point last low point becomes low point
      Serial.print("lowerKey: ");
      Serial.print(lowerKey);
      Serial.print(" lastLowerKey: ");
      Serial.print(lastLowerKey);
      Serial.print("lowerVal: ");
      Serial.print(lowerVal);
      Serial.print(" lastLowerVal: ");
      Serial.println(lastLowerVal);
#endif
      upperKey = lowerKey;
      lowerKey = lastLowerKey;
      upperVal = lowerVal;
      lowerVal = lastLowerVal;
      matchState |= UPPER_KEY_ENTERED;
    } else if ((matchState & UPPER_KEY_ENTERED) == UPPER_KEY_ENTERED) {  // only upper key was found, therefore inputVal was UNDERSCALE, interpolate using lowest two values to estimate slope // THIS ASSUMES SORTED LIST!
#if DEBUG_LEVEL >= WARNING
      Serial.println("Input UNDERSCALE, interpolating using lowermost values to estimate slope");
#endif
      upperKey = _cal_map[1].key;
      lowerKey = _cal_map[0].key;
      upperVal = _cal_map[1].value;
      lowerVal = _cal_map[0].value;
      matchState |= LOWER_KEY_ENTERED;
    } else {
#if DEBUG_LEVEL >= ERROR
      Serial.print("No keys were matched, unknown error, this shouldnt even be possible");
#endif
    }
  }
  // this IF has been changed from an else, now checks again if both keys have been found as the section above to estimate the slope may (should) have found them and set the flags
  if ((matchState & BOTH_KEYS_ENTERED) == BOTH_KEYS_ENTERED) {
    float returnVal = lowerVal + (upperVal - lowerVal) * float(inputVal - lowerKey) / fabsf(upperKey - lowerKey);
#if DEBUG_LEVEL >= INFO
    Serial.println("Both Keys found, Interpolating...");
    Serial.print("interpolated offset calculated: ");
    Serial.println(returnVal, 3);
#endif
    returnVal += inputVal;
#if DEBUG_LEVEL >= INFO
    Serial.print("corrected inputVal: ");
    Serial.println(returnVal, 3);
#endif
    return returnVal;
  }
#if DEBUG_LEVEL >= ERROR
  Serial.println("Something Went Wrong, returning inputVal");
#endif
  return inputVal;  // not found, just return input value
}






















#endif