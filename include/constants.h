#ifndef CONSTANTS_H
#define CONSTANTS_H

extern int BINARY_BASE;
extern int DEF_BASE_LENGTH;

extern int BIT_LENGTH;
extern int BYTE_LENGTH;

extern char INT_TO_ASCII_OFFSET;

extern char *PNG_SIG;
extern char *PLTE_SIG;
extern char *IHDR_SIG;
extern char *IDAT_SIG;
extern char *IEND_SIG;

extern char *bKGD_SIG;
extern char *cHRM_SIG;
extern char *cICP_SIG;
extern char *dSIG_SIG;
extern char *eXIf_SIG;
extern char *gAMA_SIG;
extern char *hIST_SIG;
extern char *iCCP_SIG;
extern char *iTXt_SIG;
extern char *pHYs_SIG;
extern char *sBIT_SIG;
extern char *sPLT_SIG;
extern char *sRGB_SIG;
extern char *sTER_SIG;
extern char *tEXt_SIG;
extern char *tIME_SIG;
extern char *tRNS_SIG;
extern char *zTXt_SIG;

extern char HEX_LETTERS[16];
extern char HEX_KEYS[16];

extern char *HEX_VALUES[16];

extern int HCLEN_SIZE;
extern int HCLEN_ORDER[19];

extern int RGB_WHITE;
extern int RGB_BLACK;


extern float R_FLOAT_COEFFICIENTS;
extern float G_FLOAT_COEFFICIENTS;
extern float B_FLOAT_COEFFICIENTS;

extern int BASE_DISTANCES[30];
extern int EXTRA_BITS[30];

extern int HEX_PRINT_AMOUNT;
extern int HEX_SECTION_LENGTH;
extern int HEX_BYTE_AMOUNT;

#endif