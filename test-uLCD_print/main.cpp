#include "mbed.h"
#include "uLCD_4DGL.h"

uLCD_4DGL uLCD(D1, D0, D2);
int event_num = 6;
int mode = 0;
int song_num = 0;
int scroll = 1;


int main()
{
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
      uLCD.printf("\n2. Quicker\n");
      uLCD.printf("\n3. Slower\n");
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
      uLCD.printf("\n2. Quicker\n");
      uLCD.printf("\n3. Slower\n");
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
      uLCD.printf("\n2. Quicker\n");
      uLCD.printf("\n3. Slower\n");
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
          uLCD.printf("\n2. Quicker\n");
          uLCD.printf("\n3. Slower\n");
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
          uLCD.printf("\n2. Quicker\n");
          uLCD.color(GREEN);
          uLCD.printf("\n3. Slower\n");
          uLCD.text_width(2); //2X size text
          uLCD.text_height(2);
          uLCD.color(BLUE);
          uLCD.locate(1, 6);
          uLCD.printf("<< >| >>"); break;
        case 2:
          uLCD.color(GREEN);
          uLCD.printf("\nSong List\n");
          uLCD.printf("\n1. Little Star\n");
          uLCD.printf("\n2. Quicker\n");
          uLCD.color(0xFFFFFF);//3. is white
          uLCD.printf("\n3. Slower\n");
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
      uLCD.printf("\n2. Quicker\n");
      uLCD.printf("\n3. Slower\n");
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
      uLCD.printf("\n2. Quicker\n");
      uLCD.printf("\n3. Slower\n");
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
      uLCD.printf("\nLittle Star\n"); // Song name
      uLCD.color(GREEN);
      uLCD.printf("\n A famous song...");// Song imformation
      uLCD.text_width(2); //2X size text
      uLCD.text_height(2);
      uLCD.color(BLUE);
      uLCD.locate(4, 6);
      uLCD.printf(">|");
      break;
    case 1:
      //show song_list[oldsong_num]info
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
      //show song_list[oldsong_num]info
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
        //but we need to forward 5 sec to show up
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