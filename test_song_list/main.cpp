#include "mbed.h"
#include <cmath>
#include "DA7212.h"

#define bufferLength (32)
#define musicLength (256)
#define kAudioTxBufferSize 1000
#define kAudioSampleFrequency 16000

DA7212 audio;
Serial pc(USBTX, USBRX);

InterruptIn sw2(SW2);
InterruptIn sw3(SW3);

EventQueue queue3(32 * EVENTS_EVENT_SIZE);
EventQueue queue5(32 * EVENTS_EVENT_SIZE);
Thread thread_song;
Thread thread_songStop;
int idC = 0;
int song_num = 0;
int switchCase_num = 0;
int song1[42], song2[32], song3[24], note1[42], note2[32], note3[24];
int l = 0;

float music[musicLength];
int16_t waveform[kAudioTxBufferSize];
char serialInBuffer[bufferLength];
int serialCount = 0;

DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);

void songList() {
  for (l = 0; l < 42; l++) {
    song1[l] = music[l];
  }
  for (l = 42; l < 84; l++) {
    note1[l - 42] = music[l]/100;
  }
  for (l = 84; l < 116; l++) {
    song2[l - 84] = music[l];
  }
  for (l = 116; l < 148; l++) {
    note2[l - 116] = music[l]/100;
  }
  for (l = 148; l < 172; l++) {
    song3[l - 148] = music[l];
  }
  for (l = 172; l < 196; l++) {
    note3[l - 172] = music[l]/100;
  }
}

void loadmusic(void)
{
  led2 = 0;
  int i = 0;
  serialCount = 0;
  audio.spk.pause();
  while (i < musicLength)
  {
    if (pc.readable())
    {
      serialInBuffer[serialCount] = pc.getc();
      serialCount++;
      if (serialCount == 3)
      {
        serialInBuffer[serialCount] = '\0';
        music[i] = (float)atof(serialInBuffer);
        serialCount = 0;
        i++;
      }
    }
    if (i == 196){
      i = 256;
    }
  }
  led2 = 1;
  wait_us(1000);
  led3 = 0;
  queue3.call(songList);
  led3 = 1;
  switchCase_num = switchCase_num + 1;
}

void playNote(int freq)
{
  for(int i = 0; i < 1000; i++)
  {
    waveform[i] = (int16_t) (sin((double)i * 2. * M_PI/(double) (kAudioSampleFrequency / freq)) * ((1<<16) - 1));
  }
  audio.spk.play(waveform, kAudioTxBufferSize);
}

void playSong(void) {
  led3 = 0;
  switch (song_num)
  {
    case 0:
      {
      for (int k = 0; k < 42; k++)
      {
        int length1 = note1[k];
        while (length1--)
        {
          // the loop below will play the note for the duration of 1s
          for (int j = 0; j < 16; ++j)
          {
            queue3.call(playNote, song1[k]);
            wait_us(1);

          }
          if (length1 < 1) wait_us(1000000);
        }
      }
      }
      break;
    case 1:
      {
      for (int k = 0; k < 32; k++)
      {
        int length2 = note2[k];
        while (length2--)
        {
          // the loop below will play the note for the duration of 1s
          for (int j = 0; j < 16; ++j)
          {
            idC = queue3.call(playNote, song2[k]);
          }
          if (length2 < 1) wait_us(1000000);
        }
      }
      }
      break;
    case 2:
      {
      for (int k = 0; k < 24; k++)
      {
        int length3 = note3[k];
        while (length3--)
        {
          // the loop below will play the note for the duration of 1s
          for (int j = 0; j < 16; ++j)
          {
            idC = queue3.call(playNote, song3[k]);
          }
          if (length3 < 1) wait_us(1000000);
        }
      }
      }
      break;
  }
  led3 = 1;
}

void stopPlay(void) {
  led1 = 0;
  queue3.cancel(idC);
  wait_us(100000);
  led1 = 1;
  led2 = 1;
  led3 = 1;
}

void switchCase(void) {
  switch (switchCase_num)
  {
    case 0:
      queue3.call(loadmusic);
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
  thread_songStop.start(callback(&queue5, &EventQueue::dispatch_forever));
  sw2.rise(queue3.event(switchCase));
  sw3.rise(queue5.event(stopPlay));
}