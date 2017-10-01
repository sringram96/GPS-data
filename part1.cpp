#include <iostream>
#include <cassert>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <vector>
#include <cstdlib>
using namespace std;
//class definition
#ifndef __gpsdata_h_
#define __gpsdata_h_
class GPSData{
public:
  GPSData();
  void set_position(int ix, int iy);
  const int& getx();
  const int& gety();
  const float& getspeed();
  void change_x(int ix);
  void change_y(int iy);
  void changespeed(float ispeed);
private:
  int x;
  int y;
  float speed;
};
#endif
//class implementation
GPSData::GPSData(){
  x = 0;
  y = 0;
  speed = 0;
}
void GPSData::set_position(int ix, int iy){
  x = ix;
  y = iy;
}
const int& GPSData::getx(){
  return(x);
}
const int& GPSData::gety(){
  return(y);
}
const float& GPSData::getspeed(){
  return(speed);
}
void GPSData::changespeed(float ispeed){
  speed = ispeed;
}
void GPSData::change_x(int ix){
  x = ix;
}
void GPSData::change_y(int iy){
  y = iy;
}

// A few global constants
const int feet_per_mile = 5280;
const double seconds_between_samples = 4.25;

//non-member functions
double distance(GPSData* data, int n, int &avg_feet_per_minute){
  //initialize variables to keep track of sum
  double distance_sum = 0;
  double speed_sum = 0;
  //calculate speed/sample in for loop
  for(int i = 1; i < n; i++){
    double distance = sqrt(pow(data[i].getx() - data[i-1].getx(), 2) + pow(data[i].gety() - data[i-1].gety(),2));
    distance_sum += distance;
    int calculatedspeed = distance/(seconds_between_samples/60);
    speed_sum += calculatedspeed;
    data[i].changespeed(calculatedspeed);
  }
  //this part wasnt included in the original until i discovered a bug due to the fact that my code was rounding decimals > 0.5 down
  //this part calculates the decimal and decides whether it should be rounded up or down, then rounds appropriately
  int total = speed_sum/(n-1);
  float totalfloat = speed_sum/(n-1);
  float decimal = totalfloat - total;
  if(decimal > 0.5){
    avg_feet_per_minute = speed_sum/(n-1);
    avg_feet_per_minute += 1;
  }
  else{
    avg_feet_per_minute = speed_sum/(n-1);
  }
  //return overall distance
  return(distance_sum);
}
double filter(GPSData input[], GPSData output[], int n){
  //initialize variables, including buffer varaible for the avg feet/min variable
  int buffer = 0;
  //first and last nodes always stay the same
  output[0] = input[0];
  output[n-1] = input[n-1];
  //filter the points
  for(int i = 1; i < n-1; i++){
    output[i].change_x((input[i-1].getx() + input[i].getx() + input[i+1].getx())/3);
    output[i].change_y((input[i-1].gety() + input[i].gety() + input[i+1].gety())/3);
  }
  //return percent change
  double answer = (distance(input, n, buffer)-distance(output,n,buffer))/distance(input, n, buffer);
  return(answer*100);
}
void print(GPSData* input, int count){
  //format appropriately
  cout << setw(7) << right << setfill(' ') << "x-pos" << setw(8) << "y-pos" << setw(9) << "ft/min" << setw(10) << "min/mile" << endl;
  for(int i = 0; i < count; i++){
    float time = feet_per_mile / input[i].getspeed();
    int min = feet_per_mile / input[i].getspeed();
    int seconds = (time - min)*60;
    //if statements checks need for 'N/A' string
    if(input[i].getspeed() == 0){
      cout << setw(7) << setfill(' ') << right << input[i].getx() << setw(8) << right << input[i].gety() << setw(9) <<  (int)input[i].getspeed() << setw(10) << "N/A" << endl; 
    }
    else{
      cout << setw(7) << setfill(' ') << right << input[i].getx() << setw(8) << right << input[i].gety() << setw(9) <<  (int)input[i].getspeed() << setw(7) << setfill(' ') << min << ":" << setw(2) << setfill('0') << seconds << endl;
    }
  }
}
//appropriately prints the pace string
void print_pace(float speed){
  float time = feet_per_mile / speed;
  int min = feet_per_mile / speed;
  int seconds = (time - min)*60;
  cout << setw(2) << setfill(' ') << min << ":" << setw(2) << setfill('0') << seconds;
}

// ==========================================================================================

void recursive_filter(GPSData* input, GPSData* output, int n, double percent_change_threshold) {
  // make a temporary array to store the intermediate filtering result
  GPSData tmp[n];
  // run the filter once
  double percent_change = filter(input,tmp,n);
  cout << "RECURSE " << std::setw(5) << setfill(' ') << fixed << std::setprecision(2)
            << percent_change << "% change" << std::endl;
  if (fabs(percent_change) < percent_change_threshold) {
    // if the percent change is under the target threshold, we're done
    for (int i = 0; i < n; i++) {
      // copy the data from the temporary arraya into the final answer
      output[i] = tmp[i];
    }
  } else {
    // otherwise, print the intermediate result (for debugging)
    print(tmp,n);
    // and repeat!
    recursive_filter(tmp,output,n,percent_change_threshold);
  }
}

// ==========================================================================================

int main(int argc, char** argv) {
  // The mandatory first argument is the GPS data input file.
  if (argc < 2) {
    std::cerr << "ERROR!  This program must be called with at least one argument, the GPS data input file" << std::endl;
  }
  std::ifstream istr(argv[1]);
  assert (istr.good());


  // Load the GPS data file contents into a vector.
  // NOTE: this is the only place we use a vector in Part 1 of the homework.
  // This is just for convenience &/ flexibility for testing.
  std::vector<int> file_contents;
  int x;
  while (istr >> x) { file_contents.push_back(x); }


  // The optional second argument is the number of data points to work with.
  int count = -1;
  if (argc > 2) {
    count = atoi(argv[2]);
  }
  // If the argument is not provided, or if it is larger than the size
  // of the file, just use the whole file.
  if (count < 0 || int(file_contents.size()) / 2 < count) count = file_contents.size() / 2;

  
  // The optional third argument is the distance percentage change stopping criterion
  double percent_change_threshold = 0.5;
  if (argc > 3) {
    percent_change_threshold = atof(argv[3]);
  }  
  assert (percent_change_threshold > 0.0);

  
  // Prepare an array of the original data (the selected amount of data).
  GPSData input[count];
  for (int i = 0; i < count; i++) {
    input[i].set_position(file_contents[i*2],file_contents[i*2+1]);
  }
  std::cout << "-----------------------------------" << std::endl;
  int original_avg_speed;
  double original_distance = distance(input,count,original_avg_speed) / double (feet_per_mile);
  std::cout << "ORIGINAL" << std::endl;
  print(input,count);


  // Prepare arrays for the filter data.
  GPSData filtered[count];
  GPSData recursive_filtered[count];

  
  // Perform a single pass of filtering
  std::cout << "-----------------------------------" << std::endl;
  filter(input,filtered,count);
  int filtered_avg_speed;
  double filtered_distance = distance(filtered,count,filtered_avg_speed) / double (feet_per_mile);
  std::cout << "FILTERED" << std::endl;
  print(filtered,count);

  
  // Perform multiple passes of filtering (until the distance changes by less than the target percentage).
  std::cout << "-----------------------------------" << std::endl;
  recursive_filter(input,recursive_filtered,count,percent_change_threshold);
  int recursive_filtered_avg_speed;
  double recursive_filtered_distance = distance(recursive_filtered,count,recursive_filtered_avg_speed) / double (feet_per_mile);
  std::cout << "RECURSIVE FILTERED" << std::endl;
  print(recursive_filtered,count);


  // Output
  std::cout << "-----------------------------------" << std::endl;
  std::cout << "ORIGINAL:           "
            << std::fixed << std::setprecision(2) << std::setw(5) << setfill(' ') << original_distance
            << " miles, " << std::setw(4) << original_avg_speed << " feet per minute, ";
  print_pace(original_avg_speed);
  std::cout << " minutes per mile" << std::endl;
  std::cout << "FILTERED:           "
            << std::fixed << std::setprecision(2) << std::setw(5) << setfill(' ') << filtered_distance
            << " miles, " << std::setw(4) << filtered_avg_speed   << " feet per minute, ";
  print_pace(filtered_avg_speed);
  std::cout << " minutes per mile" << std::endl;  
  std::cout << "RECURSIVE FILTERED: "
            << std::fixed << std::setprecision(2) << std::setw(5) << setfill(' ') << recursive_filtered_distance
            << " miles, " << std::setw(4) << recursive_filtered_avg_speed << " feet per minute, ";
  print_pace(recursive_filtered_avg_speed);
  std::cout << " minutes per mile" << std::endl;
}

