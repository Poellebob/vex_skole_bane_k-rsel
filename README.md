# **Path System for Autonomous Robot Control**

This README provides an overview of the path system used in the robot's autonomous movement. It explains the logic behind path parsing and the mathematics used to control wheel speeds for linear and circular movement.

---

## **1. How the Path System Works**

The path system defines a sequence of movements that the robot will follow. Each movement is stored as a **string** in the `path` vector, where the type of movement and its parameters are specified.

### **Path Format**

The format of each path step is as follows:

- **Line Movement**:

  ```
  line: distance(cm), speed(rpm)
  ```

  - `distance`: The distance (in cm) that the robot will move forward.
  - `speed`: The speed of the robot in **rotations per minute (RPM)**.

- **Circular Movement**:

  ```
  circ: angle, dir(left or right), radius(cm), speed(rpm)
  ```

  - `angle`: The angle (in degrees) that the robot will turn.
  - `dir`: The direction of the turn, either `left` or `right`.
  - `radius`: The radius (in cm) of the turn, measured from the **center of the robot**.
  - `speed`: The speed (RPM) of the outer wheel.

- **Loop Movement**:
  ```
  loop
  ```
  - This command restarts the path from the beginning, allowing for infinite repetition of the path.

### **Example Path**

```cpp
std::vector<std::string> path = {
	"line: 190, 40",
	"circ: 93, right, 30, 40",
	"line: 220, 40",
	"loop"
};
```

**Explanation:**

1. Move forward 190 cm at 40 RPM.
2. Turn **right** 93° with a radius of 30 cm at 40 RPM.
3. Move forward 220 cm at 40 RPM.
4. Repeat the path from the beginning.

---

## **2. How the Wheel Movement Math Works**

### **1. Line Movement**

For linear movement, both wheels move at the same speed, allowing the robot to drive straight.

**Parameters:**

- `distance`: Distance to move (cm)
- `speed`: Speed of the wheels (RPM)

**Calculation Steps:**

1. **Time to Complete Movement**:  
   $\text{time} = \frac{\text{distance}}{\text{wheel circumference}} \div \text{speed}$

   - This calculates how long (in minutes) the robot needs to move to cover the given distance.
   - Since `pros::delay` uses milliseconds, the result is converted to milliseconds:  
     $\text{time} \times 60,000$

2. **Move Wheels at the Same Speed**:  
   Both motors are set to the same speed:
   ```cpp
   left_mtr.move_velocity(speed);
   right_mtr.move_velocity(speed);
   ```

---

### **2. Circular Movement**

For circular movement, the robot follows an arc. One wheel moves faster than the other to achieve this effect.

**Parameters:**

- `angle`: Angle to rotate (degrees)
- `dir`: Direction of turn (left or right)
- `radius`: Radius of the turn (cm)
- `speed`: Speed of the outer wheel (RPM)

**Explanation of the Math:**

1. **Calculate Speed Ratio Between Wheels**  
   Since one wheel moves along a larger arc than the other, the inner wheel must move slower.  
   $\text{inner speed} = \text{speed} \times \frac{\text{radius} - \frac{\text{robot width}}{2}}{\text{radius} + \frac{\text{robot width}}{2}}$
   This formula calculates the speed ratio of the inner wheel relative to the outer wheel.

2. **Set Wheel Speeds**

   - If the direction is **left**, the **left wheel** moves slower.
     ```cpp
     left_mtr.move_velocity(speed * (radius - (robot_width / 2)) / (radius + (robot_width / 2)));
     right_mtr.move_velocity(speed);
     ```
   - If the direction is **right**, the **right wheel** moves slower.
     ```cpp
     right_mtr.move_velocity(speed * (radius - (robot_width / 2)) / (radius + (robot_width / 2)));
     left_mtr.move_velocity(speed);
     ```

3. **Calculate Time to Complete the Turn**  
   To calculate how long the turn should last, we need to determine how far the outer wheel will move.  
   $\text{distance} = 2 \pi (\text{radius} + \frac{\text{robot width}}{2}) \cdot \frac{\text{angle}}{360}$
   - This gives the arc length (in cm) for the outer wheel.
   - Next, calculate the time required for the wheel to move this distance:  
     $\text{time} = \frac{\text{distance}}{\text{wheel circumference}} \div \text{speed}$
   - Convert to milliseconds:  
     $\text{time} \times 60,000$
   - **Delay for this time to complete the turn.**

---

## **3. Code Walkthrough**

1. **Path Iteration**

   ```cpp
   for (int i = 0; i < path.size(); i++)
   ```

   Each path step is read, and its type (`line`, `circ`, or `loop`) is determined.  
   If a **loop** command is encountered, the path index `i` is reset to `-1`, so the loop starts over.

2. **Line Movement Handling**

   ```cpp
   if ("line" == path_type)
   ```

   - Extracts `distance` and `speed` from the path.
   - Calls `move_velocity` to set wheel speeds.
   - Calculates the required delay and calls `pros::delay`.

3. **Circular Movement Handling**

   ```cpp
   if ("circ" == path_type)
   ```

   - Extracts `angle`, `dir`, `radius`, and `speed` from the path.
   - Calls `move_velocity` for the inner and outer wheel, according to the turning direction.
   - Calculates the required delay and calls `pros::delay`.

4. **Stop the Robot**
   After completing all path steps, both motors are set to zero velocity.

---

## **4. Constants Used in Calculations**

| **Constant**          | **Value**                | **Description**                                         |
| --------------------- | ------------------------ | ------------------------------------------------------- |
| `wheel_circumference` | \( 10.3 \times \pi \) cm | The wheel's circumference, used to calculate rotations. |
| `robot_width`         | 34 cm                    | The distance between the two wheels (wheelbase).        |

---

## **5. How to Add Custom Paths**

1. Open the C++ file and modify the `path` vector.
2. Add new steps using the **line**, **circ**, and **loop** formats.

### **Example**

```cpp
std::vector<std::string> path = {
	"line: 100, 50",       // Move 100 cm at 50 RPM
	"circ: 180, left, 45, 60",  // Turn left 180° with a 45 cm radius at 60 RPM
	"loop"                 // Repeat the path from the beginning
};
```

---

# Use and upload the code to vex v5

To use this code in Visual Studio Code (VSCode) with the PROS extension, follow these steps:

1. **Install VSCode**: If you haven't already, download and install Visual Studio Code from [here](https://code.visualstudio.com/).

2. **Install the PROS Extension**:

   - Open VSCode.
   - Go to the Extensions view by clicking on the Extensions icon in the Activity Bar on the side of the window or by pressing `Ctrl+Shift+X`.
   - Search for "PROS" and install the PROS extension.

3. **Open the Project**:

   - Download your project as a .zip file from GitHub by clicking the green "Code" button, then selecting "Download ZIP".
   - Extract the downloaded .zip file to a directory of your choice.
   - Open the cloned project folder in VSCode by going to `File > Open Folder...` and select the folder with the code.

4. **Build the Project**:

   - Find the `src/main.cpp` and open it by clicking on it.
   - Connect the vex v5 brain to your computer via usb.
   - Find the PROS "build and upload" button in the top right and click on it.

5. **Run the Code**:
   - Press the play button on the v5 brain.

For more detailed information, refer to the [PROS documentation](https://pros.cs.purdue.edu/v5/getting-started/index.html).
