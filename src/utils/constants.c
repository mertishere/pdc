const int BINARY_BASE = 2;
const int DEF_BASE_LENGTH = 3;

const int BIT_LENGTH = 1;
const int BYTE_LENGTH = 8;

const char INT_TO_ASCII_OFFSET = 48; // the difference to go from the string "0" to the int 0 in binary

const char *PNG_SIG = "89504e470d0a1a0a";
const char *PLTE_SIG = "504c5445";
const char *IHDR_SIG = "49484452";
const char *IDAT_SIG = "49444154";
const char *IEND_SIG = "49454e44";

const char *bKGD_SIG = "624b4744";
const char *cHRM_SIG = "6348524d";
const char *cICP_SIG = "63494350";
const char *dSIG_SIG = "64534947";
const char *eXIf_SIG = "65584966";
const char *gAMA_SIG = "67414d41";
const char *hIST_SIG = "68495354";
const char *iCCP_SIG = "69434350";
const char *iTXt_SIG = "69545874";
const char *pHYs_SIG = "70485973";
const char *sBIT_SIG = "73424954";
const char *sPLT_SIG = "73504c54";
const char *sRGB_SIG = "73524742";
const char *sTER_SIG = "73544552";
const char *tEXt_SIG = "74455874";
const char *tIME_SIG = "74494d45";
const char *tRNS_SIG = "74524e53";
const char *zTXt_SIG = "7a545874";

const char HEX_LETTERS[6] = { 'a', 'b', 'c', 'd', 'e', 'f' };
const char HEX_KEYS[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
};

const char *HEX_VALUES[16] = { "0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111", "1000", "1001", "1010", "1011", "1100", "1101", "1110", "1111" };

const int HCLEN_SIZE = 19;
const int HCLEN_ORDER[19] = { 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };

const int RGB_WHITE = 255;
const int RGB_BLACK = 0;


const float R_FLOAT_COEFFICIENTS = 0.299;
const float G_FLOAT_COEFFICIENTS = 0.587;
const float B_FLOAT_COEFFICIENTS = 0.114;

const int BASE_DISTANCES[30] = {
    1,     2,     3,    4,
    5,     7,     9,    13,
    17,    25,    33,   49,
    65,    97,    129,  193,
    257,   385,   513,  769,
    1025,  1537,  2049, 3073,
    4097,  6145,  8193, 12289,
    16385, 24577,
};

const int EXTRA_BITS[30] = {
    0,  0,  0,  0,
    1,  1,  2,  2,
    3,  3,  4,  4,
    5,  5,  6,  6,
    7,  7,  8,  8,
    9,  9,  10, 10,
    11, 11, 12, 12,
    13, 13,
};


const int HEX_PRINT_AMOUNT = 2;
const int HEX_SECTION_LENGTH = 8;
const int HEX_BYTE_AMOUNT = HEX_PRINT_AMOUNT * HEX_SECTION_LENGTH;