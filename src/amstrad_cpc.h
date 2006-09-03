#ifndef __AMSTRAD_CPC_H__
#define __AMSTRAD_CPC_H__

#define AMSTRAD_CPC_464       0x01 
#define AMSTRAD_CPC_664       0x02 
#define AMSTRAD_CPC_6128      0x03 
#define AMSTRAD_CPC_464_PLUS  0x04 
#define AMSTRAD_CPC_6128_PLUS 0x05 

#define AMSTRAD_CPC_2MHZ      0x00
#define AMSTRAD_CPC_4MHZ      0x01
#define AMSTRAD_CPC_8MHZ      0x02
#define AMSTRAD_CPC_16MHZ     0x03

#define AMSTRAD_CPC_60HZ      0x00
#define AMSTRAD_CPC_50HZ      0x01

#define AMSTRAD_CPC_ISP       0x00
#define AMSTRAD_CPC_TRIUMPH   0x01
#define AMSTRAD_CPC_SAISHO    0x02
#define AMSTRAD_CPC_SOLAVOX   0x03
#define AMSTRAD_CPC_AWA       0x04
#define AMSTRAD_CPC_SCHNEIDER 0x05
#define AMSTRAD_CPC_ORION     0x06
#define AMSTRAD_CPC_AMSTRAD   0x07

#define AMSTRAD_CPC_CTM65     0x00
#define AMSTRAD_CPC_CTM644    0x01

#define AMSTRAD_CPC_ABSENT    0x00
#define AMSTRAD_CPC_PRESENT   0x01

typedef struct {
  struct {
    byte *lower_rom;
    byte *ram;
    byte *upper_rom[8];
    byte expansion;
  } memory;
  struct {
    byte row;
    byte line[16];
  } keyboard;
  struct {
    byte pen;
    byte ink[17];
    byte generate_interrupts;
    byte upper_ram_enable;
    byte lower_ram_enable;
    byte mode;
    byte ram_configuration;
    byte counter;
  } gate_array;
} AMSTRAD_CPC;

extern unsigned int amstrad_cpc_width;
extern unsigned int amstrad_cpc_height;
extern AMSTRAD_CPC amstrad_cpc;

void amstrad_cpc_parse(int argc, char **argv);
void amstrad_cpc_init(void);
void amstrad_cpc_reset(void);
void amstrad_cpc_exit(void);
void amstrad_cpc_run(void);
void amstrad_cpc_load_snapshot(char *filename);
void amstrad_cpc_save_snapshot(char *filename);
void amstrad_cpc_key_press(Widget widget, XtPointer data, XEvent *event);
void amstrad_cpc_key_release(Widget widget, XtPointer data, XEvent *event);
void amstrad_cpc_expose(Widget widget, XtPointer data, XEvent *event);

#endif
