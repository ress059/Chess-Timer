/*********************
 *      INCLUDES
 *********************/
#include "chess_timer_ui.h"
#include "lvgl/lvgl.h"


/*********************
 *      DEFINES
 *********************/
#define BG_TOP_HEIGHT 50
#define BG_START_TIME 400
#define SLIDER_SCREEN_TIME 400
#define START_ANIM_TIME 500
#define BG_TOP_ANIM_TIME 400
#define ANIM_VERT_TIME 800 //ms


/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_obj_t * create_btn(lv_obj_t * parent, char * label, bool red_label, lv_event_cb_t event_cb);
static void timer_btn_pressed_cb(lv_obj_t *obj, lv_event_t event);
static void anim_bg_top(uint32_t time);
static void create_slider_scr();

static void slider_event_cb(lv_obj_t * sliderobj, lv_event_t event);
static void apply_btn_cb(lv_obj_t *obj, lv_event_t event);
static void back_btn_cb(lv_obj_t *obj, lv_event_t event);

/**********************
 *  STATIC VARIABLES
 **********************/
static uint8_t bat_remaining = 100; //TODO
static uint16_t slider_minutes = 15;

static lv_obj_t * homeimg;
static lv_obj_t * bg_top;
static lv_obj_t * time_btn;
static lv_obj_t * apply_btn;
static lv_obj_t * back_btn;
static lv_obj_t * slider_scr_cont;

static lv_style_t default_btn_style;
static lv_style_t red_btn_style;



static lv_obj_t * batcontainer;
static lv_obj_t * slider_label;



/**********************
 *  STYLES
 **********************/
static lv_style_t slider_cont_style;
static lv_style_t slider_style;


void chess_timer_ui(void){
    //initialize styles
    lv_style_init(&red_btn_style);
    lv_style_set_text_color(&red_btn_style, LV_STATE_DEFAULT, LV_COLOR_RED);

    lv_style_init(&default_btn_style);
    lv_style_set_bg_color(&default_btn_style, LV_STATE_DEFAULT, lv_color_hex(0x1D1D1D));
    lv_style_set_bg_opa(&default_btn_style, LV_STATE_DEFAULT, LV_OPA_COVER);
    lv_style_set_border_width(&default_btn_style, LV_STATE_DEFAULT, 3);
    lv_style_set_border_color(&default_btn_style, LV_STATE_DEFAULT, lv_color_hex(0xBEBEBE));
    lv_style_set_text_font(&default_btn_style, LV_STATE_DEFAULT, &lv_font_montserrat_24);
    lv_style_set_text_color(&default_btn_style, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_radius(&default_btn_style, LV_STATE_DEFAULT, 30);

    lv_style_init(&slider_cont_style);
    lv_style_set_radius(&slider_cont_style, LV_STATE_DEFAULT, 20);
    lv_style_set_bg_color(&slider_cont_style, LV_STATE_DEFAULT, lv_color_hex(0x525252));
    lv_style_set_border_width(&slider_cont_style, LV_STATE_DEFAULT, 0);

    lv_style_init(&slider_style);
    lv_style_set_border_width(&slider_style, LV_STATE_DEFAULT|LV_STATE_PRESSED, 2);
    lv_style_set_border_color(&slider_style, LV_STATE_DEFAULT|LV_STATE_PRESSED, LV_COLOR_BLACK);

    //home screen and background
    lv_obj_t * homescr = lv_obj_create(NULL, NULL);
    lv_scr_load(homescr);
    LV_IMG_DECLARE(chess_timer_ui_home_background);
    homeimg = lv_img_create(homescr, NULL);
    lv_img_set_src(homeimg, &chess_timer_ui_home_background);

    //top container and slider screen container
    bg_top = lv_obj_create(homescr, NULL);
    lv_obj_clean_style_list(bg_top, LV_OBJ_PART_MAIN);
    lv_obj_set_size(bg_top, LV_HOR_RES_MAX, 57);
    lv_obj_align(bg_top, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    slider_scr_cont = lv_obj_create(homescr, NULL);
    lv_obj_clean_style_list(slider_scr_cont, LV_OBJ_PART_MAIN);
    lv_obj_set_size(slider_scr_cont, LV_HOR_RES_MAX, LV_VER_RES_MAX);
    lv_obj_align(slider_scr_cont, NULL, LV_ALIGN_CENTER, 0, 0);

    //main title, battery
    lv_obj_t * bg_top_my_name = lv_label_create(bg_top, NULL);
    lv_obj_set_style_local_text_font(bg_top_my_name, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_32);
    lv_obj_set_style_local_text_color(bg_top_my_name, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_label_set_text(bg_top_my_name, "Ian Ress");
    lv_obj_set_pos(bg_top_my_name, 5, 0);

    lv_obj_t * bg_top_title = lv_label_create(bg_top, NULL);
    lv_obj_set_style_local_text_font(bg_top_title, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_18);
    lv_obj_set_style_local_text_color(bg_top_title, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_label_set_text(bg_top_title, "Chess Timer");
    lv_obj_align(bg_top_title, bg_top_my_name, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

    lv_obj_t * bat_symbol = lv_label_create(bg_top, NULL);
    lv_obj_set_style_local_text_font(bat_symbol, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_24);
    lv_obj_set_style_local_text_color(bat_symbol, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GREEN);
    lv_label_set_text(bat_symbol, LV_SYMBOL_BATTERY_FULL);
    lv_obj_set_pos(bat_symbol, 204, 0);

    lv_obj_t * bat_label = lv_label_create(bg_top, NULL);
    lv_obj_set_style_local_text_color(bat_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_text_font(bat_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_12);
    lv_label_set_text_fmt(bat_label, "%d%%", bat_remaining);
    lv_obj_align(bat_label, bat_symbol, LV_ALIGN_OUT_LEFT_MID, -3, 0);

    //time button
    time_btn = create_btn(homescr, "Timer", false, timer_btn_pressed_cb);
    lv_obj_align(time_btn, NULL, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    //move offscreen
    lv_obj_set_y(bg_top, -lv_obj_get_height(bg_top));

    //animations
    anim_bg_top(200);

    //debugging

    /* animate homescreen if it's first turned on
    static bool firstrun = true;
    if (firstrun == true){
        power_on_anim(homescr, 200);
        firstrun = false;
    }*/
}

static lv_obj_t * create_btn(lv_obj_t * parent, char * label, bool red_label, lv_event_cb_t event_cb)
{
    lv_obj_t * btn = lv_btn_create(parent, NULL);
    lv_obj_clean_style_list(btn, LV_BTN_PART_MAIN);
    lv_obj_set_size(btn, 160, 45);
    lv_obj_add_style(btn, LV_BTN_PART_MAIN, &default_btn_style);

    if (red_label == true) //make label red
    {
        lv_obj_add_style(btn, LV_BTN_PART_MAIN, &red_btn_style);
    }

    lv_obj_t * btn_label = lv_label_create(btn, NULL);
    lv_label_set_text(btn_label, label);
    lv_obj_set_event_cb(btn, *event_cb);

    return btn;
}

static void timer_btn_pressed_cb(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_PRESSED) {
        anim_bg_top(200);
        create_slider_scr();
    }
}

//animates the homescreen in or out for 'time' ms
static void anim_bg_top(uint32_t time)
{
    static bool firstpass = true;
    lv_coord_t y_act = lv_obj_get_y(time_btn);

    lv_anim_t bg_anim;
    lv_anim_t btn_anim;
    lv_anim_path_t bg_path;
    lv_anim_path_t btn_path;
    lv_anim_init(&bg_anim);
    lv_anim_init(&btn_anim);
    lv_anim_path_init(&bg_path);
    lv_anim_path_init(&btn_path);
    lv_anim_set_var(&bg_anim, bg_top);
    lv_anim_set_var(&btn_anim, time_btn);
    lv_anim_set_time(&bg_anim, time);
    lv_anim_set_time(&btn_anim, time);
    lv_anim_set_exec_cb(&bg_anim, (lv_anim_exec_xcb_t) lv_obj_set_y);
    lv_anim_set_exec_cb(&btn_anim, (lv_anim_exec_xcb_t) lv_obj_set_y);

    if (firstpass == true) {
        lv_anim_path_set_cb(&bg_path, lv_anim_path_ease_in);
        lv_anim_set_values(&bg_anim, -lv_obj_get_height(bg_top), 0);
        lv_anim_set_values(&btn_anim, LV_VER_RES_MAX, 265);
        lv_anim_set_path(&bg_anim, &bg_path);
        lv_anim_set_path(&btn_anim, &bg_path);
        lv_anim_start(&bg_anim);
        lv_anim_start(&btn_anim);

        lv_obj_fade_in(bg_top, time+50, 0);
        lv_obj_fade_in(time_btn, time+50, 0);
        firstpass = false;
        return;
    }

    if (y_act == 265) {
        //animate out
        lv_anim_path_set_cb(&bg_path, lv_anim_path_ease_out);
        lv_anim_path_set_cb(&btn_path, lv_anim_path_ease_out);
        lv_anim_set_values(&bg_anim, 0, -lv_obj_get_height(bg_top));
        lv_anim_set_values(&btn_anim, 265, LV_VER_RES_MAX);
        lv_anim_set_path(&bg_anim, &bg_path);
        lv_anim_set_path(&btn_anim, &btn_path);
        lv_anim_set_playback_time(&bg_anim, time);
        lv_anim_start(&bg_anim);
        lv_anim_start(&btn_anim);

        lv_obj_fade_out(bg_top, time+50, 0);
        lv_obj_fade_out(time_btn, time+50, 0);
        lv_obj_fade_in(bg_top, time+50, time);
    }

    else if (y_act == LV_VER_RES_MAX){
        //animate in
        lv_anim_path_set_cb(&bg_path, lv_anim_path_ease_out);
        lv_anim_path_set_cb(&btn_path, lv_anim_path_ease_in);
        lv_anim_set_values(&bg_anim, 0, -lv_obj_get_height(bg_top));
        lv_anim_set_values(&btn_anim, LV_VER_RES_MAX, 265);
        lv_anim_set_path(&bg_anim, &bg_path);
        lv_anim_set_path(&btn_anim, &btn_path);
        lv_anim_set_playback_time(&bg_anim, time);
        lv_anim_set_delay(&btn_anim, time);
        lv_anim_start(&bg_anim);
        lv_anim_start(&btn_anim);

        lv_obj_fade_out(bg_top, time+50, 0);
        lv_obj_fade_in(time_btn, time+50, time);
        lv_obj_fade_in(bg_top, time+50, time);
    }
}

static void create_slider_scr()
{
    //slider container
    lv_obj_t * slider_cont = lv_obj_create(slider_scr_cont, NULL);
    lv_obj_clean_style_list(slider_scr_cont, LV_OBJ_PART_MAIN);
    lv_obj_set_size(slider_cont, 220, 90);
    lv_obj_align_mid_x(slider_cont, slider_scr_cont, LV_ALIGN_CENTER, 0);
    lv_obj_set_y(slider_cont, 99);
    lv_obj_add_style(slider_cont, LV_OBJ_PART_MAIN, &slider_cont_style);

    //slider
    lv_obj_t * slider = lv_slider_create(slider_scr_cont, NULL);
    lv_obj_set_width(slider, 196);
    lv_obj_align(slider, slider_cont, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_adv_hittest(slider, true); //knob only mode
    lv_slider_set_range(slider, 1, 30);
    slider_minutes = 15;
    lv_slider_set_value(slider, slider_minutes, LV_ANIM_OFF);
    lv_obj_set_event_cb(slider, slider_event_cb);

    //slider styles
    lv_obj_set_style_local_outline_width(slider, LV_SLIDER_PART_BG, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_outline_width(slider, LV_SLIDER_PART_KNOB, LV_STATE_DEFAULT, 1);
    lv_obj_set_style_local_outline_color(slider, LV_SLIDER_PART_KNOB, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_outline_width(slider, LV_SLIDER_PART_INDIC, LV_STATE_DEFAULT, 1);
    lv_obj_set_style_local_outline_color(slider, LV_SLIDER_PART_INDIC, LV_STATE_DEFAULT, LV_COLOR_BLACK);

    //slider label
    slider_label = lv_label_create(slider_scr_cont, NULL);
    lv_obj_set_style_local_text_color(slider_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_text_font(slider_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_18);
    lv_label_set_text_fmt(slider_label, "Minutes per Side: %d", slider_minutes);
    lv_obj_align(slider_label, slider_cont, LV_ALIGN_IN_BOTTOM_MID, 0, -10);

    //apply button
    apply_btn = create_btn(slider_scr_cont, "Apply", false, apply_btn_cb);
    lv_obj_align_mid_x(apply_btn, slider_scr_cont, LV_ALIGN_CENTER, 0);
    lv_obj_set_y(apply_btn, 207);

    //quit button
    back_btn = create_btn(slider_scr_cont, "Back", true, back_btn_cb);
    lv_obj_align_mid_x(back_btn, slider_scr_cont, LV_ALIGN_CENTER, 0);
    lv_obj_set_y(back_btn, 265);

    lv_obj_fade_in(slider_scr_cont, 200, 200);
}

static void slider_event_cb(lv_obj_t * sliderobj, lv_event_t event)
{
    if (event == LV_EVENT_VALUE_CHANGED){
        slider_minutes = lv_slider_get_value(sliderobj);
        lv_label_set_text_fmt(slider_label, "Minutes per Side: %d", slider_minutes);
    }
}

static void apply_btn_cb(lv_obj_t *obj, lv_event_t event)
{

}

static void back_btn_cb(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_PRESSED) {
        anim_bg_top(200);
        lv_obj_fade_out(slider_scr_cont, 200, 0);
        //lv_obj_clean(slider_scr_cont);
    }

}










static void set_time_pressed_cb(lv_obj_t * obj, lv_event_t e)
{
    if (e == LV_EVENT_PRESSED){
        /**make the slider screen on bgtop**/
        // bat container
        batcontainer = lv_cont_create(bg_top, NULL);
        lv_obj_clean_style_list(batcontainer, LV_CONT_PART_MAIN);
        lv_cont_set_fit(batcontainer, LV_FIT_TIGHT);
        lv_cont_set_layout(batcontainer, LV_LAYOUT_COLUMN_MID);

        //bat symbol
        lv_obj_t * batsymbol = lv_label_create(batcontainer, NULL);
        lv_obj_set_style_local_text_font(batsymbol, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_24);
        lv_obj_set_style_local_text_color(batsymbol, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GREEN);
        lv_label_set_text(batsymbol, LV_SYMBOL_BATTERY_FULL);

        //bat label
        lv_obj_t * bat_label = lv_label_create(batcontainer, NULL);
        lv_obj_set_style_local_text_color(bat_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
        lv_label_set_text_fmt(bat_label, "%d%%", bat_remaining);
        lv_obj_align(batcontainer, bg_top, LV_ALIGN_IN_TOP_RIGHT, -5, 5);

        //back button
        lv_obj_t * backbtn = lv_btn_create(bg_top, NULL);
        lv_obj_clean_style_list(backbtn, LV_BTN_PART_MAIN);
        lv_obj_set_size(backbtn, 50, 50);
        lv_obj_align(backbtn, bg_top, LV_ALIGN_IN_TOP_LEFT, 0, 0);
        //lv_obj_set_event_cb(backbtn, backbtn_pressed_cb);

        //back button label
        lv_obj_t * backbtnlabel = lv_label_create(backbtn, NULL);
        lv_obj_set_style_local_text_color(backbtnlabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
        lv_obj_set_style_local_text_font(backbtnlabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_montserrat_24);
        lv_label_set_text(backbtnlabel, LV_SYMBOL_LEFT);

        //slider
        lv_obj_t * slider = lv_slider_create(bg_top, NULL);
        lv_obj_set_adv_hittest(slider, true); //knob only mode
        lv_slider_set_range(slider, 0, 30);
        lv_slider_set_anim_time(slider, 50);
        lv_obj_set_width(slider, 3*LV_HOR_RES_MAX/4);
        //lv_obj_set_height(slider, 50);

        //in first pass set slider at midpoint
        //once value changes, set to whatever slide_minutes is
        static bool first_pass = true;
        if (first_pass == true){
            slider_minutes = lv_slider_get_max_value(slider)/2;
            first_pass = false;
        }

        lv_slider_set_value(slider, slider_minutes, LV_ANIM_OFF);
        lv_obj_align(slider, bg_top, LV_ALIGN_CENTER, 0, 0);

        lv_obj_set_event_cb(slider, slider_event_cb);

        //slider label
        //sliderlabel = lv_label_create(bg_top, NULL);
        //lv_obj_set_style_local_text_color(sliderlabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
        //lv_obj_set_style_local_text_font(sliderlabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT,
                                       //&lv_font_montserrat_18);
        //lv_label_set_text_fmt(sliderlabel, "Minutes per Side: %d", slider_minutes);
        //lv_obj_align(sliderlabel, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

        //apply button
        lv_obj_t * applybtn = lv_btn_create(bg_top, NULL);
        lv_obj_clean_style_list(applybtn, LV_BTN_PART_MAIN);
        //lv_obj_add_style(applybtn, LV_OBJ_PART_MAIN, &homebtnstyle);
        lv_obj_set_size(applybtn, 125, 50);
        lv_obj_align(applybtn, bg_top, LV_ALIGN_IN_BOTTOM_MID, 0, -15);
        lv_obj_t * applybtnlabel = lv_label_create(applybtn, NULL);
        lv_label_set_text(applybtnlabel, "Apply");


        /**animate everything**/
        uint16_t delay =50;

        //place bgtop near bottom
        lv_obj_set_y(bg_top, (4*LV_VER_RES_MAX)/5);

        //move bgtop to top of screen while fading in
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, bg_top);
        lv_anim_set_time(&a, SLIDER_SCREEN_TIME);
        lv_anim_set_delay(&a, delay);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t) lv_obj_set_y);
        lv_anim_set_values(&a, lv_obj_get_y(bg_top), 0);
        lv_anim_start(&a);
        lv_obj_fade_in(bg_top, SLIDER_SCREEN_TIME+50, delay);

        //fade in slider move down.
        delay+=SLIDER_SCREEN_TIME;

        lv_anim_init(&a);
        lv_anim_set_var(&a, slider);
        lv_anim_set_time(&a, SLIDER_SCREEN_TIME/3);
        lv_anim_set_delay(&a, delay);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t) lv_obj_set_y);
        lv_anim_set_values(&a, lv_obj_get_y(slider)-20, lv_obj_get_y(slider));
        lv_anim_start(&a);
        lv_obj_fade_in(slider, (SLIDER_SCREEN_TIME/3)+50, delay);

        delay+=100;
        lv_anim_init(&a);
        //lv_anim_set_var(&a, sliderlabel);
        lv_anim_set_time(&a, SLIDER_SCREEN_TIME/3);
        lv_anim_set_delay(&a, delay);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t) lv_obj_set_y);
        //lv_anim_set_values(&a, lv_obj_get_y(sliderlabel)-20, lv_obj_get_y(sliderlabel));
        lv_anim_start(&a);
        //lv_obj_fade_in(sliderlabel, (SLIDER_SCREEN_TIME/3)+50, delay);

        //fade in apply btn move down.
        delay+=100;

        lv_anim_init(&a);
        lv_anim_set_var(&a, applybtn);
        lv_anim_set_time(&a, (SLIDER_SCREEN_TIME/3));
        lv_anim_set_delay(&a, delay);
        lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t) lv_obj_set_y);
        lv_anim_set_values(&a, lv_obj_get_y(applybtn)-100, lv_obj_get_y(applybtn));
        lv_anim_start(&a);
        lv_obj_fade_in(applybtn, (SLIDER_SCREEN_TIME/3)+50, delay);


        //fade in batcontainer, back btn. no move.
        delay+=100;
        lv_obj_fade_in(batcontainer, SLIDER_SCREEN_TIME/3, delay);
        lv_obj_fade_in(backbtn, SLIDER_SCREEN_TIME/3, delay);
    }
}


