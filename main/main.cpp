#include <cstring>
#include <ctime>
#include <driver/gpio.h>
#include <driver/i2c.h>
#include <esp_attr.h>
#include <esp_err.h>
#include <esp_log.h>
#include <freertos/task.h>
#include <string>
#include <cmath>
extern "C" {
#include "oled.h"
#include "tm1637.h"
}

std::string text;

const gpio_num_t LED_CLK = GPIO_NUM_16;
const gpio_num_t LED_DTA = GPIO_NUM_17;

int g_song_secs = 0;
bool show_dot = false;

void tm1637_task(void * arg) {
  tm1637_led_t * lcd = tm1637_init(LED_CLK, LED_DTA);

	setenv("TZ", "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00", 1);
	tzset();

	while (g_song_secs > 0) {
    int mins = g_song_secs / 60;
    int secs = g_song_secs % 60;
    int data[] = {mins / 10, mins % 10, secs / 10, secs % 10};
    for (int i = 0; i < 4; i++) {
      tm1637_set_segment_number(lcd, i, data[i], show_dot);
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void count_down(void * arg) {
  time_t now;
  struct tm timeinfo;
  int last_secs = 0;
  
  while (g_song_secs > 0) {
    time(&now);
    localtime_r(&now, &timeinfo);
    int secs = timeinfo.tm_sec;
    if (secs != last_secs) {
      show_dot = !show_dot;
      last_secs = secs;
      g_song_secs--;
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void show_song(std::string song_name, std::string artist, bool clear) {
  const std::string indent = "    ";

  if (clear) {
    xTaskCreate(&task_ssd1306_display_clear, "ssd1306_display_clear", 2048,
                nullptr, 6, nullptr);
  }

  vTaskDelay(100 / portTICK_PERIOD_MS);

  text = "\n\n\n" + indent.substr(0, (15 - song_name.length()) / 2) +
         song_name + "\n\n" + indent.substr(0, (15 - artist.length()) / 2) +
         artist;

  xTaskCreate(&task_ssd1306_display_text, "ssd1306_display_text", 2048,
              (void *)text.c_str(), 6, nullptr);

  xTaskCreate(&task_ssd1306_scroll, "ssid1306_scroll", 2048, nullptr, 6,
              nullptr);
}

extern "C" void app_main(void) {
  i2c_master_init();
  ssd1306_init();
  


  // dac_cosine_handle_t dac_cosine;

  // dac_cosine_config_t cos0_cfg = {
  //     .chan_id = DAC_CHAN_0,
  //     .freq_hz = 880, // It will be covered by 8000 in the latter configuration
  //     .clk_src = DAC_COSINE_CLK_SRC_DEFAULT,
  //     .atten = DAC_COSINE_ATTEN_DEFAULT,
  //     .phase = DAC_COSINE_PHASE_0,
  //     .offset = 0,
  // };

  // ESP_ERROR_CHECK(dac_cosine_new_channel(&cos0_cfg, &dac_cosine));
  // ESP_ERROR_CHECK(dac_cosine_start(dac_cosine));

  std::string song_name = "K626 Requiem";
  std::string artist = "Mozart";
  show_song(song_name, artist, true);
  
  g_song_secs = 193;
  
  xTaskCreate(&tm1637_task, "tm1637_task", 2048, nullptr, 6, nullptr);
  
  xTaskCreate(&count_down, "count_down", 2048, nullptr, 6, nullptr);
  
  
}
