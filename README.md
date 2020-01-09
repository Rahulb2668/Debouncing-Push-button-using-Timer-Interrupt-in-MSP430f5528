# Debouncing-Push-button-using-Timer-Interrupt-in-MSP430f5528
Debouncing Push button using Timer Interrupt in MSP430f5528 with Driverlib library


Creating a software deboucing using timer interrupt in MSP430ff28 (MisPigy).
Checking the debouncing and toggle led.
When the button is pressed a count value is stored as the lastdebouncetime.
Then we check the button is pressed or is high for more than the basic debounce delay time (50ms)

If it is high for more than 50ms the led is toggled.

