# 🗳️ AVR-Based Electronic Voting Machine

## ✨ Brief Introduction
- This project implements a simple electronic voting machine using an **AVR microcontroller**.  
- It allows voting for four parties — **BJP, INC, AAP, and TMC** — through dedicated buttons, and displays voting results on an LCD screen.  
- The system ensures accurate vote counting through **debouncing**, uses **pull-up resistors** to maintain stable inputs, and provides immediate feedback via the LCD.

---

## 🔍 Algorithm Overview
1. **Initialize the System**
   - Set the CPU frequency and include all necessary header files.
   - Define pins for LCD control lines, data lines, voting buttons, and vote counters.

2. **Set Up the LCD**
   - Initialize the LCD in **4-bit mode** for efficient pin use.
   - Clear the screen and display a welcome message such as:
     ```
     Voting System
     ```

3. **Configure Button Inputs**
   - Set the button pins as inputs.
   - Enable internal pull-up resistors to prevent floating input states.

4. **Main Voting Loop**
   - Continuously monitor all buttons.
     - If a party button is pressed:
       - Increment that party’s vote count.
       - Display a short confirmation message on the LCD, e.g.:
         ```
         BJP Voted
         ```
     - If the results button is pressed:
       - Sequentially display the vote counts for all four parties across two screens.

5. **Implement Debouncing**
   - Introduce a short delay after detecting a button press to avoid false multiple counts due to mechanical bouncing.

