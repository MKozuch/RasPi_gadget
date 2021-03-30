#include <wiringPi.h>
#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include "usb_hid_keys.h"

struct Report{
  char modifiers;
  char reserved;
  char keys[6];
};
struct Report emptyReport = {.modifiers=0, .reserved=0, .keys={0}};

enum Buttons {VioletBtn=15, GreenBtn=16};

void onVioletBtnPressed(){
  struct Report report = {.modifiers=0x0, .reserved=0x0, .keys={KEY_1, 0,0,0,0,0}};
  // printf(report);
}

void onGreenBtnPressed(){
   printf("B\n");
}

void onVioletBtnReleased(){
}

void onGreenBtnReleased(){
}

volatile sig_atomic_t sigint_received = 0;

void sig_handler(int signo){
  if(signo == SIGINT)
    sigint_received = 1;
}


int main(){

  signal(SIGINT, sig_handler);

  wiringPiSetup();
  pinMode(VioletBtn, INPUT);
  pullUpDnControl(VioletBtn, PUD_UP);

  pinMode(GreenBtn, INPUT);
  pullUpDnControl(GreenBtn, PUD_UP);

  //wiringPiISR(VioletBtn, INT_EDGE_FALLING, &onVioletBtnPressed);
  //wiringPiISR(GreenBtn, INT_EDGE_FALLING, &onGreenBtnPressed);

  FILE *file = fopen("/dev/hidg0","wb");
  bool previousEmpty = 1;

  while(true){
    delay(1);

    struct Report report = {.modifiers=0x0, .reserved=0x0, .keys={0,0,0,0,0,0}};
    if(digitalRead(VioletBtn) == 0)
      report.keys[0] = KEY_0;
    if(digitalRead(GreenBtn) == 0)
      report.keys[1] = KEY_1;

    fwrite(&report, sizeof(char), sizeof(report), file);

    if(sigint_received == 1)
      break;
  }

  fwrite(&emptyReport, sizeof(char), sizeof(emptyReport), file);
  fclose(file);
  printf("Cya\n");
  return 0;
}
