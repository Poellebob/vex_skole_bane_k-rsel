#include "main.h"
#include "vector"
// #include "math.h"
//  hvis den fejler upload pårund af permistions så prøv
//  sudo chmod 666 <port>

pros::Motor left_mtr(-1);
pros::Motor right_mtr(2);

/*
line: distance(cm), speed(rpm)
circ: angle, dir(left or right), radius(cm), speed(rpm)
loop
*/
std::vector<std::string> path = {
	"line: 100, 50",
	"circ: 180, right, 50, 50",
	"loop"};

float wheel_circumference = 10.3 * M_PI;
int robot_width = 29;

/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button()
{
	static bool pressed = false;
	pressed = !pressed;
	if (pressed)
	{
		pros::lcd::set_text(2, "I was pressed!");
	}
	else
	{
		pros::lcd::clear_line(2);
	}
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize()
{
	pros::lcd::initialize();
	pros::lcd::set_text(1, "Hello PROS User!");

	pros::lcd::register_btn1_cb(on_center_button);
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
void autonomous() {}

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
void opcontrol()
{
	for (int i = 0; i < path.size(); i++)
	{
		std::string path_type = path[i].substr(0, 4);
		pros::lcd::set_text(1, path[i].substr(0, 4));

		if ("loop" == path_type)
		{
			i = 0;
		}
		if ("line" == path_type)
		{
			std::string args = path[i].substr(5);
			int comma_pos = args.find(",");
			float distance = std::stoi(args.substr(0, comma_pos));
			float speed = std::stoi(args.substr(comma_pos + 1));
			left_mtr.move_velocity(speed);
			right_mtr.move_velocity(speed);
			float time = (distance / wheel_circumference) / speed;
			pros::delay(time * 60000);
		}
		if ("circ" == path_type)
		{
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
			float time = (angle / 360) * (((2 * M_PI * radius) / (wheel_circumference)) / speed) * 58;
			pros::delay(time * 1000);
		}
	}
	right_mtr.move_velocity(0);
	left_mtr.move_velocity(0);
}
