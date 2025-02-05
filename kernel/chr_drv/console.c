#include <linux/tty.h>
#include <asm/io.h>
#include <asm/system.h>

#define ORIG_X          (*(unsigned char *)0x90000)
#define ORIG_Y          (*(unsigned char *)0x90001)

#define ORIG_VIDEO_PAGE     (*(unsigned short *)0x90004)
#define ORIG_VIDEO_MODE     ((*(unsigned short *)0x90006) & 0xff)
#define ORIG_VIDEO_COLS     (((*(unsigned short *)0x90006) & 0xff00) >> 8)
#define ORIG_VIDEO_LINES    ((*(unsigned short *)0x9000e) & 0xff)
#define ORIG_VIDEO_EGA_AX   (*(unsigned short *)0x90008)
#define ORIG_VIDEO_EGA_BX   (*(unsigned short *)0x9000a)
#define ORIG_VIDEO_EGA_CX   (*(unsigned short *)0x9000c)

#define VIDEO_TYPE_MDA      0x10    /* Monochrome Text Display  */
#define VIDEO_TYPE_CGA      0x11    /* CGA Display          */
#define VIDEO_TYPE_EGAM     0x20    /* EGA/VGA in Monochrome Mode   */
#define VIDEO_TYPE_EGAC     0x21    /* EGA/VGA in Color Mode    */

static unsigned char    video_type;     /* Type of display being used   */
static unsigned long    video_num_columns;  /* Number of text columns   */
static unsigned long    video_num_lines;    /* Number of test lines     */
static unsigned long    video_mem_base;     /* Base of video memory     */
static unsigned long    video_mem_term;     /* End of video memory      */
static unsigned long    video_size_row;     /* Bytes per row        */
static unsigned char    video_page;     /* Initial video page       */
static unsigned short   video_port_reg;     /* Video register select port   */
static unsigned short   video_port_val;     /* Video register value port    */
static unsigned short   video_erase_char;

static unsigned long    origin;
static unsigned long    scr_end;
static unsigned long    pos;
static unsigned long    x, y;
static unsigned long    top, bottom;
static unsigned long    attr = 0x07;

static inline void gotoxy(int new_x,unsigned int new_y)
{
	if (new_x > video_num_columns || new_y >= video_num_lines)
		return;

	x = new_x;
	y = new_y;
	pos = origin + y*video_size_row + (x << 1);
}

static inline void set_origin(void)
{
	cli();
	outb_p(12, video_port_reg);
	outb_p(0xff & ((origin - video_mem_base) >> 9), video_port_val);
	outb_p(13, video_port_reg);
	outb_p(0xff & ((origin - video_mem_base) >> 1), video_port_val);
	sti();
}

static inline void set_cursor()
{
	cli();
	outb_p(14, video_port_reg);
	outb_p(0xff&((pos-video_mem_base)>>9), video_port_val);
	outb_p(15, video_port_reg);
	outb_p(0xff&((pos-video_mem_base)>>1), video_port_val);
	sti();
}

static void scrup(void)
{
	if (!top && bottom == video_num_lines) {
		origin += video_size_row;
		pos += video_size_row;
		scr_end += video_size_row;

		if (scr_end > video_mem_term) {
			__asm__("cld\n\t"
				"rep\n\t"
				"movsl\n\t"
				"movl video_num_columns,%1\n\t"
				"rep\n\t"
				"stosw"
				::"a" (video_erase_char),
				"c" ((video_num_lines-1)*video_num_columns>>1),
				"D" (video_mem_base),
				"S" (origin):);
			scr_end -= origin-video_mem_base;
			pos -= origin-video_mem_base;
			origin = video_mem_base;
		} else {
			__asm__("cld\n\t"
				"rep\n\t"
				"stosw"
				::"a" (video_erase_char),
				"c" (video_num_columns),
				"D" (scr_end-video_size_row):);
		}
		set_origin();
	} else {
		__asm__("cld\n\t"
			"rep\n\t"
			"movsl\n\t"
			"movl video_num_columns,%%ecx\n\t"
			"rep\n\t"
			"stosw"
			::"a" (video_erase_char),
			"c" ((bottom-top-1)*video_num_columns>>1),
			"D" (origin+video_size_row*top),
			"S" (origin+video_size_row*(top+1)):);
	}
}

static void lf(void)
{
	if (y + 1 < bottom) {
		y++;
		pos += video_size_row;
		return;
	}
	scrup();
}

static void cr(void)
{
	pos -= x << 1;
	x = 0;
}

static void del(void)
{
	if (x) {
		pos -= 2;
		x--;
		*(unsigned short*)pos = video_erase_char;
	}
}

void con_init(void)
{
	char * display_desc = "????";
	char * display_ptr;

	video_num_columns = ORIG_VIDEO_COLS;
	video_size_row = video_num_columns * 2;
	video_num_lines = ORIG_VIDEO_LINES;
	video_page = ORIG_VIDEO_PAGE;
	video_erase_char = 0x0720;

	/* Is this a monochrome display? */
	if (ORIG_VIDEO_MODE == 7) {
		video_mem_base = 0xb0000;
		video_port_reg = 0x3b4;
		video_port_val = 0x3b5;
		if ((ORIG_VIDEO_EGA_BX & 0xff) != 0x10) {
			video_type = VIDEO_TYPE_EGAM;
			video_mem_term = 0xb8000;
			display_desc = "EGAm";
		} else {
			video_type = VIDEO_TYPE_MDA;
			video_mem_term = 0xb2000;
			display_desc = "*MDA";
		}
	} else { /* color display */
		video_mem_base = 0xb8000;
		video_port_reg  = 0x3d4;
		video_port_val  = 0x3d5;

		if ((ORIG_VIDEO_EGA_BX & 0xff) != 0x10) {
			video_type = VIDEO_TYPE_EGAC;
			video_mem_term = 0xc0000;
			display_desc = "EGAc";
		}
		else {
			video_type = VIDEO_TYPE_CGA;
			video_mem_term = 0xba000;
			display_desc = "*CGA";
		}
	}

	display_ptr = ((char *)video_mem_base) + video_size_row - 8;
	while (*display_desc) {
		*display_ptr++ = *display_desc++;
		display_ptr++;
	}

	origin = video_mem_base;
	scr_end = video_mem_base + video_num_lines * video_size_row;
	top = 0;
	bottom  = video_num_lines;

	gotoxy(ORIG_X, ORIG_Y);
	set_cursor();
}

void console_print(const char *buf, int nr)
{
	const char* s = buf;

	while (nr--) {
		char c = *s++;
		if (c > 31 && c < 127) {
			if (x >= video_num_columns) {
				x -= video_num_columns;
				pos -= video_size_row;
				lf();
			}

			*(char *)pos = c;
			*(((char*)pos) + 1) = attr;
			pos += 2;
			x++;
		} else if (c == 10 || c == 11 || c == 12)
			lf();
		else if (c == 13)
			cr();
		else if (c == 127) {
			del();
		} else if (c == 8) {
			if (x) {
				x--;
				pos -= 2;
			}
		}
	}

	gotoxy(x, y);
	set_cursor();
}
