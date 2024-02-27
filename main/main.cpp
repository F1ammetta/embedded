#include <cstring>
#include <ctime>
#include <driver/dac_cosine.h>
#include <driver/gpio.h>
#include <driver/i2c.h>
#include <esp_attr.h>
#include <esp_err.h>
#include <esp_log.h>
#include <freertos/task.h>
#include <string>
extern "C" {
#include "oled.h"
}

std::string text;

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

  dac_cosine_handle_t dac_cosine;

  dac_cosine_config_t cos0_cfg = {
      .chan_id = DAC_CHAN_0,
      .freq_hz = 880, // It will be covered by 8000 in the latter configuration
      .clk_src = DAC_COSINE_CLK_SRC_DEFAULT,
      .atten = DAC_COSINE_ATTEN_DEFAULT,
      .phase = DAC_COSINE_PHASE_0,
      .offset = 0,
  };

  ESP_ERROR_CHECK(dac_cosine_new_channel(&cos0_cfg, &dac_cosine));
  ESP_ERROR_CHECK(dac_cosine_start(dac_cosine));

  std::string song_name = "K626 Requiem";
  std::string artist = "Mozart";
  show_song(song_name, artist, true);
}
