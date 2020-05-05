#include "mbed.h"
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
// Gesture parameters
#define mode 3;
#define scroll 3;
EventQueue queue6(32 * EVENTS_EVENT_SIZE);
Thread thread_gesture;
// Event parameters
int event_num = 0;
DigitalOut led1(LED1);
DigitalOut led2(LED2);
InterruptIn sw2(SW2);
InterruptIn sw3(SW3);
EventQueue queue1(32 * EVENTS_EVENT_SIZE);
EventQueue queue2(32 * EVENTS_EVENT_SIZE);
EventQueue queue3(32 * EVENTS_EVENT_SIZE);
Thread thread_add;
Thread thread_reduce;
Thread thread_switch;
// Music parameter
DA7212 audio;
Serial pc(USBTX, USBRX);
int16_t waveform[kAudioTxBufferSize];
EventQueue queue4(32 * EVENTS_EVENT_SIZE);
Thread thread_song;
char serialInBuffer[bufferLength];
int serialCount = 0;
int idC = 0;
#define song_num 3; //how many songs, address of songs
int oldsong_num;
// Should Get from python, but I haven't solve out.
int song[42] = {
  261, 261, 392, 392, 440, 440, 392,
  349, 349, 330, 330, 294, 294, 261,
  392, 392, 349, 349, 330, 330, 294,
  392, 392, 349, 349, 330, 330, 294,
  261, 261, 392, 392, 440, 440, 392,
  349, 349, 330, 330, 294, 294, 261
};
int noteLength[42] = {
  1, 1, 1, 1, 1, 1, 2,
  1, 1, 1, 1, 1, 1, 2,
  1, 1, 1, 1, 1, 1, 2,
  1, 1, 1, 1, 1, 1, 2,
  1, 1, 1, 1, 1, 1, 2,
  1, 1, 1, 1, 1, 1, 2
};

// uLCD parameter
EventQueue queue5(32 * EVENTS_EVENT_SIZE);
Thread thread_uLCD;
uLCD_4DGL uLCD(D1, D0, D2); // serial tx, serial rx, reset pin;

void add_thread(){
    led1 = 0; //red light
    led2 = 1;
    event_num++;
}

void reduce_thread(){
    led1 = 1;
    led2 = 0; //green light
    event_num--;
}

void uLCD_print(){
    uLCD.cls();
    switch (event_num){
        case 0:
        uLCD.printf("\nSTART...\n");
        break;
        case 1:
        uLCD.printf("\nLittle Star\n"); // Song name
        uLCD.printf("\n A famous song...");// Song imformation
        uLCD.text_width(2); //2X size text
        uLCD.text_height(2);
        uLCD.color(BLUE);
        uLCD.locate(1,2);
        uLCD.printf(">|");
        break;
        case 2:
        switch (mode){
            case 0:
            uLCD.background_color(0xFFFFFF)//song list title is white 
            uLCD.printf("\nSong List\n");
            uLCD.printf("\n1. Little Star\n");
            uLCD.printf("\n2. Quicker\n");
            uLCD.printf("\n3. Slower\n");
            uLCD.text_width(2); //2X size text
            uLCD.text_height(2);
            uLCD.color(BLUE);
            uLCD.locate(1,2);
            uLCD.printf("<<  >|  >>");break;
            case 1:
            uLCD.printf("\nSong List\n");
            uLCD.printf("\n1. Little Star\n");
            uLCD.printf("\n2. Quicker\n");
            uLCD.printf("\n3. Slower\n");
            uLCD.text_width(2); //2X size text
            uLCD.text_height(2);
            uLCD.color(BLUE);
            uLCD.locate(1,2);
            uLCD.background_color(0xFFFFFF)//<< is white
            uLCD.printf("<<");
            uLCD.locate(1,3);
            uLCD.printf("  >|  >>");break;
            case 2:
            uLCD.printf("\nSong List\n");
            uLCD.printf("\n1. Little Star\n");
            uLCD.printf("\n2. Quicker\n");
            uLCD.printf("\n3. Slower\n");
            uLCD.text_width(2); //2X size text
            uLCD.text_height(2);
            uLCD.color(BLUE);
            uLCD.locate(1,2);
            uLCD.printf("<<  >|  ");
            uLCD.locate(1,4);
            uLCD.background_color(0xFFFFFF)//>> is white
            uLCD.printf(">>");break;
        }break;
        case 3:
        switch(mode){
            case 0://see what song is white
            switch(song_num){
                case 0:
                uLCD.printf("\nSong List\n");
                uLCD.background_color(0xFFFFFF)//1. is white
                uLCD.printf("\n1. Little Star\n");
                uLCD.printf("\n2. Quicker\n");
                uLCD.printf("\n3. Slower\n");
                uLCD.text_width(2); //2X size text
                uLCD.text_height(2);
                uLCD.color(BLUE);
                uLCD.locate(1,2);
                uLCD.printf("<<  >|  >>");break;
                case 1:
                uLCD.printf("\nSong List\n");
                uLCD.printf("\n1. Little Star\n");
                uLCD.background_color(0xFFFFFF)//2. is white
                uLCD.printf("\n2. Quicker\n");
                uLCD.printf("\n3. Slower\n");
                uLCD.text_width(2); //2X size text
                uLCD.text_height(2);
                uLCD.color(BLUE);
                uLCD.locate(1,2);
                uLCD.printf("<<  >|  >>");break;
                case 3:
                uLCD.printf("\nSong List\n");
                uLCD.printf("\n1. Little Star\n");
                uLCD.printf("\n2. Quicker\n");
                uLCD.background_color(0xFFFFFF)//3. is white
                uLCD.printf("\n3. Slower\n");
                uLCD.text_width(2); //2X size text
                uLCD.text_height(2);
                uLCD.color(BLUE);
                uLCD.locate(1,2);
                uLCD.printf("<<  >|  >>");break;
            }
            break;
            case 1:
            uLCD.printf("\nSong List\n");
            uLCD.printf("\n1. Little Star\n");
            uLCD.printf("\n2. Quicker\n");
            uLCD.printf("\n3. Slower\n");
            uLCD.text_width(2); //2X size text
            uLCD.text_height(2);
            uLCD.color(BLUE);
            uLCD.locate(1,2);
            uLCD.background_color(0xFFFFFF)//<< is white
            uLCD.printf("<<");
            uLCD.locate(1,3);
            uLCD.printf("  >|  >>");break;
            case 2:
            uLCD.printf("\nSong List\n");
            uLCD.printf("\n1. Little Star\n");
            uLCD.printf("\n2. Quicker\n");
            uLCD.printf("\n3. Slower\n");
            uLCD.text_width(2); //2X size text
            uLCD.text_height(2);
            uLCD.color(BLUE);
            uLCD.locate(1,2);
            uLCD.printf("<<  >|  ");
            uLCD.locate(1,4);
            uLCD.background_color(0xFFFFFF)//>> is white
            uLCD.printf(">>");break;
        }break;
        case 4:
        /*
        switch (mode)
        {
            case 0:
            //show song_list[song_num]info
            break;
            case 1:
            //show song_list[oldsong_num]info
            break;
            case 2:
            //show song_list[oldsong_num]info
            break;
        }
        */
       /*
       case 5:
       //need first or later, so not write here
       break
       */
      /*
      case 6:
      //game; haven't any idea
      */
     /*
     case 7:
     //to restart game, same as case 5, not here
     break;
     */
     }
}

void switch_event(){
    queue5.call(uLCD_print);
    queue6.call(gesture_test);
    switch(event_num){
        case 0:
        queue4.call(loadSignalHandler);
        event_num++;
        break;
        case 1:
        /*
        play song_list[song_num]
            for(int i = 0; i < 42; i++){
                int length = noteLength[i];
                while(length--){
                    // the loop below will play the note for the duration of 1s
                    for(int j = 0; j < kAudioSampleFrequency / kAudioTxBufferSize; ++j)
                    {
                        queue4.call(playNoteC(song[i]));
                    }
                    if(length < 1) wait(1.0);
                }
            }
            oldsong_num = song_num;
        break;*/
        case 2:
        stopPlayNoteC();
        mode = gesture_index;
        break;
        case 3:
        switch(mode){
            case 0:
            scroll = gesture_index;
            switch (scroll){
                case 1:
                song_num = (song_num)-1;break;
                case 2:
                song_num = (song_num)+1;break;
            }
            if (scroll<0)
            {
                song_num = 0;
            }
            if (scroll>2)
            {
                song_num = 2;
            }
            case 1:
            break;
            case 2:
            break;
        }
        break;
        case 4:
        switch(gesturea){
            case 0:
            switch (song_num){
                case 0:
                /*
                play new song
                song_list[song_num]???
                
                    for(int i = 0; i < 42; i++){
                        int length = noteLength[i];
                        while(length--){
                        // the loop below will play the note for the duration of 1s
                        for(int j = 0; j < kAudioSampleFrequency / kAudioTxBufferSize; ++j)
                        {
                            queue4.call(playNoteC(song[i]));
                        }
                        if(length < 1) wait(1.0);
                        }
                    }*/
                break;
                case 1://Quicker
                break;
                case 2://Slower
                break;
            }break;
            case 1:
                i = i -10;
                if (i<0)
                {
                    i = 0;
                }
                /*
                play old song
                song_list[oldsong_num]???
                for(int i = 0; i < 42; i++){
                        int length = noteLength[i];
                        while(length--){
                        // the loop below will play the note for the duration of 1s
                        for(int j = 0; j < kAudioSampleFrequency / kAudioTxBufferSize; ++j)
                        {
                            queue4.call(playNoteC(song[i]));
                        }
                        if(length < 1) wait(1.0);
                        }
                    }
                */
            break;
            case 2:
                i = i +10;
                if (i>length(song))
                {
                    i = length(song) -1;
                }
                /*
                play old song
                song_list[oldsong_num]???
                for(int i = 0; i < 42; i++){
                        int length = noteLength[i];
                        while(length--){
                        // the loop below will play the note for the duration of 1s
                        for(int j = 0; j < kAudioSampleFrequency / kAudioTxBufferSize; ++j)
                        {
                            queue4.call(playNoteC(song[i]));
                        }
                        if(length < 1) wait(1.0);
                        }
                    }
                */
            break;
        }
        break;
        case 5:
        /*
        uLCD.cls();
        uLCD.printf("\nPlease wait...\n");
        oldsong_num = song_num; //restore
        event_num = event_num - 4;//go back to case2 and can start again
        */
       break;
       case 6:
       /*
       //game, haven't any idea
       if (event_num == 5 || event_num == 7){
           break; //quit the game
       }
       */
      break;
      case 7:
      /*
      uLCD.cls();
      uLCD.printf("\nPlease wait...\n");
      event_num = event_num - 1;// go back to case 6, restart the game
      */
      break;
    }
}

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

void playNoteC(int i_val) {idC = queue4.call_every(1, playNote, i_val);}

void stopPlayNoteC(void) {queue4.cancel(idC);}


int main() {
    thread_add.start(callback(&queue1, &EventQueue::dispatch_forever));
    thread_reduce.start(callback(&queue2, &EventQueue::dispatch_forever));
    thread_switch.start(callback(&queue3, &EventQueue::dispatch_forever));
    thread_song.start(callback(&queue4, &EventQueue::dispatch_forever));
    thread_uLCD.start(callback(&queue5, &EventQueue::dispatch_forever));
    thread_gesture.start(callback(&queue6, &EventQueue::dispatch_forever));
    // sw2 is used to get to the next step
    sw2.rise(queue1.event(add_thread));
    // sw3 is used to go back to the previous step
    sw3.rise(queue2.event(reduce_thread));
    if (sw2 == 1 && sw3 == 1 ){
        event_num == 6;
    }
    // call switch_event every second, automatically defering to the eventThread
    Ticker eventTicker;
    eventTicker.attach(&queue3.event(&switch_event),1.0f);
    while (1) {wait(1);}
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
    // Maybe it doesn't matter? 
  }
}