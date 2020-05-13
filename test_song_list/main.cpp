#include "mbed.h"
#include <cmath>
#include "DA7212.h"

#define bufferLength (32)
#define signalLength (1024)

DA7212 audio;
Serial pc(USBTX, USBRX);

InterruptIn sw2(SW2);
InterruptIn sw3(SW3);

EventQueue queue3(32 * EVENTS_EVENT_SIZE);
Thread thread_song;
int idC = 0;
int song_num = 0;
int switchCase_num = 0;

float signal[signalLength];
int16_t waveform[kAudioTxBufferSize];
char serialInBuffer[bufferLength];
int serialCount = 0;

DigitalOut led2(LED2);

void loadSignal(void)
{
  led2 = 0;
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
  led2 = 1;
  song_list();
}

void song_list(){
    int song1[42], song2[42], song3[42], note1[42], note2[42], note3[42];
    int j;
    for (j=0;j<42;j++){
        song1[j] = (int)signal[j]*1000.0;
    }
    for (j=42;j<84;j++){
        note1[j-42] = (int)signal[j]*1000.0;
    }
    for (j=84;j<126;j++){
        song2[j-84] = (int)signal[j]*1000.0;
    }
    for (j=126;j<168;j++){
        note2[j-126] = (int)signal[j]*1000.0;
    }
    for (j=168;j<210;j++){
        song3[j-168] = (int)signal[j]*1000.0;
    }
    for (j=210;j<252;j++){
        note3[j-210] = (int)signal[j]*1000.0;
    }
    songlist[0].loadinfo(song1);
    songlist[1].loadinfo(note1);
    songlist[2].loadinfo(song2);
    songlist[3].loadinfo(note2);
    songlist[4].loadinfo(song3);
    songlist[5].loadinfo(note3);
}

void playNote(int freq)
{
  for(int a = 0; a < kAudioTxBufferSize; a++)
  {
    waveform[a] = (int16_t) (sin((double)a * 2. * M_PI/(double) (kAudioSampleFrequency / freq)) * ((1<<16) - 1));
  }
  audio.spk.play(waveform, kAudioTxBufferSize);
}

void loadSignalHandler(void) {queue3.call(loadSignal);}

void playSong(void) {
    int k;
    for(int k = 0; k < size(songlist[song_num]); k++)
       {
         int length = songlist[song_num+1][k];
         while(length--)
         {
           // the loop below will play the note for the duration of 1s
           for(int j = 0; j < kAudioSampleFrequency / kAudioTxBufferSize; ++j)
           {
            queue3.call(playNote, songlist[song_num+1][k]);
          }
          if(length < 1) wait_us(1000);
          int stoppoint = k;
}

void stopPlay(void) {queue3.cancel(idC);}

void switchCase(void) {
    switch (switchCase_num)
    {
    case 0:
        queue3.call(loadSignalHandler);
        switchCase_num = switchCase_num + 1;
        break;
    case 1:
        idC = queue3.call_every(1, playSong);
        break;
    }
}

int main(void)
{
  led = 1;
  thread_song.start(callback(&queue3, &EventQueue::dispatch_forever));
  sw2.rise(queue3.event(switchCase));
  sw3.rise(queue3.event(stopPlay));
}