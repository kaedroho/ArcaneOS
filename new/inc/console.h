#ifndef CONSOLE_H
#define	CONSOLE_H

void console_putc(char c);
void console_putc_protected(char c);
void console_puts(char* s);
void console_puts_protected(char* s);
void console_putm(char* string,unsigned short length);
void console_putm_protected(char* string,unsigned short length);
void console_putu32(unsigned int i, unsigned int base);
void console_putu32_protected(unsigned int i, unsigned int base);
void console_puti32(int i, unsigned int base);
void console_puti32_protected(int i, unsigned int base);
void console_scrollup(unsigned short spaces);

#endif	/* CONSOLE_H */

