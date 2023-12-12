// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
//const int rs = 48, en = 49, d4 = 50, d5 = 51, d6 = 52, d7 = 53;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//adjust this variable based on how many different stats you want to display
const int num_messages = 2;

//these are string pairs in the form top row, bottom row
//we need to generate these strings with readings from the actual sensors
//the lcd board is 16x2, so each message must 
//this is the only thing that needs to happen on the sensor side -- 
//all the formatting is already taken care of in this code
String messages[num_messages][num_messages] = {
  {"Carbon Emitted: ", "1000 kg "},
  {"Algae Produced: ", "20 grams"}
};



int currentMessage = 0;
String space = "                "; // 16 spaces to fill the LCD screen
String scrollMessageTop, scrollMessageBottom;
void setup() {
  lcd.begin(16, 2); // Initialize the LCD
  prepareScrollMessages();
}
void loop() {
  for (int i = 0; i < scrollMessageTop.length() - 16; i++) {
    lcd.clear(); // Clear the display
    // Display a portion of the top message
    lcd.setCursor(0, 0);
    lcd.print(scrollMessageTop.substring(i, i + 16));
    // Display a portion of the bottom message
    lcd.setCursor(0, 1);
    lcd.print(scrollMessageBottom.substring(i, i + 16));
    if (i == 0) {
      delay(3000); // Pause for 5 seconds when the full message is displayed
    } else {
      delay(300); // Adjust delay for scrolling speed
    }
  }
  // Change to the next message set
  currentMessage = (currentMessage + 1) % 2;
  prepareScrollMessages();
}
void prepareScrollMessages() {
  // Prepare the messages for scrolling
  scrollMessageTop = messages[currentMessage][0] + space;
  scrollMessageBottom = messages[currentMessage][1] + space;
}
