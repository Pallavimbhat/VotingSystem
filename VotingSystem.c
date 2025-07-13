#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

// LCD control pins
#define LCD_RS PC0
#define LCD_E  PC1
#define LCD_CONTROL_PORT PORTC
#define LCD_CONTROL_DDR  DDRC

// LCD data pins on PD4–PD7
#define LCD_PORT PORTD
#define LCD_DDR  DDRD

// Buttons on PORTA
#define BTN_BJP PA0
#define BTN_INC PA1
#define BTN_AAP PA2
#define BTN_TMC PA3
#define BTN_RES PA4
#define BUTTON_PIN PINA
#define BUTTON_PORT PORTA
#define BUTTON_DDR DDRA

// Vote counters
volatile uint16_t votes_bjp = 0;
volatile uint16_t votes_inc = 0;
volatile uint16_t votes_aap = 0;
volatile uint16_t votes_tmc = 0;

// LCD enable pulse
void lcd_enable_pulse(void) {
    LCD_CONTROL_PORT |= (1 << LCD_E);
    _delay_us(1);
    LCD_CONTROL_PORT &= ~(1 << LCD_E);
    _delay_us(20);
}

// Send command to LCD
void lcd_command(unsigned char cmd) {
    LCD_CONTROL_PORT &= ~(1 << LCD_RS);
    LCD_PORT = (LCD_PORT & 0x0F) | (cmd & 0xF0);
    lcd_enable_pulse();
    LCD_PORT = (LCD_PORT & 0x0F) | ((cmd << 4) & 0xF0);
    lcd_enable_pulse();
    _delay_ms(0.4);
}

// Send character to LCD
void lcd_char(unsigned char data) {
    LCD_CONTROL_PORT |= (1 << LCD_RS);
    LCD_PORT = (LCD_PORT & 0x0F) | (data & 0xF0);
    lcd_enable_pulse();
    LCD_PORT = (LCD_PORT & 0x0F) | ((data << 4) & 0xF0);
    lcd_enable_pulse();
    _delay_ms(0.4);
}

// Send string to LCD
void lcd_string(const char *str) {
    while (*str) {
        lcd_char(*str++);
    }
}

// Clear LCD screen
void lcd_clear(void) {
    lcd_command(0x01);
    _delay_ms(0.4);
}

// LCD initialization
void lcd_init(void) {
    LCD_CONTROL_DDR |= (1 << LCD_RS) | (1 << LCD_E);
    LCD_DDR |= 0xF0;
    _delay_ms(4);

    LCD_PORT &= 0x0F;
    LCD_PORT |= 0x30; lcd_enable_pulse(); _delay_ms(1);
    LCD_PORT |= 0x30; lcd_enable_pulse(); _delay_us(30);
    LCD_PORT |= 0x30; lcd_enable_pulse(); _delay_us(30);
    LCD_PORT &= 0x0F; LCD_PORT |= 0x20; lcd_enable_pulse(); _delay_us(30);

    lcd_command(0x28);
    lcd_command(0x0C);
    lcd_command(0x06);
    lcd_clear();
}

// Debounce check
uint8_t is_button_pressed(uint8_t pin) {
    if (!(BUTTON_PIN & (1 << pin))) {
        _delay_ms(10);
        if (!(BUTTON_PIN & (1 << pin))) {
            while (!(BUTTON_PIN & (1 << pin)));
            return 1;
        }
    }
    return 0;
}

// Check if ALL buttons are pressed for reset
uint8_t is_reset_pressed(void) {
    return (!(BUTTON_PIN & (1 << BTN_BJP)) &&
            !(BUTTON_PIN & (1 << BTN_INC)) &&
            !(BUTTON_PIN & (1 << BTN_AAP)) &&
            !(BUTTON_PIN & (1 << BTN_TMC)) &&
            !(BUTTON_PIN & (1 << BTN_RES)));
}

// Main function
int main(void) {
    char lcd_buf[16];
    lcd_init();

    // Set button pins as input & enable pull-ups
    BUTTON_DDR &= ~((1 << BTN_BJP) | (1 << BTN_INC) | (1 << BTN_AAP) | (1 << BTN_TMC) | (1 << BTN_RES));
    BUTTON_PORT |= (1 << BTN_BJP) | (1 << BTN_INC) | (1 << BTN_AAP) | (1 << BTN_TMC) | (1 << BTN_RES);

    // Welcome screen
    lcd_string("Voting System");
    _delay_ms(500);
    lcd_clear();
    lcd_string("Ready to Vote");
    _delay_ms(500);
    lcd_clear();

    while (1) {
        if (is_button_pressed(BTN_BJP)) {
            votes_bjp++;
            lcd_clear();
            lcd_string("BJP Voted");
            _delay_ms(300);
            lcd_clear();
            snprintf(lcd_buf, sizeof(lcd_buf), "BJP: %u", votes_bjp);
            lcd_string(lcd_buf);
            _delay_ms(300);
            lcd_clear();
        }

        if (is_button_pressed(BTN_INC)) {
            votes_inc++;
            lcd_clear();
            lcd_string("INC Voted");
            _delay_ms(300);
            lcd_clear();
            snprintf(lcd_buf, sizeof(lcd_buf), "INC: %u", votes_inc);
            lcd_string(lcd_buf);
            _delay_ms(300);
            lcd_clear();
        }

        if (is_button_pressed(BTN_AAP)) {
            votes_aap++;
            lcd_clear();
            lcd_string("AAP Voted");
            _delay_ms(300);
            lcd_clear();
            snprintf(lcd_buf, sizeof(lcd_buf), "AAP: %u", votes_aap);
            lcd_string(lcd_buf);
            _delay_ms(300);
            lcd_clear();
        }

        if (is_button_pressed(BTN_TMC)) {
            votes_tmc++;
            lcd_clear();
            lcd_string("TMC Voted");
            _delay_ms(300);
            lcd_clear();
            snprintf(lcd_buf, sizeof(lcd_buf), "TMC: %u", votes_tmc);
            lcd_string(lcd_buf);
            _delay_ms(300);
            lcd_clear();
        }

        if (is_button_pressed(BTN_RES)) {
            lcd_clear();
            snprintf(lcd_buf, sizeof(lcd_buf), "B:%u I:%u", votes_bjp, votes_inc);
            lcd_string(lcd_buf);
            _delay_ms(500);
            lcd_clear();
            snprintf(lcd_buf, sizeof(lcd_buf), "A:%u T:%u", votes_aap, votes_tmc);
            lcd_string(lcd_buf);
            _delay_ms(800);
            lcd_clear();
        }

        // Special: reset if all buttons pressed
        if (is_reset_pressed()) {
            votes_bjp = votes_inc = votes_aap = votes_tmc = 0;
            lcd_clear();
            lcd_string("Votes Reset");
            _delay_ms(1000);
            lcd_clear();
        }
    }
}
