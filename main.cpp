#include "mbed.h"
#include "Arial12x12.h"
#include "Arial24x23.h"
#include "Arial43x48_numb.h"
#include "pict.h"
#include "pavement_48x34.h"
#include "ILI9341.h"

const unsigned short  FOREGROUND_COLORS[] = {White, Cyan, Red, Magenta, Yellow, Orange, GreenYellow};
const unsigned short  BACKGROUND_COLORS[] = {Black, Navy, DarkGreen, DarkCyan, Maroon};
 
PinName buspins[] = { PA_9, PC_7, PA_10, PB_3, PB_5, PB_4, PB_10, PA_8 };
Serial          pc(PA_2, PA_3);     // serial interface with PC
ILI9341        tft(BUS_8, buspins, PB_0, PC_1, PA_4, PA_1, PA_0, "tft");                // ILI9341 driver
Timer           t;
unsigned short  backgroundColor;
unsigned short  foregroundColor;
unsigned short  colorIndex = 0;
char            orient = 3;
// DigitalOut      backlight(PA_10);   // Display backlight 
 
int main()
{
    // backlight = 1;
    tft.set_orientation(orient);
    int time, time2;
    pc.baud (115200);
    pc.printf("\n\nSystem Core Clock = %.3f MHZ\r\n",(float)SystemCoreClock/1000000);
    t.start();
 
    while(1) {
        foregroundColor = FOREGROUND_COLORS[colorIndex++ % 7];
        tft.foreground(foregroundColor);    // set chars to white
        backgroundColor = BACKGROUND_COLORS[colorIndex % 5];
        tft.background(backgroundColor);    // set background to black
        tft.set_orientation((orient++) % 4);
        tft.cls();                     // clear the screen
        tft.locate(0,30);
        tft.printf("Display ID: %.8X\r\n", tft.tftID);
        pc.printf("Display ID: %.8X\r\n", tft.tftID);
        // mem write/read test
        unsigned short readback;
        unsigned short colorstep = (0x10000/tft.width());
        for(unsigned short i=0; i<tft.width(); i++) {
            tft.pixel(i,0,i*colorstep); // write line
        }
        bool readerror=false;
        for(unsigned short i=0; i<tft.width(); i++) { // verify line
            readback = tft.pixelread(i,0);
            if(readback!=i*colorstep) {
                readerror=true;
                pc.printf("pix %.4X readback %.4X\r\n", i*colorstep, readback);
            }
        }
        tft.locate(0,10);
        tft.printf("pixelread test %s\r\n", readerror ? "FAIL":"PASS");
        wait(2);
 
        tft.cls();
        tft.set_font((unsigned char*) Terminal6x8,32,127,false); //variable width disabled
        tft.locate(0,0);
        tft.printf("Display Test\r\nSome text just to see if auto carriage return works correctly");
        tft.set_font((unsigned char*) Terminal6x8);
        tft.printf("\r\nDisplay Test\r\nSome text just to see if auto carriage return works correctly");
        pc.printf("  Display Test \r\n");
        wait(3);
        t.reset();
        tft.cls();
        time=t.read_us();
        tft.locate(2,55);
        tft.printf("cls: %.3fms", (float)time/1000);
        pc.printf("cls: %.3fms\r\n", (float)time/1000);
        wait(3);
 
        tft.cls();
        t.reset();
        // draw some graphics
        tft.set_font((unsigned char*) Arial24x23);
        tft.locate(10,10);
        tft.printf("Test");
 
        tft.line(0,0,tft.width()-1,0,foregroundColor);
        tft.line(0,0,0,tft.height()-1,foregroundColor);
        tft.line(0,0,tft.width()-1,tft.height()-1,foregroundColor);
 
        tft.rect(10,30,50,40,foregroundColor);
        tft.fillrect(60,30,100,40,foregroundColor);
 
        tft.circle(150,32,30,foregroundColor);
        tft.fillcircle(140,20,10,foregroundColor);
 
        double s;
 
        for (unsigned short i=0; i<tft.width(); i++) {
            s =10 * sin((long double) i / 10 );
            tft.pixel(i,40 + (int)s ,foregroundColor);
        }
 
 
        time=t.read_us();
        tft.locate(2,55);
        tft.set_font((unsigned char*) Terminal6x8);
        tft.printf("plot: %.3fms", (float)time/1000);
        pc.printf("plot: %.3fms\r\n", (float)time/1000);
        wait(3);
        tft.cls();
        t.reset();
        Bitmap_s pic = {
            64,     // XSize
            64,     // YSize
            8,      // Bytes in Line
            burp,   // Pointer to picture data
        };
        tft.Bitmap_BW(pic,tft.width()-64,0);
        time=t.read_us();
        tft.locate(2,55);
        tft.printf("bmp: %.3fms", (float)time/1000);
        pc.printf("bmp: %.3fms\r\n", (float)time/1000);
        wait(3);
        tft.cls();
        tft.set_font((unsigned char*) Arial43x48_numb, 46, 58, false); //only numbers, variable-width disabled
        t.reset();
        tft.locate(0,0);
        tft.printf("%d", 12345);
        time=t.read_us();
        tft.locate(2,55);
        tft.set_font((unsigned char*) Terminal6x8);
        tft.printf("Big Font: %.3fms", (float)time/1000);
        pc.printf("Big Font: %.3fms\r\n", (float)time/1000);
        wait(3);
        // sparse pixels test
        tft.cls();
        tft.FastWindow(true);
        t.reset();
        for(unsigned int i=0; i<20000; i++) {
            tft.pixel((i+(i*89)%tft.width()), (i+(i*61)%tft.height()), White);
        }
        tft.copy_to_lcd();
        time=t.read_us();
        tft.cls();
        tft.FastWindow(false);
        t.reset();
        for(unsigned int i=0; i<20000; i++) {
            tft.pixel((i+(i*89)%tft.width()), (i+(i*61)%tft.height()), White);
        }
        tft.copy_to_lcd();
        time2=t.read_us();
        tft.locate(2,55);
        tft.printf("std:%.3fms fastw:%.3fms", (float)time2/1000, (float)time/1000);
        pc.printf("std: %.3fms fastw: %.3fms\r\n", (float)time2/1000, (float)time/1000);
        wait(3);
        // scroll test, only for TFT
        tft.cls();
        tft.set_font((unsigned char*) Arial24x23);
        tft.locate(2,10);
        tft.printf("Scrolling");
        tft.rect(0,0,tft.width()-1,tft.height()-1,White);
        tft.rect(1,1,tft.width()-2,tft.height()-2,Blue);
        tft.setscrollarea(0,tft.sizeY());
        wait(1);
        tft.scroll(1); //up 1
        wait(1);
        tft.scroll(0); //center
        wait(1);
        tft.scroll(tft.sizeY()-1); //down 1
        wait(1);
        tft.scroll(tft.sizeY()); // same as 0, center
        wait(1);
        tft.scroll(tft.sizeY()>>1); // half screen
        wait(1);
        tft.scrollreset(); // center
        wait(1);
        for(unsigned short i=1; i<=tft.sizeY(); i++) {
            tft.scroll(i);
            wait_ms(2);
        }
        wait(2);
        // color inversion
        for(unsigned short i=0; i<=8; i++) {
            tft.invert(i&1);
            wait_ms(200);
        }
        wait(2);
        // bmp 16bit test
        tft.cls();
        t.reset();
        for(int y=0; y<tft.height(); y+=34) {
            for(int x=0; x<tft.width(); x+=48) tft.Bitmap(x,y,48,34,(unsigned char *)pavement_48x34);
        }
        time=t.read_us();
        tft.locate(2,55);
        tft.set_font((unsigned char*) Terminal6x8);
        tft.printf("Bmp speed: %.3fms", (float)time/1000);
        pc.printf("Bmp speed: %.3fms\r\n", (float)time/1000);
        wait(2);
    }
}

