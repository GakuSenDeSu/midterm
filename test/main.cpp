#include "mbed.h"
#include "uLCD_4DGL.h"

uLCD_4DGL uLCD(D1, D0, D2);
int event_num = 0;
int mode = 0;

int main()
{
uLCD.cls();
switch (event_num) {
case 0:
  uLCD.printf("\nSTART...\n");
  break;
case 1:
  uLCD.printf("\nLittle Star\n"); // Song name
  uLCD.printf("\n A famous song...");// Song imformation
  uLCD.text_width(2); //2X size text
  uLCD.text_height(2);
  uLCD.color(BLUE);
  uLCD.locate(1, 2);
  uLCD.printf(">|");
  break;
case 2:
  switch (mode) {
    case 0:
      uLCD.background_color(0xFFFFFF);//song list title is white
      uLCD.printf("\nSong List\n");
      uLCD.printf("\n1. Little Star\n");
      uLCD.printf("\n2. Quicker\n");
      uLCD.printf("\n3. Slower\n");
      uLCD.text_width(2); //2X size text
      uLCD.text_height(2);
      uLCD.color(BLUE);
      uLCD.locate(1, 2);
      uLCD.printf("<<  >|  >>");
      break;
    case 1:
      uLCD.printf("\nSong List\n");
      uLCD.printf("\n1. Little Star\n");
      uLCD.printf("\n2. Quicker\n");
      uLCD.printf("\n3. Slower\n");
      uLCD.text_width(2); //2X size text
      uLCD.text_height(2);
      uLCD.color(BLUE);
      uLCD.locate(1, 2);
      uLCD.background_color(0xFFFFFF);//<< is white
      uLCD.printf("<<");
      uLCD.locate(1, 3);
      uLCD.printf("  >|  >>");
      break;
    case 2:
      uLCD.printf("\nSong List\n");
      uLCD.printf("\n1. Little Star\n");
      uLCD.printf("\n2. Quicker\n");
      uLCD.printf("\n3. Slower\n");
      uLCD.text_width(2); //2X size text
      uLCD.text_height(2);
      uLCD.color(BLUE);
      uLCD.locate(1, 2);
      uLCD.printf("<<  >|  ");
      uLCD.locate(1, 4);
      uLCD.background_color(0xFFFFFF);//>> is white
      uLCD.printf(">>");
      break;
  }
  break;
case 3:
  switch (mode) {
    case 0://see what song is white
      switch (song_num) {
        case 0:
          uLCD.printf("\nSong List\n");
          uLCD.background_color(0xFFFFFF);//1. is white
          uLCD.printf("\n1. Little Star\n");
          uLCD.printf("\n2. Quicker\n");
          uLCD.printf("\n3. Slower\n");
          uLCD.text_width(2); //2X size text
          uLCD.text_height(2);
          uLCD.color(BLUE);
          uLCD.locate(1, 2);
          uLCD.printf("<<  >|  >>");
          break;
        case 1:
          uLCD.printf("\nSong List\n");
          uLCD.printf("\n1. Little Star\n");
          uLCD.background_color(0xFFFFFF);//2. is white
          uLCD.printf("\n2. Quicker\n");
          uLCD.printf("\n3. Slower\n");
          uLCD.text_width(2); //2X size text
          uLCD.text_height(2);
          uLCD.color(BLUE);
          uLCD.locate(1, 2);
          uLCD.printf("<<  >|  >>"); break;
        case 2:
          uLCD.printf("\nSong List\n");
          uLCD.printf("\n1. Little Star\n");
          uLCD.printf("\n2. Quicker\n");
          uLCD.background_color(0xFFFFFF);//3. is white
          uLCD.printf("\n3. Slower\n");
          uLCD.text_width(2); //2X size text
          uLCD.text_height(2);
          uLCD.color(BLUE);
          uLCD.locate(1, 2);
          uLCD.printf("<<  >|  >>"); break;
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
      uLCD.locate(1, 2);
      uLCD.background_color(0xFFFFFF);//<< is white
      uLCD.printf("<<");
      uLCD.locate(1, 3);
      uLCD.printf("  >|  >>"); break;
    case 2:
      uLCD.printf("\nSong List\n");
      uLCD.printf("\n1. Little Star\n");
      uLCD.printf("\n2. Quicker\n");
      uLCD.printf("\n3. Slower\n");
      uLCD.text_width(2); //2X size text
      uLCD.text_height(2);
      uLCD.color(BLUE);
      uLCD.locate(1, 2);
      uLCD.printf("<<  >|  ");
      uLCD.locate(1, 4);
      uLCD.background_color(0xFFFFFF);//>> is white
      uLCD.printf(">>");
      break;
  }
  break;
case 4:
  switch (mode)
  {
    case 0:
      //show song_list[song_num]info
      //show song_list[oldsong_num]info
      uLCD.printf("\nLittle Star\n"); // Song name
      uLCD.printf("\n A famous song...");// Song imformation
      uLCD.text_width(2); //2X size text
      uLCD.text_height(2);
      uLCD.color(BLUE);
      uLCD.locate(1, 2);
      uLCD.printf(">|");
      break;
    case 1:
      //show song_list[oldsong_num]info
      uLCD.printf("\nLittle Star\n"); // Song name
      uLCD.printf("\n A famous song...");// Song imformation
      uLCD.text_width(2); //2X size text
      uLCD.text_height(2);
      uLCD.color(BLUE);
      uLCD.locate(1, 2);
      uLCD.printf(">|");
      break;
    case 2:
      //show song_list[oldsong_num]info
      //show song_list[oldsong_num]info
      uLCD.printf("\nLittle Star\n"); // Song name
      uLCD.printf("\n A famous song...");// Song imformation
      uLCD.text_width(2); //2X size text
      uLCD.text_height(2);
      uLCD.color(BLUE);
      uLCD.locate(1, 2);
      uLCD.printf(">|");
      break;
  }

case 5:
  //need first or later, so not write here
  break;

case 6:
  //load basic
  int score = 0;
  int circle[42];
  for (int r = 0; r < 42; r++)
  {
    circle[r] = rand() % 1 + 1;
  }
  uLCD.rectangle(0, 5 , 50, 15, 0xFFD306); //beat line at 10, error is +/-5
  uLCD.locate(30, 30);
  uLCD.printf("score: ");
  uLCD.printf("%D", score);
  for (int r1 = 0; r1 < 42; r1++)
  {
    if ( circle[r1] == 1 )
    {
      for (int y = 0; y <= 47; y = y + 10) //the highest edge is 50, beat line is at 10, error is +/-5
      {
        uLCD.filled_circle(1 , 50 - y , 4, 0xFF0000);
        if ((scroll == 1) && (y >= 5) && (y <= 15)) //red=left hand AND 5<=y<=15
        {
          uLCD.cls(); // circle disappear
          score = score + 1;
          uLCD.locate(30, 30); //Right top corner
          uLCD.printf("score:");
          uLCD.printf("%D", score);
        }
        if ((scroll == 1) && (y < 5)) //miss
        {
          uLCD.cls(); // circle disappear
          uLCD.locate(30, 30); //Right top corner
          uLCD.printf("score:");
          uLCD.printf("%D", score);
          uLCD.locate(30, 5);
          uLCD.color(0x00FFFF);
          uLCD.printf("Miss");
        }
      }
      //but we need to forward 5 sec to show up
    }
    if ( circle[r1] == 2 ) {
      for (int y = 0; y <= 47; y = y + 10) //the highest edge is 50, beat line is at 10, error is +/-5
      {
        uLCD.filled_circle(1 , 50 - y , 4, 0x46A3FF); // beat line position is below 10
        if ((scroll == 2) && (y >= 5) && (y <= 15)) //red=left hand AND 5<=y<=15
        {
          uLCD.cls(); // circle disappear
          score = score + 1;
          uLCD.locate(30, 30); //Right top corner
          uLCD.printf("score:");
          uLCD.printf("%D", score);
        }
        if (scroll == 2 && y < 5) //miss
        {
          uLCD.cls(); // circle disappear
          uLCD.locate(30, 30); //Right top corner
          uLCD.printf("score:");
          uLCD.printf("%D", score);
          uLCD.locate(30, 5);
          uLCD.color(0x00FFFF);
          uLCD.printf("Miss");
        }
      }
    }
    //print score result
    uLCD.cls(); // circle disappear
    uLCD.locate(1, 2); //at center
    uLCD.printf("\nscore:\n");
    uLCD.text_width(2); //4X size text
    uLCD.text_height(2);
    uLCD.color(0xFF0000);
    uLCD.printf("%D", score);
  }
  break;
case 7:
  //to restart game, same as case 5, not here
  break;
}

}