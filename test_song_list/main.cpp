#include "mbed.h"
#include <cmath>
#include "DA7212.h"

#define bufferLength (32)
#define signalLength (1024)

DA7212 audio;
Serial pc(USBTX, USBRX);

InterruptIn sw2(SW2);


EventQueue queue4(32 * EVENTS_EVENT_SIZE);
Thread thread_song;
int idC = 0;
int song_num = 0;
int switchCase_num = 0;

float signal[signalLength];
int16_t waveform[kAudioTxBufferSize];
char serialInBuffer[bufferLength];
int serialCount = 0;

DigitalOut green_led(LED2);

void loadSignal(void)
{
  green_led = 0;
  int i = 0;
  serialCount = 0;
  audio.spk.pause();
  while(i < signalLength)
  {
    if(pc.readable())
    {
      serialInBuffer[serialCount] = pc.getc();
      serialCount++;
      if(serialCount == 5)
      {
        serialInBuffer[serialCount] = '\0';
        signal[i] = (float) atof(serialInBuffer);
        serialCount = 0;
        i++;
      }
    }
  }
  green_led = 1;
  song_list();
}

void song_list(){
    int song1[42], song2[42], song3[42];
    int j;
    for (j=0;j<42;j++){
        song1[j] = (int)song1[j];
    }
    for (j=42;j<84;j++){
        song2[j-42] = (int)song2[j];
    }
    for (j=84;j<126;j++){
        song3[j-84] = (int)song3[j];
    }
    songlist[0].loadinfo(song1);
    songlist[1].loadinfo(song2);
    songlist[2].loadinfo(song3);
}


void playNote(int freq)
{
  for (int i = 0; i < kAudioTxBufferSize; i++)
  {
    waveform[i] = (int16_t) (signal[(uint16_t) (i * freq * signalLength * 1. / kAudioSampleFrequency) % signalLength] * ((1<<16) - 1));
  }
  // the loop below will play the note for the duration of 1s
  for(int j = 0; j < kAudioSampleFrequency / kAudioTxBufferSize; ++j)
  {
    audio.spk.play(waveform, kAudioTxBufferSize);
  }
}

void loadSignalHandler(void) {queue4.call(loadSignal);}

void playSong(void) {int k; queue4.call(playNote,songlist[song_num][k]);}

void stopPlay(void) {queue.cancel(idC);}

void switchCase(void) {
    switch (switchCase_num)
    {
    case 0:
        queue4.event(loadSignalHandler);
        switchCase_num = switchCase_num + 1;
        break;
    case 1:
        switchCase_num = switchCase_num + 1;
        queue4.event(playSong);
        break;
    case 2:
        switchCase_num = switchCase_num - 1;
        queue4.event(stopPlay);
        break;
    }
}

int main(void)
{
  green_led = 1;
  thread_song.start(callback(&queue4, &EventQueue::dispatch_forever));
  sw2.rise(queue4.event(switchCase));
}