/******************************************************************************
 * audioTest.cpp: this is a driver program for testing functionality of the
 *    audio manager class. It can play or loop a given WAV file.
 *****************************************************************************/
#include "sound.h"
#include "audio.h"
#include <string>
#include <iostream>
using namespace std;

/******************************************************************************
 * prompt:
 *****************************************************************************/
string prompt(string text = "file: ")
{
   string buff;
   cout << text;
   getline(cin, buff);
   return buff;
}

/******************************************************************************
 * main: this is a driver program for testing functionality of the
 *    audio manager class. It can play or loop a given WAV file.
 *****************************************************************************/
int main(int argc, char **argv)
{  
    AudioManager manager;

    cout << "Audio test (enter 'help' for options)\n\n";

    while (cout << "> ")
    {
       try
       {
          string buff;
          getline(cin, buff);

          if (buff == "play")
          {
             manager.play(prompt());
          }
          else if (buff == "loop")
          {
             manager.play(prompt(), true);
          }
          else if (buff == "quit")
          {
             break;
          }
          else if (buff == "stop")
          {
             manager.stop(prompt());
          }
          else if (buff == "help")
          {
             cout << "play:   play the specified file a single time\n"
                  << "help:   display this help menu\n"
                  << "loop:   loop the specified file\n"
                  << "stop:   stops the specified sound\n"
                  << "quit:   safely quit the program\n"
                  << "+   :   increase volume\n"
                  << "-   :   decrease volume\n";
          }
          else if (buff == "+")
          {
             manager.adjustVolume(5);
          }
          else if (buff == "-")
          {
             manager.adjustVolume(-5);
          }
          else
          {
             cout << "unrecognized command\n";
          }
       }
       catch (string ex)
       {
          cout << ex << endl;
       }
    }

    return 0;
}