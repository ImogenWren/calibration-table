# calibration-table
Library & example for doing multi point calibrations using a table & interpolation


## Example data output
- showing input to output with extrapolation above and below table defined values for the following table
```
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

```
<img width="1910" height="1113" alt="image" src="https://github.com/user-attachments/assets/0e13082a-657d-4afe-a5aa-9b9b6f0928a6" />


_note: you might notice that the IDE V2.x serial plotter is showing more than the 50 samples it usually only allows, if you would also like a useful serial plotter like IDE V1 -> follow [this](https://www.youtube.com/watch?v=qrfpPuw2W3A) guide. Seriously Arduino when are you gonna fix this shite?_
