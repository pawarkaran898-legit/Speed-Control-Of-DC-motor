## Speed Control of DC Motor using Arduino (PWM)

This project demonstrates **speed and distance-based safety indication for a DC motor / moving object** using an Arduino. It measures distance with an ultrasonic sensor and drives:

- **Green / Yellow / Red LEDs** to indicate safe / caution / danger distance zones.
- **Buzzer** with different beep patterns depending on how close an obstacle is.
- Optional **I²C LCD (second Arduino as slave)** if `USE_LCD` is enabled in the sketch.

The code is contained in `speedcontrol.ino`.

### Hardware Used

- Arduino Uno (or compatible)
- HC-SR04 (or similar) ultrasonic distance sensor
- 3× LEDs: Green, Yellow, Red with current‑limiting resistors
- Piezo buzzer
- (Optional) Second Arduino + 16×2 LCD as I²C slave display
- Breadboard, jumper wires, USB cable

### Pin Connections

- **Ultrasonic sensor**
  - `trigPin` → D8
  - `echoPin` → D9
- **LEDs**
  - Green LED (`ledG`) → D4
  - Yellow LED (`ledY`) → D5
  - Red LED (`ledR`) → D6
- **Buzzer**
  - `buzzer` → D10

You can change these in `speedcontrol.ino` if your wiring is different.

### Zone Thresholds

In `speedcontrol.ino`:

- `SAFE_T = 30` → distance \(>\) 30 cm is **SAFE** (green LED on, buzzer off)
- `WARN_T = 15` → 15–30 cm is **CAUTION** (yellow LED on, slow beeps)
- `< 15 cm` is **DANGER** (red LED on, fast beeps)

You can tune these constants for your specific setup.

### Optional LCD Display

At the top of `speedcontrol.ino` there is:

```cpp
#define USE_LCD false
```

Set this to `true` if you still use the second Arduino + LCD as an I²C slave (address 8). When enabled, the master sends distance and zone text to the LCD via `Wire`.

### Serial Output (for plotting / logging)

The sketch sends CSV lines over the serial port at **9600 baud**:

```text
timestamp_ms,raw_cm,filtered_cm,alert
```

Where:

- `timestamp_ms` – `millis()` since reset
- `raw_cm` – instantaneous distance reading
- `filtered_cm` – low‑pass filtered distance
- `alert` – 0 = SAFE, 1 = CAUTION, 2 = DANGER

You can log or plot this data from Python, Excel, etc.

### Getting Started

1. Install the **Arduino IDE**.
2. Open `speedcontrol.ino` in the IDE.
3. Select your board and COM port.
4. Click **Upload**.
5. Open **Serial Monitor** at **9600 baud** to see live distance and alert values.

### Repository Structure

- `speedcontrol.ino` – main Arduino sketch.
- `README.md` – project description, wiring, and usage instructions.
- (Optional) `docs/` or `images/` – you can add your PPT or screenshots here before pushing to GitHub.

### How to Initialize and Push to GitHub

From `c:\projects\speedcontrol`:

```bash
git init
git add .
git commit -m "Initial commit: Arduino speed control project"
git branch -M main
git remote add origin https://github.com/<your-username>/<your-repo-name>.git
git push -u origin main
```

Replace `<your-username>` and `<your-repo-name>` with your actual GitHub info.
