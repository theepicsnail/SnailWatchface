#include "pebble.h"
#define WIDTH 144
#define HEIGHT 168

Window *window;
Layer *window_layer;

// --- shortdate ---
static char shortdate_text[] = "00/00/00";
TextLayer *shortdate_layer;

static void update_shortdate(struct tm *tick_time, TimeUnits units_changed) {
  strftime(shortdate_text, sizeof(shortdate_text), "%D", tick_time);
  text_layer_set_text(shortdate_layer, shortdate_text);
}

void shortdate_create() {
  shortdate_layer = text_layer_create(GRect(0, 74+25, WIDTH, 32));
  text_layer_set_text_color(shortdate_layer, GColorWhite);
  text_layer_set_text_alignment(shortdate_layer, GTextAlignmentCenter);
  text_layer_set_background_color(shortdate_layer, GColorBlack);
  text_layer_set_font(shortdate_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  layer_add_child(window_layer, text_layer_get_layer(shortdate_layer));
}
// --- end shortdate---

// --- date ---
static char date_text[] = "Tue, Sep 30";
TextLayer *date_layer;

static void update_date(struct tm *tick_time, TimeUnits units_changed) {
  strftime(date_text, sizeof(date_text), "%a, %b %d", tick_time);
  text_layer_set_text(date_layer, date_text);
}

void date_create() {
  date_layer = text_layer_create(GRect(0, 42+25, WIDTH, 32));
  text_layer_set_text_color(date_layer, GColorWhite);
  text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
  text_layer_set_background_color(date_layer, GColorBlack);
  text_layer_set_font(date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  layer_add_child(window_layer, text_layer_get_layer(date_layer));
}
// --- end date ---

// --- bluetooth ---
void bluetooth_handler(bool connected) {
  if (!connected)
    vibes_double_pulse();
}
void bluetooth_create() {
  bluetooth_connection_service_subscribe(bluetooth_handler);
}
// --- end bluetooth ---

// --- time ---
static char time_text[] = "23:59";
TextLayer *time_layer;

static void update_time(struct tm *tick_time, TimeUnits units_changed) {
  strftime(time_text, sizeof(time_text), "%R", tick_time);
  text_layer_set_text(time_layer, time_text);

}

void time_create() {
  time_layer = text_layer_create(GRect(0,-13+26,WIDTH, 51));
  text_layer_set_text_color(time_layer, GColorWhite);
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  text_layer_set_background_color(time_layer, GColorBlack);
  text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
  layer_add_child(window_layer, text_layer_get_layer(time_layer));
}
// --- end time ---


void handle_tick(struct tm *tick_time, TimeUnits units_changed) {

  if (units_changed & DAY_UNIT) {
    update_date(tick_time, units_changed);
    update_shortdate(tick_time, units_changed);
  }
  if (units_changed & MINUTE_UNIT)
    update_time(tick_time, units_changed);
}

void handle_deinit(void) {
  tick_timer_service_unsubscribe();
}

void handle_init(void) {
  window = window_create();
  window_stack_push(window, true /* Animated */);
  window_set_background_color(window, GColorBlack);
  window_layer = window_get_root_layer(window);

  time_create();
  date_create();
  shortdate_create();
  bluetooth_create();

  tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
  time_t now = time(NULL);
  handle_tick(localtime(&now), MINUTE_UNIT | DAY_UNIT);
}

int main(void) {
  handle_init();

  app_event_loop();

  handle_deinit();
}
