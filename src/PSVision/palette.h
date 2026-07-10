#ifndef PALETTE_H
#define PALETTE_H

#define PAL_FILELIST_BACK 0
#define PAL_FILELIST_BORDER 1
#define PAL_FILELIST_COUNTER 2
#define PAL_FILELIST_SCROLL 3
#define PAL_FILELIST_SCROLL_IND 4
#define PAL_FILELIST_HEADER 5

uint32_t palette[] = {
    TFT_DARKGREY,  // PAL_FILELIST_BACK - фон списка
    TFT_WHITE,     // PAL_FILELIST_BORDER - рамка
    TFT_WHITE,     // PAL_FILELIST_COUNTER - счётчик файлов
    TFT_GOLD,      // PAL_FILELIST_SCROLL - полоса прокрутки
    TFT_BLACK,     // PAL_FILELIST_SCROLL_IND - индикатор на полосе прокрутки
    TFT_BLACK      // PAL_FILELIST_HEADER - заголовок
};

#endif
