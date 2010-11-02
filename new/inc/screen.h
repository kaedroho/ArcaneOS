#ifndef SCREEN_H
#define	SCREEN_H

void screen_putc(unsigned short x,unsigned short y,unsigned short c);
unsigned short screen_getc(unsigned short x,unsigned short y);
void screen_positioncursor(unsigned short x,unsigned short y);
void screen_putpixel(unsigned short x,unsigned short y,unsigned long p);

unsigned short screen_getwidth();
unsigned short screen_getheight();
unsigned long screen_getframebuffer();



struct vbe_modeinfo
{
   unsigned short ModeAttributes;
   unsigned char  WinAAttributes;
   unsigned char  WinBAttributes;
   unsigned short WinGranularity;
   unsigned short WinSize;
   unsigned short WinASegment;
   unsigned short WinBSegment;
   void  *WinFuncPtr;
   unsigned short BytesPerScanLine;
   unsigned short XRes;
   unsigned short YRes;
   unsigned char  Xu8intSize;
   unsigned char  Yu8intSize;
   unsigned char  NumberOfPlanes;
   unsigned char  BitsPerPixel;
   unsigned char  NumberOfBanks;
   unsigned char  MemoryModel;
   unsigned char  BankSize;
   unsigned char  NumberOfImagePages;
   unsigned char  res1;
   unsigned char  RedMaskSize;
   unsigned char  RedFieldPosition;
   unsigned char  GreenMaskSize;
   unsigned char  GreenFieldPosition;
   unsigned char  BlueMaskSize;
   unsigned char  BlueFieldPosition;
   unsigned char  RsvedMaskSize;
   unsigned char  RsvedFieldPosition;
   //VBE 2.0
   unsigned int *PhysBasePtr;
   unsigned int OffScreenMemOffset;
   unsigned short OffScreenMemSize;
   //VBE 2.1
   unsigned short LinbytesPerScanLine;
   unsigned char  BankNumberOfImagePages;
   unsigned char  LinNumberOfImagePages;
   unsigned char  LinRedMaskSize;
   unsigned char  LinRedFieldPosition;
   unsigned char  LingreenMaskSize;
   unsigned char  LinGreenFieldPosition;
   unsigned char  LinBlueMaskSize;
   unsigned char  LinBlueFieldPosition;
   unsigned char  LinRsvdMaskSize;
   unsigned char  LinRsvdFieldPosition;
   unsigned char  res2[194];
};



#endif	/* SCREEN_H */
