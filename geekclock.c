#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <time.h>
#include <pthread.h>

/* The window which contains the text. */

struct
{
    int width;
    int height;
    char text[256];
    int text_len;

    /* X Windows related variables. */
    Display * display;
    int screen;
    Window root;
    Window window;
    GC gc;
    XFontStruct * font;
    unsigned long black_pixel;
    unsigned long white_pixel;
}
text_box;

/* Connect to the display, set up the basic variables. */
static void x_connect()
{
    text_box.display = XOpenDisplay (NULL);
    if (! text_box.display) {
        fprintf (stderr, "Could not open display.\n");
        exit (1);
    }
    text_box.screen = DefaultScreen (text_box.display);
    text_box.root = RootWindow (text_box.display, text_box.screen);
    text_box.black_pixel = BlackPixel (text_box.display, text_box.screen);
    text_box.white_pixel = WhitePixel (text_box.display, text_box.screen);
}

/* Create the window. */
static void create_window()
{
    text_box.width = 100;
    text_box.height = 30;
    text_box.window =
        XCreateSimpleWindow (text_box.display,
                             text_box.root,
                             1, /* x */
                             1, /* y */
                             text_box.width,
                             text_box.height,
                             0, /* border width */
                             text_box.black_pixel, /* border pixel */
                             text_box.white_pixel  /* background */);
    XSelectInput (text_box.display, text_box.window,
                  ExposureMask);
    XMapWindow (text_box.display, text_box.window);
}

/* Set up the GC (Graphics Context). */
static void set_up_gc()
{
    text_box.screen = DefaultScreen (text_box.display);
    text_box.gc = XCreateGC (text_box.display, text_box.window, 0, 0);
    XSetBackground (text_box.display, text_box.gc, text_box.white_pixel); 
    XSetForeground (text_box.display, text_box.gc, text_box.black_pixel); 
}

/* Set up the text font. */
static void set_up_font ()
{
//    const char * fontname = "-*-helvetica-*-r-*-*-14-*-*-*-*-*-*-*";
    const char * fontname = "-*-fixed-*-r-*-*-20-*-*-*-*-*-*-*";
    text_box.font = XLoadQueryFont (text_box.display, fontname);
    /* If the font could not be loaded, revert to the "fixed" font. */
    if (! text_box.font) {
        fprintf (stderr, "unable to load font %s: using fixed\n", fontname);
        text_box.font = XLoadQueryFont (text_box.display, "fixed");
    }
    XSetFont (text_box.display, text_box.gc, text_box.font->fid);
}

/* Draw the window. */
static void draw_screen ()
{
    int x;
    int y;
    int direction;
    int ascent;
    int descent;
    XCharStruct overall;

    time_t t = time(0);   // get time now
    struct tm * now = localtime(&t);
    sprintf(text_box.text, "%02x:%02x:%02x", now->tm_hour, now->tm_min, now->tm_sec);
    text_box.text_len = strlen (text_box.text);


    /* Centre the text in the middle of the box. */
    XTextExtents (text_box.font, text_box.text, text_box.text_len,
                  & direction, & ascent, & descent, & overall);
    x = (text_box.width - overall.width) / 2;
    y = text_box.height / 2 + (ascent - descent) / 2;
    XClearWindow (text_box.display, text_box.window);
    XDrawString (text_box.display, text_box.window, text_box.gc,
                 x, y, text_box.text, text_box.text_len);
}

/* Loop over events. */
static void event_loop()
{
    fd_set in_fds;
    struct timeval tv;
    int x11_fd = ConnectionNumber(text_box.display);

    while (1)
    {
        FD_ZERO(&in_fds);
        FD_SET(x11_fd, &in_fds);

        // Set our timer.  One second sounds good.
        tv.tv_usec = 0;
        tv.tv_sec = 1;

        // Wait for X Event or a Timer
        int num_ready_fds = select(1, &in_fds, NULL, NULL, &tv);
        if (num_ready_fds == 0)
	{
            // Handle timer here
	    draw_screen();
	}

        // Handle XEvents and flush the input
        XEvent e;
        while(XPending(text_box.display))
	{
            XNextEvent(text_box.display, &e);
            if (e.type == Expose)
	    {
                draw_screen();
            }
	}
    }
}

int main (int argc, char ** argv)
{
    time_t t = time(0);   // get time now
    struct tm * now = localtime(&t);
    sprintf(text_box.text, "%2x:%2x:%2x", now->tm_hour, now->tm_min, now->tm_sec);
    text_box.text_len = strlen (text_box.text);

    x_connect ();
    create_window ();
    set_up_gc ();
    set_up_font ();
    event_loop ();
    return 0;
}
