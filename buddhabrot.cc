/*  Copyright © 2011-2013 Henrique Dante de Almeida

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* buddhabrot: a very small but complete implementation of the Buddhabrot
   style rendering of Mandelbrot fractals. */

#include <iostream>
#include <complex>
#include <cstdlib>
#include <gtkmm.h>
#include <atomic>

using namespace Glib;
using namespace Gtk;
using namespace std;
using Cairo::Context;
using Cairo::ImageSurface;

#define S 512
static union {
	guint32 *buff;
	guchar *b;
};

static atomic_uint gray[S*S];
static Cairo::RefPtr<ImageSurface> surface;
static int stride;
void draw(void);

bool on_draw(const Cairo::RefPtr<Context>& cr)
{
	cr->set_source(surface, 0, 0);
	cr->paint();
	return true;
}

void buddhabrot(DrawingArea *darea)
{
	float epsilon = 4.0/S;
	float x, y;
	unsigned jx, jy;
	unsigned M = 1000;
	unsigned m = 100;
	unsigned R = 1500000;
	unsigned i, j, k;
	complex<float> z, c;

	for (k = 0; k < R; k++) {
		x = 4.0*rand()/((float)RAND_MAX+1)-2.0;
		y = 4.0*rand()/((float)RAND_MAX+1)-2.0;
		i = 0;
		c = complex<float>(x, y);
		z = 0;
		while (abs(z) < 2 && i < M) {
			z = z*z + c;
			i++;
		}
		if (i < M && i > m) {
			z = c;
			for (j = 0; j < i-1; j++) {
				jx = (real(z)+2.0)/epsilon;
				jy = (imag(z)+2.0)/epsilon;
				gray[jx*S+jy].fetch_add(1, memory_order_relaxed);
				z = z*z + c;
			}
		}
	}
	draw();

	// TODO: not thread safe
	darea->queue_draw();
}

void draw(void)
{
	unsigned i, j, k, g;
	unsigned max;

	max = 0;
	for (i = 0; i < S*S; i++) {
		g = gray[i].load(memory_order_relaxed);
		if (g > max) max = g;
	}
	for (i = 0; i < S; i++) {
		k = stride*i/4;
		for (j = 0; j < S; j++) {
			g = gray[i*S+j].load(memory_order_relaxed);
			g = min(g, max);
			g = 255*g/max;
			buff[k+j] = g<<16 | g<<8 | g;
		}
	}

}

int main(int argc, char *argv[])
{
	int r;

	RefPtr<Application> app = Application::create(argc, argv, "buddha.brot");
	ApplicationWindow window;
	DrawingArea darea;
	Threads::Thread *thread;

	window.add(darea);
	darea.set_size_request(S, S);
	darea.signal_draw().connect(sigc::ptr_fun(&on_draw));
	darea.show();

	stride = ImageSurface::format_stride_for_width(Cairo::FORMAT_RGB24, S);
	b = new guchar[S*stride];
	surface = ImageSurface::create(b, Cairo::FORMAT_RGB24, S, S, stride);
	thread = Threads::Thread::create(sigc::bind(&buddhabrot, &darea));

	r = app->run(window);
	thread->join();
	delete[] b;
	return r;
}

