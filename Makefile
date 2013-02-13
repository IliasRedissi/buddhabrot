GTK=`pkg-config --cflags --libs gtk+-2.0`
GTKMM=`pkg-config --cflags --libs gtkmm-3.0`

all: buddhabrot buddhabrot++

buddhabrot: buddhabrot.c
	cc -O3 -Wall -o buddhabrot buddhabrot.c $(GTK) -lm

buddhabrot++: buddhabrot.cc
	c++ -O3 -Wall -std=c++11 -o buddhabrot++ buddhabrot.cc $(GTKMM)

clean:
	rm -f buddhabrot buddhabrot++
