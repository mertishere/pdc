# Ancillary chunks

(https://www.libpng.org/pub/png/spec/1.2/PNG-Chunks.html) \
Ancillary chunks not required to appear in this order.


|Name  | Multiple |  Ordering constraints (IDAT & PLTE)         |
|:-----|:--------:|--------------------------------------------:|
| cHRM |  **No**  | _Before_ PLTE **and** IDAT                  |
| gAMA |  **No**  | _Before_ PLTE **and** IDAT                  |
| iCCP |  **No**  | _Before_ PLTE **and** IDAT                  |
| sBIT |  **No**  | _Before_ PLTE **and** IDAT                  |
| sRGB |  **No**  | _Before_ PLTE **and** IDAT                  |
| bKGD |  **No**  | _After_ PLTE; _before_ IDAT                 |
| hIST |  **No**  | _After_ PLTE; _before_ IDAT                 |
| tRNS |  **No**  | _After_ PLTE; _before_ IDAT                 |
| pHYs |  **No**  | _Before_ IDAT                               |
| sPLT |  **Yes** | _Before_ IDAT                               |
| tIME |  **No**  | None                                        |
| iTXt |  **Yes** | None                                        |
| tEXt |  **Yes** | None                                        |
| zTXt |  **Yes** | None                                        |
