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
	_delay_us(20);  // reduced from 100 µs
}

// Send command to LCD
void lcd_command(unsigned char cmd) {
	LCD_CONTROL_PORT &= ~(1 << LCD_RS);
	LCD_PORT = (LCD_PORT & 0x0F) | (cmd & 0xF0);
	lcd_enable_pulse();
	LCD_PORT = (LCD_PORT & 0x0F) | ((cmd << 4) & 0xF0);
	lcd_enable_pulse();
	_delay_ms(0.4);  // reduced from 2ms
}

// Send character to LCD
void lcd_char(unsigned char data) {
	LCD_CONTROL_PORT |= (1 << LCD_RS);
	LCD_PORT = (LCD_PORT & 0x0F) | (data & 0xF0);
	lcd_enable_pulse();
	LCD_PORT = (LCD_PORT & 0x0F) | ((data << 4) & 0xF0);
	lcd_enable_pulse();
	_delay_ms(0.4);  // reduced from 2ms
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
	_delay_ms(0.4);  // reduced from 2ms
}

// LCD initialization
void lcd_init(void) {
	LCD_CONTROL_DDR |= (1 << LCD_RS) | (1 << LCD_E);
	LCD_DDR |= 0xF0;  // PD4–PD7 as output
	_delay_ms(4);     // reduced from 20ms

	LCD_PORT &= 0x0F;
	LCD_PORT |= 0x30; lcd_enable_pulse(); _delay_ms(1);
	LCD_PORT |= 0x30; lcd_enable_pulse(); _delay_us(30);
	LCD_PORT |= 0x30; lcd_enable_pulse(); _delay_us(30);
	LCD_PORT &= 0x0F; LCD_PORT |= 0x20; lcd_enable_pulse(); _delay_us(30);

	lcd_command(0x28); // 4-bit mode, 2-line
	lcd_command(0x0C); // Display ON, Cursor OFF
	lcd_command(0x06); // Auto increment
	lcd_clear();
}

// Debounce and wait for release
uint8_t is_button_pressed(uint8_t pin) {
	if (!(BUTTON_PIN & (1 << pin))) {
		_delay_ms(10);  // reduced from 50ms
		if (!(BUTTON_PIN & (1 << pin))) {
			while (!(BUTTON_PIN & (1 << pin))); // wait for release
			return 1;
		}
	}
	return 0;
}

// Main function
int main(void) {
	char lcd_buf[16];
	lcd_init();

	// Set buttons as input and enable pull-up resistors
	BUTTON_DDR &= ~((1 << BTN_BJP) | (1 << BTN_INC) | (1 << BTN_AAP) | (1 << BTN_TMC) | (1 << BTN_RES));
	BUTTON_PORT |= (1 << BTN_BJP) | (1 << BTN_INC) | (1 << BTN_AAP) | (1 << BTN_TMC) | (1 << BTN_RES);

	lcd_string("Voting System");
	_delay_ms(400);  // reduced from 2000ms
	lcd_clear();

	while (1) {
		if (is_button_pressed(BTN_BJP)) {
			votes_bjp++;
			lcd_clear(); lcd_string("BJP Voted");
			_delay_ms(200); lcd_clear();  // reduced from 1000ms
		}
		if (is_button_pressed(BTN_INC)) {
			votes_inc++;
			lcd_clear(); lcd_string("INC Voted");
			_delay_ms(200); lcd_clear();
		}
		if (is_button_pressed(BTN_AAP)) {
			votes_aap++;
			lcd_clear(); lcd_string("AAP Voted");
			_delay_ms(200); lcd_clear();
		}
		if (is_button_pressed(BTN_TMC)) {
			votes_tmc++;
			lcd_clear(); lcd_string("TMC Voted");
			_delay_ms(200); lcd_clear();
		}
		if (is_button_pressed(BTN_RES)) {
			lcd_clear();
			snprintf(lcd_buf, sizeof(lcd_buf), "B:%u I:%u", votes_bjp, votes_inc);
			lcd_string(lcd_buf);
			_delay_ms(400);  // reduced from 2000ms
			lcd_clear();
			snprintf(lcd_buf, sizeof(lcd_buf), "A:%u T:%u", votes_aap, votes_tmc);
			lcd_string(lcd_buf);
			_delay_ms(600);  // reduced from 3000ms
			lcd_clear();
		}
	}
}