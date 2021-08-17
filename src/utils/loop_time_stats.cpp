#include <cmath>
#include "utils/loop_time_stats.h"

loop_time_stats::loop_time_stats(string myfile, output_mode m)
{
  mode = m;
  filename_loop_time_stats = myfile;
  switch(mode) {
    case output_mode::fileout_and_screenout:
      flag_file_out = true;
      flag_screen_out = true;
      break;
    case output_mode::fileout_only:
      flag_file_out = true;
      flag_screen_out = false;
      break;
    case output_mode::screenout_only:
      flag_file_out = false;
      flag_screen_out = true;
      break;
    default:
      flag_file_out = false;
      flag_screen_out = false;
  }

  if ( flag_file_out == true ) ofs.open(myfile);
  for (int i=0;i<MAX_LOOP_TIME_FOR_LOOP_TIME_STATS_US;i++) t_loop_time_bins[i] = 0;
  clock_gettime ( CLOCK_MONOTONIC, &t_now );

}

loop_time_stats::~loop_time_stats()
{
  if (flag_file_out == true) ofs.close();
}

void loop_time_stats::loop_starting_point()
{

  clock_gettime ( CLOCK_MONOTONIC, &t_now );
  t_diff = t_now;
  TIMESPEC_DECREMENT( t_diff, t_prev );
  t_prev = t_now;
  if (cnt++ < NUMBER_OF_FIRST_CYCLES_TO_BE_SKIPPED_FOR_STATS) return; // for the first cycle, do nothing.
  t_loop_time_us = t_diff.tv_nsec / 1000;
  if ( t_loop_time_us < 0) t_loop_time_bins[0]++;
  else if ( t_loop_time_us > MAX_LOOP_TIME_FOR_LOOP_TIME_STATS_US-1 ) t_loop_time_bins[MAX_LOOP_TIME_FOR_LOOP_TIME_STATS_US-1]++;
  else t_loop_time_bins[t_loop_time_us]++;
}

void loop_time_stats::store_loop_time_stats()
{
  if (flag_screen_out == true){
    int max_index = 0;
    int max_count = 0;
    int init_index = 0;
    int end_index = 0;
    for (int i=0;i<MAX_LOOP_TIME_FOR_LOOP_TIME_STATS_US;i++) {
      if ( max_count < t_loop_time_bins[i] ) {
        max_index = i;
        max_count = t_loop_time_bins[i];
      }
    }
    for (int i=0;i<MAX_LOOP_TIME_FOR_LOOP_TIME_STATS_US;i++) {
      if (t_loop_time_bins[i] > 0) {
        init_index = i;
        break;
      }
    }
    for (int i=MAX_LOOP_TIME_FOR_LOOP_TIME_STATS_US-2;i>0;i--) {
      if (t_loop_time_bins[i] > 0) {
        end_index = i;
        break;
      }
    }

    cout << "\n\n########################################################" ;
    cout << "\n\nStatistics of loop time for "<< filename_loop_time_stats << ":\n\n" ;
    cout << "########################################################\n\n" ;
    cout << "The shortest loop time:\t" << init_index << " us"<<endl;
    cout << "The longest loop time:\t " << end_index << " us"<< endl;
//        cout << "\n\nCount of the loop time:\n" ;
//        for (int i=0;i<MAX_LOOP_TIME_US;i++) {
//            cout << t_loop_time_bins[i] << " ";
//        }
    cout << endl << endl << "Log10 histogram of the loop time within the range of the shortest and longest loop times: \n";
    for (int i=init_index;i< end_index ;i+=SIZE_OF_BINS_FOR_TERMINAL_DISPLAY_US) {

      int accumulated_count = 0;
      for (int j=i;j<i+SIZE_OF_BINS_FOR_TERMINAL_DISPLAY_US;j++) accumulated_count += t_loop_time_bins[j];
      cout << i << ":\t" << accumulated_count <<"\t";

      for (int j=0;j<std::log10(accumulated_count);j++) {
        cout << "#" ;
      }
      cout << endl;
    }
    cout << endl;
  }
  if (flag_file_out == true){
    for (int i=0;i<MAX_LOOP_TIME_FOR_LOOP_TIME_STATS_US;i++) {
      ofs << t_loop_time_bins[i] << endl;
    }
  }
}
