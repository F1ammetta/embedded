extern void i2c_master_init();

extern void ssd1306_init();

extern void task_ssd1306_display_clear(void *ignore);

extern void task_ssd1306_contrast(void *ignore);

extern void task_ssd1306_scroll(void *ignore);

extern void task_ssd1306_display_text(void *arg_text);
