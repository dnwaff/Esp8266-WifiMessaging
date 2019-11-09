/*
#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "driver/ssd1306.h"
#include "driver/i2c.h"
#include <stdarg.h>

#define lcd_address 0x78

void off(void)
{
    _send_command(0xAE);
}

void on(void)
{
    _send_command(0xAF);
}

void sleep(void)
{
    _send_command(0xAE);
}

void wake(void)
{
    _send_command(0xAF);
}

void set_inverse(unsigned char value)
{
    _send_command(value ? 0xA7 : 0xA6);
}

void set_display_offset(unsigned char value)
{
    _send_command(0xD3);
    _send_command(value & 0x3F);
}

void set_contrast(unsigned char value)
{
    _send_command(0x81);
    _send_command(value);
}

void set_display_start_line(unsigned char value)
{
    _send_command(0x40 | value);
}

void set_segment_remap(unsigned char value)
{
    _send_command(value ? 0xA1 : 0xA0);
}

void set_multiplex_ratio(unsigned char value)
{
    _send_command(0xA8);
    _send_command(value & 0x3F);
}

void set_com_output_scan_direction(unsigned char value)
{
    _send_command(value ? 0xC8 : 0xC0);
}

void set_com_pins_hardware_configuration(unsigned char sequential, unsigned char lr_remap)
{
    _send_command(0xDA);
    _send_command(0x02 | ((sequential & 1) << 4) | ((lr_remap & 1) << 5));
}

void start_horizontal_scroll(unsigned char direction, unsigned char start, unsigned char end, unsigned char interval)
{
    _send_command(direction ? 0x27 : 0x26);
    _send_command(0x00);
    _send_command(start & 0x07);
    switch (interval) {
        case   2: _send_command(0x07); break; // 111b
        case   3: _send_command(0x04); break; // 100b
        case   4: _send_command(0x05); break; // 101b
        case   5: _send_command(0x00); break; // 000b
        case  25: _send_command(0x06); break; // 110b
        case  64: _send_command(0x01); break; // 001b
        case 128: _send_command(0x02); break; // 010b
        //case 256: _send_command(0x03); break; // 011b
        default:
            // default to 2 frame interval
            _send_command(0x07); break;
    }
    _send_command(end & 0x07);
    _send_command(0x00);
    _send_command(0xFF);

    // activate scroll
    _send_command(0x2F);
}

void start_vertical_and_horizontal_scroll(unsigned char direction, unsigned char start, unsigned char end, unsigned char interval, unsigned char vertical_offset)
{
    _send_command(direction ? 0x2A : 0x29);
    _send_command(0x00);
    _send_command(start & 0x07);
    switch (interval) {
        case   2: _send_command(0x07); break; // 111b
        case   3: _send_command(0x04); break; // 100b
        case   4: _send_command(0x05); break; // 101b
        case   5: _send_command(0x00); break; // 000b
        case  25: _send_command(0x06); break; // 110b
        case  64: _send_command(0x01); break; // 001b
        case 128: _send_command(0x02); break; // 010b
        //case 256: _send_command(0x03); break; // 011b
        default:
            // default to 2 frame interval
            _send_command(0x07); break;
    }
    _send_command(end & 0x07);
    _send_command(vertical_offset);

    // activate scroll
    _send_command(0x2F);
}

void stop_scroll()
{
    // all scroll configurations are removed from the display when executing this command.
    _send_command(0x2E);
}

void pam_set_start_address(unsigned char address)
{
    // "Set Lower Column Start Address for Page Addressing Mode"
    _send_command(address & 0x0F);

    // "Set Higher Column Start Address for Page Addressing Mode"
    _send_command((address << 4) & 0x0F);
}

void set_memory_addressing_mode(unsigned char mode)
{
    _send_command(0x20);
    _send_command(mode & 0x3);
}

void hv_set_column_address(unsigned char start, unsigned char end)
{
    _send_command(0x21);
    _send_command(start & 0x7F);
    _send_command(end & 0x7F);
}

void hv_set_page_address(unsigned char start, unsigned char end)
{
    _send_command(0x22);
    _send_command(start & 0x07);
    _send_command(end & 0x07);
}

void pam_set_page_start(unsigned char address)
{
    _send_command(0xB0 | (address & 0x07));
}

void set_display_clock_ratio_and_frequency(unsigned char ratio, unsigned char frequency)
{
    _send_command(0xD5);
    _send_command((ratio & 0x0F) | ((frequency & 0x0F) << 4));
}

void set_precharge_period(unsigned char phase1, unsigned char phase2)
{
    _send_command(0xD9);
    _send_command((phase1 & 0x0F) | ((phase2 & 0x0F ) << 4));
}

void set_vcomh_deselect_level(unsigned char level)
{
    _send_command(0xDB);
    _send_command((level & 0x03) << 4);
}

void nop()
{
    _send_command(0xE3);
}

void set_charge_pump_enable(unsigned char enable)
{
    _send_command(0x8D);
    _send_command(enable ? 0x14 : 0x10);
}

void initialise(void)
{
    // Init
    off();

    set_display_clock_ratio_and_frequency(0, 8);
    set_multiplex_ratio(0x3F); // 1/64 duty
    set_precharge_period(0xF, 0x01);
    set_display_offset(0);
    set_display_start_line(0);
    set_charge_pump_enable(1);
    set_memory_addressing_mode(0); // horizontal addressing mode; across then down
    set_segment_remap(1);
    set_com_output_scan_direction(1);
    set_com_pins_hardware_configuration(1, 0);
    set_contrast(0x81);
    set_vcomh_deselect_level(1);


    set_inverse(0);

    hv_set_column_address(0, 127);
    hv_set_page_address(0, 7);

    pam_set_start_address(0);
    pam_set_page_start(0);

    _send_command(0xA4); // displayallon resum
    _send_command(0xA6); // normal display
    wake();
    // set_precharge_period(2, 2);
}

void update(void)
{
    hv_set_column_address(0, 127);
    hv_set_page_address(0, 7);
    int i;
    for (i = 0; i < 1024; i++)
        _send_data(_screen[i]);
}

void set_pixel(int x, int y)
{
    if (x >= SSD1306_LCDWIDTH || y >= SSD1306_LCDHEIGHT) return;

    _screen[x + (y / 8) * 128] |= 1 << (y % 8);
}

void clear_pixel(int x, int y)
{
    if (x >= SSD1306_LCDWIDTH || y >= SSD1306_LCDHEIGHT) return;

    _screen[x + (y / 8) * 128] &= ~(1 << (y % 8));
}

void line(int x0, int y0, int x1, int y1) {
  int steep = abs(y1 - y0) > abs(x1 - x0);
  int t;

  if (steep) {
    t = x0; x0 = y0; y0 = t;
    t = x1; x1 = y1; y1 = t;
  }

  if (x0 > x1) {
    t = x0; x0 = x1; x1 = t;
    t = y0; y0 = y1; y1 = t;
  }

  int dx, dy;

  dx = x1 - x0;
  dy = abs(y1 - y0);

  int err = dx / 2;
  int ystep;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;}

  for (; x0<x1; x0++) {
    if (steep) {
      set_pixel(y0, x0);
    } else {
      set_pixel(x0, y0);
    }
    err -= dy;
    if (err < 0) {
      y0 += ystep;
      err += dx;
    }
  }
}

void set_font(unsigned char *font, unsigned int width)
{
    _console_font_data = font;
    _console_font_width = width;
}

void set_double_height_text(unsigned int double_height)
{
    _double_height_text = double_height;
}

void ssdPutc(unsigned char c)
{   int b ;
    while (_cursor_x >= (128 / _console_font_width))
    {
        _cursor_x -= (128 / _console_font_width);
        _cursor_y++;
    }

    while (_cursor_y > 7)
    {
        scroll_up();
    }

    switch (c)
    {
    case '\n':
        _cursor_y++;
        break;

    case '\r':
        _cursor_x = 0;
        break;

    case '\t':
        _cursor_x = (_cursor_x + 4) % 4;
        break;

    default:

        for (b=0; b < _console_font_width; b++)
        {
            _screen[_cursor_x * _console_font_width + _cursor_y * 128 + b] = _console_font_data[(c - FONT_START) * _console_font_width + b];
        }

        _cursor_x++;
    }
}

void scroll_up(void)
{
	int y;
    for (y=1; y <= 7; y++)
    {
    	int x;
        for (x =0; x < 128; x++)
        {
            _screen[x + 128 * (y - 1)] = _screen[x + 128 * y];
        }
    }
    int x;
    for (x = 0; x < 128; x++)
    {
        _screen[x + 128 * 7] = 0;
    }

    _cursor_y--;
}

void ssdPrintf(const char *format, ...)
{
    static char buffer[128];

    //va_list args;
    //va_start(args, format);
    //vsprintf(buffer, format, args);
    //va_end(args);

    char *c = (char *)&buffer;
    while (*c != 0)
    {
        ssdPutc(*c++);
    }
}

void clear(void)
{	int i;
    for (i = 0; i < 1024; i++)
        _screen[i] = 0;

    _cursor_x = 0;
    _cursor_y = 0;
}

void _send_command(unsigned char code)
{
    i2c_start();
    i2c_writeByte(lcd_address);
    i2c_writeByte(0x00); // This byte is COMMAND
    i2c_writeByte(code);
    i2c_stop();
}

void _send_data(unsigned char value)
{
    i2c_start();
    i2c_writeByte(lcd_address);
    i2c_writeByte(0x40); // This byte is DATA
    i2c_writeByte(value);
    i2c_stop();
}
*/
