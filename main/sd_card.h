#ifndef __SD_CARD_H
#define __SD_CARD_H

#define SD_PIN_NUM_MISO CONFIG_SD_CARD_MISO
#define SD_PIN_NUM_MOSI CONFIG_SD_CARD_MOSI
#define SD_PIN_NUM_CLK  CONFIG_SD_CARD_CLK
#define SD_PIN_NUM_CS   CONFIG_SD_CARD_CS

#if CONFIG_IDF_TARGET_ESP32S2
#define SPI_DMA_CHAN    host.slot
#elif CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32S3
#define SPI_DMA_CHAN    SPI_DMA_CH_AUTO
#else
#define SPI_DMA_CHAN    1
#endif

void sd_card_init();

#endif