#include "main.h"
#include "vector"
// #include "math.h"

// spesify the ports for the motors and put "-" for ones that are reversed
pros::Motor left_mtr(-1);
pros::Motor right_mtr(11);

/*
line: distance(cm), speed(rpm)
circ: angle, dir(left or right), radius(cm), speed(rpm)
loop; this will loop the path back to the start
*/
std::vector<std::string> path = {
	// exemple path
	"line: 190, 40",
	"circ: 93, right, 30, 40",
	"line: 220, 40",
	"circ: 160, left, 60, 40",
	"line: 100, 40",
	"circ: 75, right, 45, 40",
	"line: 120, 40",
	"circ: 180, left, 58, 40",
	"circ: 105, right, 50, 40",
	"line: 45, 40",
	"circ: 90, left, 60, 40",
	"line: 400, 40"};

// spesify the wheel circumference and the width of the robot in cm
float wheel_circumference = 10.3 * M_PI;
float robot_width = 34;

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize()
{
	pros::lcd::initialize();
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void opcontrol()
{
	autonomous();
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void autonomous()
{
	for (int i = 0; i < path.size(); i++)
	{
		// get the type of path
		std::string path_type = path[i].substr(0, 4);
		pros::lcd::set_text(1, path[i].substr(0, 4));

		if ("loop" == path_type)
		{

			i = -1;
		}
		if ("line" == path_type)
		{
			// get all the arguments for a line path
			std::string args = path[i].substr(5);
			int comma_pos = args.find(",");
			float distance = std::stoi(args.substr(0, comma_pos));
			float speed = std::stoi(args.substr(comma_pos + 1));

			// move the robot at the given speed in rpm
			left_mtr.move_velocity(speed);
			right_mtr.move_velocity(speed);

			// calculate the time it will take to move the given distance
			float time = (distance / wheel_circumference) / speed;
			pros::delay(time * 60000);
		}
		if ("circ" == path_type)
		{
			// get all the arguments for a circle path
			std::string args = path[i].substr(5);
			int comma_pos = args.find(",");
			float angle = std::stoi(args.substr(0, comma_pos));
			args = args.substr(comma_pos + 1);
			comma_pos = args.find(",");
			std::string dir = args.substr(0, comma_pos);
			args = args.substr(comma_pos + 1);
			comma_pos = args.find(",");
			float radius = std::stoi(args.substr(0, comma_pos));
			float speed = std::stoi(args.substr(comma_pos + 1));

			// figure out which way to turn the robot
			// and move the robot at the given speed in rpm with the correct ratio

			// the radius is calculated from the center of the robot
			// and the speed is calculated from the outer wheel
			// the inner wheel will move at the speed * (the radius of the inner wheels path) / (the radius of the outer wheels path)
			if ("left" == dir || " left" == dir)
			{
				left_mtr.move_velocity(speed);
				right_mtr.move_velocity(speed * (radius - (robot_width / 2)) / (radius + (robot_width / 2)));
			}
			if ("right" == dir || " right" == dir)
			{
				right_mtr.move_velocity(speed);
				left_mtr.move_velocity(speed * (radius - (robot_width / 2)) / (radius + (robot_width / 2)));
			}

			// calculate the time it will take for the outer wheel to move the given angle
			float time = (angle / 360) * (((2 * M_PI * (radius + (robot_width / 2))) / (wheel_circumference)) / speed) * 60;
			pros::delay(time * 1000);
		}
	}
	// stop the robot at the end of the path
	right_mtr.move_velocity(0);
	left_mtr.move_velocity(0);
}
