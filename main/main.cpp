#include <cstring>
#include <ctime>
#include <driver/gpio.h>
#include <driver/i2c.h>
#include <esp_err.h>
#include <esp_log.h>
#include <freertos/task.h>
#include <string>
extern "C" {
#include "oled.h"
}

std::string text;
void show_song(std::string song_name, std::string artist, bool clear) {
  const std::string indent = "                     ";
  if (clear) {
    xTaskCreate(&task_ssd1306_display_clear, "ssd1306_display_clear", 2048,
                nullptr, 6, nullptr);
  }
  vTaskDelay(100 / portTICK_PERIOD_MS);
  text = "\n\n" + indent.substr(0, (19 - song_name.length()) / 2) + song_name +
         "\n\n" + indent.substr(0, (19 - artist.length()) / 2) + artist;
  xTaskCreate(&task_ssd1306_display_text, "ssd1306_display_text", 2048,
              (void *)text.c_str(), 6, nullptr);
  // xTaskCreate(&task_ssd1306_contrast, "ssid1306_contrast", 2048, nullptr, 6,
  // nullptr);
  xTaskCreate(&task_ssd1306_scroll, "ssid1306_scroll", 2048, nullptr, 6,
              nullptr);
}
extern "C" void app_main(void) {
  i2c_master_init();
  ssd1306_init();

  // int secs_left = 432;
  std::string song_name = "K626 Requiem";
  std::string artist = "Mozart";
  show_song(song_name, artist, true);
  // time_t now;
  // struct tm timeinfo;
  // time(&now);
  // localtime_r(&now, &timeinfo);
  // int last_second = timeinfo.tm_sec;
  // while (1) {
  //   time(&now);
  //   localtime_r(&now, &timeinfo);
  //   if (last_second != timeinfo.tm_sec) {
  //     last_second = timeinfo.tm_sec;
  //     secs_left--;
  //     show_song(song_name, &secs_left, false);
  //   }
  // }
}
