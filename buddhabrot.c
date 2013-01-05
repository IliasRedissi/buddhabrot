/*  Copyright 2011 Henrique Dante de Almeida

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

#include <complex.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <stdlib.h>
#include <string.h>

#define S 512
guchar buff[S*S*3];
unsigned gray[S*S];

gboolean on_darea_expose(GtkWidget *widget, GdkEventExpose *event, gpointer user_data)
{
	gdk_draw_rgb_image(widget->window, widget->style->fg_gc[GTK_STATE_NORMAL],
		0, 0, S, S, GDK_RGB_DITHER_NONE, buff, S*3);

	return TRUE;
}

int main (int argc, char *argv[])
{
	GtkWidget *window, *darea;
	float epsilon = 4.0/S;
	float x, y;
	unsigned jx, jy;
	unsigned M = 1000;
	unsigned m = 100;
	unsigned R = 1500000;
	unsigned i, j, k;
	unsigned max;
	float complex z, c;

	memset(gray, 0, sizeof(gray));
	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	darea = gtk_drawing_area_new();
	gtk_widget_set_size_request(darea, S, S);
	gtk_container_add(GTK_CONTAINER(window), darea);
	gtk_signal_connect(GTK_OBJECT(darea), "expose-event",
		GTK_SIGNAL_FUNC(on_darea_expose), NULL);
	gtk_signal_connect(GTK_OBJECT(window), "delete-event",
		GTK_SIGNAL_FUNC(gtk_main_quit), NULL);
	gtk_widget_show_all(window);

	for (k = 0; k < R; k++) {
		x = 4.0*rand()/((float)RAND_MAX+1)-2.0;
		y = 4.0*rand()/((float)RAND_MAX+1)-2.0;
		i = 0;
		c = x + y*I;
		z = 0;
		while (cabsf(z) < 2 && i < M) {
			z = z*z + c;
			i++;
		}
		if (i < M && i > m) {
			z = c;
			for (j = 0; j < i-1; j++) {
				jx = (creal(z)+2.0)/epsilon;
				jy = (cimag(z)+2.0)/epsilon;
				gray[jx*S+jy]++;
				z = z*z + c;
			}
		}
	}

	max = 0;
	for (i = 0; i < S*S; i++) {
		if (gray[i] > max) max = gray[i];
	}
	for (i = 0; i < S*S; i++) {
		buff[3*i] = 255*gray[i]/(float)max;
		buff[3*i+1] = 255*gray[i]/(float)max;
		buff[3*i+2] = 255*gray[i]/(float)max;
	}

	gtk_main();

	return 0;
}

