#include "mbed.h"
#include <cmath>
#include "DA7212.h"

#define bufferLength (32)
#define signalLength (192)

DA7212 audio;
Serial pc(USBTX, USBRX);

InterruptIn sw2(SW2);
InterruptIn sw3(SW3);

EventQueue queue3(32 * EVENTS_EVENT_SIZE);
Thread thread_song;
int idC = 0;
int song_num = 0;
int switchCase_num = 0;
int song1[42], song2[32], song3[24], note1[42], note2[32], note3[24];
int l = 0;

float signal[signalLength];
int16_t waveform[kAudioTxBufferSize];
char serialInBuffer[bufferLength];
int serialCount = 0;

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);

void songList() {
  for (l = 0; l < 42; l++) {
    song1[l] = (int)signal[l] * 1000.0;
  }
  for (l = 42; l < 84; l++) {
    note1[l - 42] = (int)signal[l] * 1000.0;
  }
  for (l = 84; l < 116; l++) {
    song2[l - 84] = (int)signal[l] * 1000.0;
  }
  for (l = 116; l < 148; l++) {
    note2[l - 116] = (int)signal[l] * 1000.0;
  }
  for (l = 148; l < 172; l++) {
    song3[l - 148] = (int)signal[l] * 1000.0;
  }
  for (l = 172; l < 196; l++) {
    note3[l - 172] = (int)signal[l] * 1000.0;
  }
}

void loadSignal(void)
{
  led2 = 0;
  int i = 0;
  serialCount = 0;
  audio.spk.pause();
  while (i < signalLength)
  {
    if (pc.readable())
    {
      serialInBuffer[serialCount] = pc.getc();
      serialCount++;
      if (serialCount == 5)
      {
        serialInBuffer[serialCount] = '\0';
        signal[i] = (float) atof(serialInBuffer);
        serialCount = 0;
        i++;
      }
    }
  }
  led2 = 1;
  wait_us(1000);
  led2 = 0;
  queue3.call(songList);
  led2 = 1;
  switchCase_num = switchCase_num + 1;
}

void playNote(int freq)
{
  for (int a = 0; a < kAudioTxBufferSize; a++)
  {
    waveform[a] = (int16_t) (sin((double)a * 2. * M_PI / (double) (kAudioSampleFrequency / freq)) * ((1 << 16) - 1));
  }
  audio.spk.play(waveform, kAudioTxBufferSize);
}

void playSong(void) {
  
      int klength1 = sizeof(song1) / sizeof(song1[0]);
      int k = 0;
      while(k < klength1)
      {
        led3 = 0;
        int length1 = note1[k];
        while (length1--)
        {
          // the loop below will play the note for the duration of 1s
          for (int j = 0; j < kAudioSampleFrequency / kAudioTxBufferSize; ++j)
          {
            
            queue3.call(playNote, song1[k]);
          }
          if (length1 < 1) wait_us(1000);
        }
        led3 = 1;
        k++;
      }
}

void stopPlay(void) {
  led1 = 0;
  queue3.cancel(idC);
  led1 = 1;
}

void switchCase(void) {
  switch (switchCase_num)
  {
    case 0:
      queue3.call(loadSignal);
      break;
    case 1:
      queue3.call(playSong);
      break;
  }
}

int main(void)
{
  led1 = 1;
  led2 = 1;
  led3 = 1;
  thread_song.start(callback(&queue3, &EventQueue::dispatch_forever));
  sw2.rise(queue3.event(switchCase));
  sw3.rise(queue3.event(stopPlay));
}