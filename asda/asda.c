#include <wiringPi.h>
#include <stdio.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include "usb_hid_keys.h"

struct Report{
  char modifiers;
  char reserved;
  char keys[6];
};
const struct Report emptyReport = {.modifiers=0, .reserved=0, .keys={0}};

enum Buttons {VioletBtn=15, GreenBtn=16};

volatile sig_atomic_t sigint_received = 0;

void sig_handler(int signo){
  if(signo == SIGINT)
    sigint_received = 1;
}

int reportComp(const struct Report* const currentReport, const struct Report* const prevReport){
  return memcmp(currentReport, prevReport, sizeof(struct Report));
}

bool sendReport(const struct Report* const report, FILE* file){
  unsigned char* charPtr = (unsigned char*)report;
  for(int i = 0; i<sizeof(struct Report); ++i)
    printf("%02x ",charPtr[i]);
  printf("\n");

  size_t bytes = fwrite(report, sizeof(char), sizeof(struct Report), file);
  fflush(file);
  printf("Bytes written %d\n", bytes);
  return bytes;
}

void receiveReport(FILE* file){
  unsigned char buf;
  size_t bytes = fread(&buf, 1, 1, file);
  if(bytes)
    printf("Resp: %02x\n", buf);
  else
    printf("Resp: none\n");
}

int main(){

  signal(SIGINT, sig_handler);

  // printf("Sizeof report: %d\n", sizeof(struct Report));

  wiringPiSetup();

  pinMode(VioletBtn, INPUT);
  pullUpDnControl(VioletBtn, PUD_UP);

  pinMode(GreenBtn, INPUT);
  pullUpDnControl(GreenBtn, PUD_UP);

  //wiringPiISR(VioletBtn, INT_EDGE_FALLING, &onVioletBtnPressed);
  //wiringPiISR(GreenBtn, INT_EDGE_FALLING, &onGreenBtnPressed);

  FILE *file = fopen("/dev/hidg0","wb+");

  struct Report prevReport = emptyReport;

  while(true){
    delay(10);

    struct Report report = {.modifiers=0x0, .reserved=0x0, .keys={0,0,0,0,0,0}};
    if(digitalRead(VioletBtn) == 0)
      report.keys[0] = KEY_A;
    if(digitalRead(GreenBtn) == 0)
      report.keys[1] = KEY_A;

    if(reportComp(&report, &prevReport) != 0){
       if(sendReport(&report, file))
         ; //receiveReport(file);
    }

    prevReport = report;

    if(sigint_received == 1)
      break;
  }

  sendReport(&emptyReport, file);

  if(file)
    fclose(file);
  printf("\nCya\n");
  return 0;
}
