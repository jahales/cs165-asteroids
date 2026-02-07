###############################################################################
# Class: CS 165		Instructor: Brother Helfrich
# Project 4: Asteroids
#		Authors:
#			Jacob Hales
#			Sam Buchmiller
#		Estimated time:
#			30 hrs? + 12 hrs
# Sound:
#    To get the sound to work in the Linux lab, either use headphones or
#    unmute the Mono channel (it's muted by default and it isn't displayed
#    by default in the audio dialog, you'd have to play around with the
#    settings)
#
# Credits:
#    Music: G.D. Peiskee http://pianosociety.com/cms/index.php?section=180
#       "Prelude in G-sharp Minor, Sergei Rachmaninov, Op. 32 No. 12"
#    Sound Effects: the original Asteroids sounds are floating around on the
#       internet, possible downloads include:
#       http://www.codeproject.com/script/Articles/ViewDownloads.aspx?aid=7428
#    Artwork: most of the artwork was custom-made using GIMP
#    Other: where explicitly stated, SDL documentation and similar resources
#       were relied on heavily to make it work (mostly for dealing with
#       audio and bitmap files).  
###############################################################################

###############################################################################
# The main programs are:
#    game:          The playable game
#    debug:			The testing version (includes asserts)
#    vectorTest:    Test vector.cpp
#    audioTest:		Test audio.cpp
###############################################################################
game : main.o environment.o entity.o vector.o graphics.o sound.o audio.o texture.o sprite.o
	g++ -o game *.o -lrt -lGL -lGLU -lSDL -lpthread -DNDEBUG

debug : main.o environment.o entity.o vector.o graphics.o sound.o audio.o texture.o sprite.o
	g++ -o game *.o -lrt -lGL -lGLU -lSDL -lpthread

vectorTest : vectorTest.o vector.o vector.h
	g++ -o vectorTest vectorTest.o vector.o 

audioTest : audioTest.o audio.o sound.o sound.h audio.h
	g++ -o audioTest audioTest.o audio.o sound.o -lSDL -lpthread

###############################################################################
# Game objects
###############################################################################
main.o: main.cpp environment.cpp
	g++ -c -w main.cpp

environment.o : environment.cpp environment.h graphics.h entity.h sprite.h audio.h
	g++ -c -w environment.cpp

entity.o : entity.cpp entity.h vector.h sprite.h graphics.h
	g++ -c -w entity.cpp

vector.o : vector.cpp vector.h
	g++ -c -w vector.cpp 

vectorTest.o : vectorTest.cpp vector.h vector.cpp
	g++ -c -w vectorTest.cpp 

###############################################################################
# Audio, Sound, and Graphics
###############################################################################
graphics.o : graphics.cpp graphics.h texture.h
	g++ -c -w graphics.cpp

sprite.o : sprite.cpp sprite.h texture.h graphics.h
	g++ -c -w sprite.cpp

texture.o : texture.cpp texture.h
	g++ -c -w texture.cpp

audio.o : audio.cpp audio.h sound.h
	g++ -c -w audio.cpp 

sound.o : sound.cpp sound.h
	g++ -c -w sound.cpp 

audioTest.o : audioTest.cpp audio.h sound.h
	g++ -c -w audioTest.cpp 

###############################################################################
# General rules
###############################################################################
package :
	tar -czf project4.tar.gz *.h *.cpp makefile sound images

clean :
	rm -f vectorTest audioTest debug game *.o *~ *.tar *# \\n

all :  vectorTest debug game audioTest

