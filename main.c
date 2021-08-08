/////////////////////////////////////////////////
/// @file Main.c
/// @date 9th January 2020
/// @author Rahul B
///
/// @version 9
///
/// @brief
///   Main code
///
/// @details
///      - Will perform initialization of Timers, GPIOs
///      - Configure Interrupts
///      - Read push button state and analyzes debouncing
/////////////////////////////////////////////////
#include "driverlib.h"

long count = 0;  ///< Variable to store the number of timer interrupts happening
uint8_t reading;   ///< Reading of the input button pin (4.1)
uint8_t buttonstate = GPIO_INPUT_PIN_LOW; ///< Present state of the button
uint8_t lastbuttonstate = GPIO_INPUT_PIN_LOW; ///< Previous state of the button
int ldtime = 0; ///< Lastdebouncetime
int dtime = 1; ///<default debouncetime here 1 = 50ms

/////////////////////////////////////////////////
/// @brief Main program loop
///
/// @details
///     - Calls all the Timers
///     - Controls LED
///     - Read push button state
///     @returns  void
/////////////////////////////////////////////////

void main(void)
{
    WDT_A_hold(WDT_A_BASE); // Making the watchdog timer on hold
    /////////////////////////////////////////////////
    /// @brief GPIO pins initialization
    ///
    /// @details
    ///     - Configuring 4.1 as input pin with pulldown for the push button
    ///     - Configuring 4.2 for led output
    /////////////////////////////////////////////////
    GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P4, GPIO_PIN1); // Making 4.1 as input pin

    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN2); // Making 4.2 as Output pin
    /////////////////////////////////////////////////
    /// @brief Clock initialization
    ///
    /// @details
    ///     - Using the CaptureCompare mode to make interrupt on each 50ms
    ///     - ACLK - 32KHz, CCR0 = 1500
    ///     - so time = 1500/32Khz
    /////////////////////////////////////////////////
    Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE,
                                         TIMER_A_CAPTURECOMPARE_REGISTER_0); // Clearing CCI
    _enable_interrupts(); // Enabling the interrupts
    Timer_A_initUpModeParam param = { 0 }; // Creating Timer parameter object
    param.clockSource = TIMER_A_CLOCKSOURCE_ACLK; // Initializing the Clocksource for timer
    param.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1; // Making the prescaler as 1
    param.timerPeriod = 1500; // Initializing the timeperiod
    param.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE; // Disable TAIE interrupt
    param.captureCompareInterruptEnable_CCR0_CCIE =
            TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE; // Enable CCR interrupt
    param.timerClear = TIMER_A_DO_CLEAR;
    param.startTimer = true; // Starting the timer

    Timer_A_initUpMode(TIMER_A0_BASE, &param); // Initializing the timer

    Timer_A_enableCaptureCompareInterrupt(TIMER_A0_BASE,
                                          TIMER_A_CAPTURECOMPARE_REGISTER_0); // Enabling the interrupt explicitly
    /////////////////////////////////////////////////
    /// @brief Loop
    ///
    /// @details
    ///     - Read the button-state
    ///     - Check the button-state whether it stayed for more than the debounce time
    ///     - If the button state was really changed toggle the led
    /////////////////////////////////////////////////
    while (1)
    {

        reading = GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN1); // Reading the input pin value

        if (lastbuttonstate != reading) // Checking whether the button state has changed due to noise or is pressed
        {
            ldtime = count; // If changed, Store the count at the detection of the change
        }
        if ((count - ldtime) > dtime) // Checking whether the change change stayed for more than debouncing time
        {
            if (buttonstate != reading) //  Checking the present buttonstate with reading
            {
                buttonstate = reading;
                if (buttonstate == GPIO_INPUT_PIN_HIGH) // If button is pressed
                    GPIO_toggleOutputOnPin(GPIO_PORT_P4, GPIO_PIN2); // Toggle led
            }
        }
        lastbuttonstate = reading;  // updating the lastbuttonstate
    }
}
/////////////////////////////////////////////////
/// @brief ISR definition
///
/// @details
///     - Initializing the ISR vector as timer0_a0_vector
///     - Incrementing count on each 50ms
/////////////////////////////////////////////////
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(TIMER0_A0_VECTOR)))
#endif
void timer_isr(void) // ISR Function
{
    count += 1;  // Incrementing count on each interrupt

}
