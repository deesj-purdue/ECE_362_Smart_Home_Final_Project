#include "keypad.h"

// Keypad globals initialization
volatile uint8_t col = 0;
volatile char *keymap = "DCBA#9630852*741";

volatile uint8_t hist[16]; // array of 16 history bytes for 16 keys
volatile char queue[2];    // A two-entry queue of button press/release events.
volatile int qin;          // Which queue entry is next for input
volatile int qout;         // Which queue entry is next for output
volatile char password_entry_str[PASSWORD_LENGTH + 1];
volatile char temperature_entry_str[3];

volatile bool KEYPAD_TIMEOUT = false;

void update_keypad_state()
{
    switch (SECURITY_STATE)
    {
    case DISARMED:
        KEYPAD_STATE = TEMPERATURE_ENTRY;
        break;
    case ARMED:
        KEYPAD_STATE = TEMPERATURE_ENTRY;
        break;
    case PASSWORD:
        KEYPAD_STATE = PASSWORD_ENTRY;
        break;
    case ALARM:
        KEYPAD_STATE = PASSWORD_ENTRY;
        break;
    }
}

bool check_password()
{
    for (int i = 0; i < PASSWORD_LENGTH; i++)
    {
        if (password_entry_str[i] != CORRECT_PASSWORD[i])
            return false;
    }
    return true;
}

void init_tim7_keypad()
{
    // GPIOC Setup
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    GPIOC->MODER |= (GPIO_MODER_MODER4_0 | GPIO_MODER_MODER5_0 |
                     GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0); // Configure PC4-7 (columns) as output
    GPIOC->MODER &= ~(GPIO_MODER_MODER0 | GPIO_MODER_MODER1 |
                      GPIO_MODER_MODER2 | GPIO_MODER_MODER3); // Assure that PC0-3 are input
    GPIOC->OTYPER |= (GPIO_OTYPER_OT_4 | GPIO_OTYPER_OT_5 |
                      GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_7); // Configure PC4-7 for open drain
    GPIOC->PUPDR |= (GPIO_PUPDR_PUPDR0_0 | GPIO_PUPDR_PUPDR1_0 |
                     GPIO_PUPDR_PUPDR2_0 | GPIO_PUPDR_PUPDR3_0); // Pull PC0-3 high when floating

    // TIM7 Setup
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN; // Enable clock for TIM7
    TIM7->PSC = 4800 - 1;
    TIM7->ARR = 10 - 1;             // Scale to 1kHz
    TIM7->DIER |= TIM_DIER_UIE;     // Enable interrupt when timer hits value in TIM7_ARR
    NVIC->ISER[0] = 1 << TIM7_IRQn; // Enable interrupt in the NVIC
    TIM7->CR1 |= TIM_CR1_CEN;       // Enable timer
}

void TIM7_IRQHandler()
{
    TIM7->SR &= ~TIM_SR_UIF;
    int rows = read_rows();
    update_history(col, rows);
    col = (col + 1) & 3;
    drive_column(col);
}

void get_password_string()
{
    for (int i = 0; i < PASSWORD_LENGTH; i++)
    {
        char key = get_keypress();
        password_entry_str[i] = key;
    }
    password_entry_str[PASSWORD_LENGTH] = '\0';

    if (KEYPAD_TIMEOUT)
        KEYPAD_TIMEOUT = false;
}

void get_temperature_input()
{
    TARGET_TEMPERATURE = 0;
    for (int i = 0; i < 2; i++)
    {
        char key = get_keypress();
        if (key == 'A')
        {
            SECURITY_STATE = ARMED;
            return;
        }
        else if (key == 'B') // DEBUG for testing
        {
            SECURITY_STATE = PASSWORD;
            return;
        }

        TARGET_TEMPERATURE += (key - '0') * pow(10, 1 - i);
    }

    if (TARGET_TEMPERATURE < MIN_TEMPERATURE)
        TARGET_TEMPERATURE = MIN_TEMPERATURE;

    if (TARGET_TEMPERATURE > MAX_TEMPERATURE)
        TARGET_TEMPERATURE = MAX_TEMPERATURE;

    if (KEYPAD_TIMEOUT)
        KEYPAD_TIMEOUT = false;
}

// Keypad Helpers

void drive_column(uint8_t col)
{
    col &= 0x3;
    GPIOC->BSRR = 0x00F0; // set all columns (PC4-7) to 1
    if (col >= 0 && col < 4)
    {
        GPIOC->BRR = 1 << (col + 4); // set col to 0 (active low)
    }
    else
    {
        return;
    }
}

int read_rows()
{
    return (~GPIOC->IDR & 0x000F); // Pins PC0-3 correspond to rows 4-1 (MSB top row)
}

void push_queue(int n)
{
    queue[qin] = n;
    qin ^= 1;
}

char pop_queue()
{
    char tmp = queue[qout];
    queue[qout] = 0;
    qout ^= 1;
    return tmp;
}

void update_history(int col, int rows)
{
    // this updates the correct history byte for each column
    for (int i = 0; i < 4; i++)
    {
        hist[4 * col + i] = (hist[4 * col + i] << 1) + ((rows >> i) & 1);
        if (hist[4 * col + i] == 0x01)
            // push event to queue with flag upon first press event
            push_queue(0x80 | keymap[4 * col + i]);
        if (hist[4 * col + i] == 0xfe)
            // push event to queue without flag upon a release
            push_queue(keymap[4 * col + i]);
    }
}

char get_key_event()
{
    for (;;)
    {
        // asm volatile ("wfi");   // wait for an interrupt
        if (queue[qout] != 0)
            break;

        if (KEYPAD_TIMEOUT)
            return 0;
    }
    return pop_queue();
}

char get_keypress()
{
    char event;
    for (;;)
    {
        event = get_key_event();
        if (event & 0x80) // only break if event has the press flag
            break;

        if (KEYPAD_TIMEOUT)
            return 0;
    }
    return event & 0x7f; // return char without flag
}

void init_tim14_timer()
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;
    TIM14->PSC = 48000 - 1;               // 48 MHz / 48000 = 1 KHz
    TIM14->ARR = PASSWORD_TIMEOUT_MS - 1; // 1 KHz / 5000 = .2 Hz = 5 seconds
    TIM14->DIER |= TIM_DIER_UIE;
    TIM14->CR1 |= TIM_CR1_CEN;
    NVIC_EnableIRQ(TIM14_IRQn);
}

void start_password_timeout()
{
    KEYPAD_TIMEOUT = false;
    TIM14->CNT = 0;
    TIM14->CR1 |= TIM_CR1_CEN;
}

void stop_password_timeout()
{
    TIM14->CR1 &= ~TIM_CR1_CEN;
    TIM14->CNT = 0;
}

// TIM14 interrupt handler
void TIM14_IRQHandler(void)
{
    if (TIM14->SR & TIM_SR_UIF) // Check if update interrupt flag is set
    {
        TIM14->SR &= ~TIM_SR_UIF; // Clear the update interrupt flag
        KEYPAD_TIMEOUT = true;
        stop_password_timeout(); // Stop the timer when timeout occurs
    }
}