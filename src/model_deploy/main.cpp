#include "mbed.h"
#include "mbed_events.h"
// Music library
#include <cmath>
#include "DA7212.h"
// Accelerometer library
#include "accelerometer_handler.h"
#include "config.h"
#include "magic_wand_model_data.h"
// Tensorflow library
#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
// uLCD library
#include "uLCD_4DGL.h"
#include <stdlib.h> // srand   rand, C++ cstdlib
#include <time.h>  // time(), C++  ctime
// Gesture parameters
int mode = 0;
int scroll = 0;
int gesture = 0;
EventQueue queue5(32 * EVENTS_EVENT_SIZE);
Thread thread_DNN(osPriorityNormal, 90 * 1024 /*100K stack size*/);
#define bufferLength1 (32)
char serialInBuffer1[bufferLength1];
int serialCount1 = 0;
// Event parameters
int event_num = 0;
InterruptIn sw2(SW2);
InterruptIn sw3(SW3);
EventQueue queue1(32 * EVENTS_EVENT_SIZE);
EventQueue queue2(32 * EVENTS_EVENT_SIZE);
Thread thread_button;
Thread thread_switchCase;
// Music parameter
#define bufferLength (32)
#define musicLength (256)
#define kAudioTxBufferSize 1000
#define kAudioSampleFrequency 16000
DA7212 audio;
Serial pc(USBTX, USBRX);
EventQueue queue3(32 * EVENTS_EVENT_SIZE);
Thread thread_song;
int song_num = 0;
int oldsong_num = 0;
int switchCase_num = 0;
int song1[42], song2[32], song3[24], note1[42], note2[32], note3[24];
int l = 0;

float music[musicLength];
int16_t waveform[kAudioTxBufferSize];
char serialInBuffer[bufferLength];
int serialCount = 0;

DigitalOut led2(LED2);
DigitalOut led3(LED3);

// uLCD parameter
EventQueue queue4(32 * EVENTS_EVENT_SIZE);
Thread thread_uLCD;
uLCD_4DGL uLCD(D1, D0, D2);

void add_event(){
  led2 = 1;
  led3 = 1;
    wait_us(1000);
    event_num++;
}

void reduce_event(){
  led2 = 1;
  led3 = 1;
    wait_us(1000);
    event_num--;
}

// Return the result of the last prediction
int PredictGesture(float* output) {
  // How many times the most recent gesture has been matched in a row
  static int continuous_count = 0;
  // The result of the last prediction
  static int last_predict = -1;

  // Find whichever output has a probability > 0.8 (they sum to 1)
  int this_predict = -1;
  for (int i = 0; i < label_num; i++) {
    if (output[i] > 0.8) this_predict = i;
  }

  // No gesture was detected above the threshold
  if (this_predict == -1) {
    continuous_count = 0;
    last_predict = label_num;
    return label_num;
  }

  if (last_predict == this_predict) {
    continuous_count += 1;
  } else {
    continuous_count = 0;
  }
  last_predict = this_predict;

  // If we haven't yet had enough consecutive matches for this gesture,
  // report a negative result
  if (continuous_count < config.consecutiveInferenceThresholds[this_predict]) {
    return label_num;
  }
  // Otherwise, we've seen a positive result, so clear all our variables
  // and report it
  continuous_count = 0;
  last_predict = -1;

  return this_predict;
}

void gesture_test(){
  led2 = 1;
  led3 = 1;
    // Create an area of memory to use for input, output, and intermediate arrays.
    // The size of this will depend on the model you're using, and may need to be
    // determined by experimentation.
  constexpr int kTensorArenaSize = 60 * 1024;
  uint8_t tensor_arena[kTensorArenaSize];

  // Whether we should clear the buffer next time we fetch data
  bool should_clear_buffer = false;
  bool got_data = false;

  // The gesture index of the prediction
  int gesture_index;

  // Set up logging.
  static tflite::MicroErrorReporter micro_error_reporter;
  tflite::ErrorReporter* error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  const tflite::Model* model = tflite::GetModel(g_magic_wand_model_data);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    error_reporter->Report(
        "Model provided is schema version %d not equal "
        "to supported version %d.",
        model->version(), TFLITE_SCHEMA_VERSION);
    return -1;
  }

  // Pull in only the operation implementations we need.
  // This relies on a complete list of all the ops needed by this graph.
  // An easier approach is to just use the AllOpsResolver, but this will
  // incur some penalty in code space for op implementations that are not
  // needed by this graph.
  static tflite::MicroOpResolver<5> micro_op_resolver;
  micro_op_resolver.AddBuiltin(
      tflite::BuiltinOperator_DEPTHWISE_CONV_2D,
      tflite::ops::micro::Register_DEPTHWISE_CONV_2D());
  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_MAX_POOL_2D,
                               tflite::ops::micro::Register_MAX_POOL_2D());
  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_CONV_2D,
                               tflite::ops::micro::Register_CONV_2D());
  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_FULLY_CONNECTED,
                               tflite::ops::micro::Register_FULLY_CONNECTED());
  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_SOFTMAX,
                               tflite::ops::micro::Register_SOFTMAX());
  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_RESHAPE,
                               tflite::ops::micro::Register_RESHAPE(), 1);

  // Build an interpreter to run the model with
  static tflite::MicroInterpreter static_interpreter(
      model, micro_op_resolver, tensor_arena, kTensorArenaSize, error_reporter);
  tflite::MicroInterpreter* interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors
  interpreter->AllocateTensors();

  // Obtain pointer to the model's input tensor
  TfLiteTensor* model_input = interpreter->input(0);
  if ((model_input->dims->size != 4) || (model_input->dims->data[0] != 1) ||
      (model_input->dims->data[1] != config.seq_length) ||
      (model_input->dims->data[2] != kChannelNumber) ||
      (model_input->type != kTfLiteFloat32)) {
    error_reporter->Report("Bad input tensor parameters in model");
    return -1;
  }

  int input_length = model_input->bytes / sizeof(float);

  TfLiteStatus setup_status = SetupAccelerometer(error_reporter);
  if (setup_status != kTfLiteOk) {
    error_reporter->Report("Set up failed\n");
    return -1;
  }

  error_reporter->Report("Set up successful...\n");

  while (true) {

    // Attempt to read new data from the accelerometer
    got_data = ReadAccelerometer(error_reporter, model_input->data.f,
                                 input_length, should_clear_buffer);

    // If there was no new data,
    // don't try to clear the buffer again and wait until next time
    if (!got_data) {
      should_clear_buffer = false;
      continue;
    }

    // Run inference, and report any error
    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk) {
      error_reporter->Report("Invoke failed on index: %d\n", begin_index);
      continue;
    }

    // Analyze the results to obtain a prediction
    gesture_index = PredictGesture(interpreter->output(0)->data.f);

    // Clear the buffer next time we read data
    should_clear_buffer = gesture_index < label_num;

    // Produce an output
    if (gesture_index < label_num) {
      error_reporter->Report(config.output_message[gesture_index]);
      if(pc.readable())
      {
      serialInBuffer1[serialCount1] = pc.getc();
      serialCount1++;
      if(serialCount1 == 1)
       {
        serialInBuffer1[serialCount1] = '\0';
        gesture = (float) atof(serialInBuffer1);
        serialCount1 = 0;
       }
      }
    }
  }
}

void uLCD_print(){
  // Can copy from test-uLCD_print
  led2=1;
  led3=1;
uLCD.cls();
uLCD.background_color(0x000000);
switch (event_num) {
case 0:
  uLCD.printf("\nSTART...\n");
  break;
case 1:
  uLCD.color(RED);
  uLCD.printf("\nLittle Star\n"); // Song name
  uLCD.color(GREEN);
  uLCD.printf("\n A famous song...");// Song imformation
  uLCD.text_width(2); //2X size text
  uLCD.text_height(2);
  uLCD.color(BLUE);
  uLCD.locate(4, 6);
  uLCD.printf(">|");
  break;
case 2:
  switch (mode) {
    case 0:
      uLCD.color(0xFFFFFF);
      uLCD.printf("\nSong List\n");
      uLCD.color(GREEN);
      uLCD.printf("\n1. Little Star\n");
      uLCD.printf("\n2. Two Tigers\n");
      uLCD.printf("\n3. London Bridge\n");
      uLCD.text_width(2); //2X size text
      uLCD.text_height(2);
      uLCD.color(BLUE);
      uLCD.locate(1, 6);
      uLCD.printf("<< >| >>");
      break;
    case 1:
      uLCD.color(GREEN);
      uLCD.printf("\nSong List\n");
      uLCD.printf("\n1. Little Star\n");
      uLCD.printf("\n2. Two Tigers\n");
      uLCD.printf("\n3. London Bridge\n");
      uLCD.text_width(2); //2X size text
      uLCD.text_height(2);
      uLCD.locate(1, 6);
      uLCD.color(0xFFFFFF);//<< is white
      uLCD.printf("<<");
      uLCD.color(BLUE);
      uLCD.locate(3, 6);
      uLCD.printf(" >| >>");
      break;
    case 2:
      uLCD.color(GREEN);
      uLCD.printf("\nSong List\n");
      uLCD.printf("\n1. Little Star\n");
      uLCD.printf("\n2. Two Tigers\n");
      uLCD.printf("\n3. London Bridge\n");
      uLCD.text_width(2); //2X size text
      uLCD.text_height(2);
      uLCD.color(BLUE);
      uLCD.locate(1, 6);
      uLCD.printf("<<  >| ");
      uLCD.locate(6, 6);
      uLCD.color(0xFFFFFF);//>> is white
      uLCD.printf(" >>");
      break;
  }
  break;
case 3:
  switch (mode) {
    case 0://see what song is white
      switch (song_num) {
        case 0:
          uLCD.color(GREEN);
          uLCD.printf("\nSong List\n");
          uLCD.color(0xFFFFFF);//1. is white
          uLCD.printf("\n1. Little Star\n");
          uLCD.color(GREEN);
          uLCD.printf("\n2. Two Tigers\n");
          uLCD.printf("\n3. London Bridge\n");
          uLCD.text_width(2); //2X size text
          uLCD.text_height(2);
          uLCD.color(BLUE);
          uLCD.locate(1, 6);
          uLCD.printf("<< >| >>");
          break;
        case 1:
          uLCD.color(GREEN);
          uLCD.printf("\nSong List\n");
          uLCD.printf("\n1. Little Star\n");
          uLCD.color(0xFFFFFF);//2. is white
          uLCD.printf("\n2. Two Tigers\n");
          uLCD.color(GREEN);
          uLCD.printf("\n3. London Bridge\n");
          uLCD.text_width(2); //2X size text
          uLCD.text_height(2);
          uLCD.color(BLUE);
          uLCD.locate(1, 6);
          uLCD.printf("<< >| >>"); break;
        case 2:
          uLCD.color(GREEN);
          uLCD.printf("\nSong List\n");
          uLCD.printf("\n1. Little Star\n");
          uLCD.printf("\n2. Two Tigers\n");
          uLCD.color(0xFFFFFF);//3. is white
          uLCD.printf("\n3. London Bridge\n");
          uLCD.text_width(2); //2X size text
          uLCD.text_height(2);
          uLCD.color(BLUE);
          uLCD.locate(1, 6);
          uLCD.printf("<< >| >>"); break;
      }
      break;
    case 1:
      uLCD.color(GREEN);
      uLCD.printf("\nSong List\n");
      uLCD.printf("\n1. Little Star\n");
      uLCD.printf("\n2. Two Tigers\n");
      uLCD.printf("\n3. London Bridge\n");
      uLCD.text_width(2); //2X size text
      uLCD.text_height(2);
      uLCD.locate(1, 6);
      uLCD.color(0xFFFFFF);//<< is white
      uLCD.printf("<<");
      uLCD.color(BLUE);
      uLCD.locate(3, 6);
      uLCD.printf(" >| >>"); break;
    case 2:
      uLCD.color(GREEN);
      uLCD.printf("\nSong List\n");
      uLCD.printf("\n1. Little Star\n");
      uLCD.printf("\n2. Two Tigers\n");
      uLCD.printf("\n3. London Bridge\n");
      uLCD.text_width(2); //2X size text
      uLCD.text_height(2);
      uLCD.color(BLUE);
      uLCD.locate(1, 6);
      uLCD.printf("<< >| ");
      uLCD.locate(6, 6);
      uLCD.color(0xFFFFFF);//>> is white
      uLCD.printf(" >>");
      break;
  }
  break;
case 4:
  switch (mode)
  {
    case 0:
      //show song_list[song_num]info
      uLCD.color(RED);
      uLCD.printf("\nTwo Tigers\n"); // Song name
      uLCD.color(GREEN);
      uLCD.printf("\n A famous song...");// Song imformation
      uLCD.text_width(2); //2X size text
      uLCD.text_height(2);
      uLCD.color(BLUE);
      uLCD.locate(4, 6);
      uLCD.printf(">|");
      break;
    case 1:
      //show previous song info
      uLCD.color(RED);
      uLCD.printf("\nLondon Bridge\n"); // Song name
      uLCD.color(GREEN);
      uLCD.printf("\n A famous song...");// Song imformation
      uLCD.text_width(2); //2X size text
      uLCD.text_height(2);
      uLCD.color(BLUE);
      uLCD.locate(4, 6);
      uLCD.printf(">|");
      break;
    case 2:
      //show next song info
      uLCD.color(RED);
      uLCD.printf("\nTwo Tigers\n"); // Song name
      uLCD.color(GREEN);
      uLCD.printf("\n A famous song...");// Song imformation
      uLCD.text_width(2); //2X size text
      uLCD.text_height(2);
      uLCD.color(BLUE);
      uLCD.locate(4, 6);
      uLCD.printf(">|");
      break;
  }

case 5:
  //need first or later, so not write here
  uLCD.cls();
  break;

case 6:
  //load basic
  { 
    uLCD.cls();
    int score = 0;
    int circle[42];
    for (int r = 0; r < 42; r++)
    {
      circle[r] = rand() % 2 + 1;
    }
    uLCD.filled_rectangle(0, 95 , 160, 105, 0xFFD306); //beat line at 100, error is +/-10
    for (int r1 = 0; r1 < 42; r1++)
    {
      for (int y = 0; y <= 85; y = y + 10) //the lowest edge is 120, beat line is at 100, error is +/-10
      {
        if ( circle[r1] == 1 )
        {
          uLCD.filled_circle(80 ,  30 + y , 4, 0xFF0000);
          if ((scroll == 1) && (y >= 60) && (y <= 80)) //red=left hand AND 90<=y<=110
          {
            score = score + 1;
            uLCD.locate(0, 5);
            uLCD.color(GREEN);
            uLCD.printf("Good");
            uLCD.filled_circle(80, 30 + y , 4, BLACK);
            y = 86;
          }
          else //miss
          {
            uLCD.locate(0, 5);
            uLCD.color(GREEN);
            uLCD.printf("Miss");
          }
        }
        if ( circle[r1] == 2 )
        {
          uLCD.filled_circle(80 , 30 + y , 4, 0x46A3FF); // beat line position is 100
          if ((scroll == 2) && (y >= 60) && (y <= 80)) //blue=right hand AND 90<=y<=110
          {
            score = score + 1;
            uLCD.locate(0, 5);
            uLCD.color(GREEN);
            uLCD.printf("Good");
            uLCD.filled_circle(80, 30 + y , 4, BLACK);
            y = 86;
          }
          else //miss
          {
            uLCD.locate(0, 5);
            uLCD.color(GREEN);
            uLCD.printf("Miss");
          }
        }
        uLCD.filled_circle(80, 30 + y , 4, BLACK);
        uLCD.locate(0, 0);
        uLCD.color(RED);
        uLCD.printf("score:%d", score);
      }
    }
    //print score result
      uLCD.cls(); // circle disappear
      uLCD.color(RED);
      uLCD.printf("\nscore:\n");
      uLCD.locate(6, 7); //at center
      uLCD.text_width(4); //4X size text
      uLCD.text_height(4);
      uLCD.color(0xFF0000);
      uLCD.printf("%d", score);
    break;
  }
case 7:
  uLCD.cls();
  break;
}
}

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
            queue3.call(playNote, song2[k]);
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
            queue3.call(playNote, song3[k]);
          }
          if (length3 < 1) wait_us(1000000);
        }
      }
      }
      break;
  }
  led3 = 1;
}

void switch_event() {
  led2=1;
  led3=1;
  switch (event_num) {
    case 1:
      queue3.call(playSong);
      oldsong_num = song_num;
      break;
    case 2:
      //not sure if it would be overflow? gesture_test();
      mode = gesture;
      break;
    case 3:
      switch (mode) {
        case 0:
          //not sure if it would be overflow? gesture_test());
          scroll = gesture;
          switch (scroll)
          {
            case 0://Nothing
              break;
            case 1:
              song_num = song_num - 1;
              break;
            case 2:
              song_num = song_num + 1;
              break;
          }
          if (scroll < 0)
          {
            song_num = 0;
          }
          if (scroll > 2)
          {
            song_num = 2;
          }
          break;
        case 1:
          if (song_num == 0) {
            song_num = 2;
          }
          else {
            song_num = song_num - 1;
          }
          break;
        case 2:
          if (song_num == 2) {
            song_num = 0;
          }
          else {
            song_num = song_num + 1;
          }
          break;
      }
      break;
    case 4:
      queue3.call(playSong);
      break;
    case 5:
      uLCD.cls();
      uLCD.printf("\nPlease wait...\n");
      oldsong_num = song_num; //restore
      event_num = event_num - 4;//go back to case2 and can start again
      break;
    case 6:
      wait_us(5000); //play music slower than circle for 5 sec, let circle can run to beat line after 5 sec
      queue3.call(playSong);
      if (event_num == 5 || event_num == 7)
      {
        break; //quit the game
      }
      break;
    case 7:
      uLCD.cls();
      uLCD.printf("\nPlease wait...\n");
      event_num = event_num - 1;// go back to case 6, restart the game
      break;
  }
}

int main(void) {
  led2 = 1;
  led3 = 1;
    thread_button.start(callback(&queue1, &EventQueue::dispatch_forever));
    thread_switchCase.start(callback(&queue2, &EventQueue::dispatch_forever));
    thread_song.start(callback(&queue3, &EventQueue::dispatch_forever));
    thread_uLCD.start(callback(&queue4, &EventQueue::dispatch_forever));
    thread_DNN.start(callback(&queue5, &EventQueue::dispatch_forever));
    queue3.call(loadmusic);
    event_num = 1;
    // sw2 is used to get to the next step
    sw2.rise(queue1.event(add_event));
    // sw3 is used to go back to the previous step
    sw3.rise(queue1.event(reduce_event));
    //DNN
    Ticker DNNTicker;
    DNNTicker.attach(queue5.event(&gesture_test),1.0f);
    gesture_test(); 
    // call switch_event every second, automatically defering to the eventThread
    Ticker eventTicker;
    eventTicker.attach(queue2.event(&switch_event),1.0f);
    Ticker uLCDTicker;
    uLCDTicker.attach(queue4.event(&uLCD_print),1.0f);
    while (1) {wait_us(1000);}
}