Dylan Kirkby

INFO6028 – Graphics
Midterm Exam – Wednesday, October 30th, 2019

Questions answered in project AmethystEngine.

AmethystEngine by:
	Dylan Kirkby,
	Brian Cowan,
	Ivan Parkhomenko

Created in Visual Studio Community 2019
Runs in x64 only, Release recommended for performance.


Movement controls are relative to the camera's current angle. 
	1	Set Camera to skull view, locks target to skull.
	2 	Set camera to Beach Cave / Flashlight View. Locks camera to cave.
	3	Set Camera to overhead View of Island, locks target to center 0,0,0.
	4	Set Camera to 'drone' - circular panning of the island.
	5	Set Camera to view fireflies, locking to the cave behind the mangrove tree for a better starting angle.
	0	Unlock camera target.
	
	F	Toggles firefly movement.
	T	Toggle Time of Day
	R	Toggle flickering skull lights
	
	W	Move Camera Forwards
	S	Move Camera Backwards
	A	Move Camera Left
	D	Move Camera Right
	Q	Move Camera Up
	E	Move Camera Down
Mouse	Adjusts Camera viewing angle when unlocked


ALL QUESTIONS IN ONE SOLUTION
This documents gives functions names and lines to where the questions are answered.
No changes to code necessary.


Q1:
Generated with seed 112 and placed at 0,0,0 after converting.

Q2:
Default Camera view set so you can see the island
Press T once on load to view the scene in the pre Q3 state.
External model information can be found in AmethystEngine\data\config\models\models_GRAPHICS_MIDTERM.ini

Q3:
Default state the program loads into.
Press T to get back to this if it isn't already night time.
External light information can be found in AmethystEngine\data\config\lights\lights_GRAPHICS_MIDTERM.ini

Q4:
Keys 1 - 3 as instructed, found at cKeyBindings key_callback() ln 71 - 92

Q5:
Done using a c++11 random_device, Mersenne Twister, and uniform_int_distribution with some heavy divison.
Randomizers declared in main at 49 - 52.
Operation happens at main() 293-297.
Toggled with the R Key.

Q6:
Found at main() 299 - 310
Press 4 to activate.

Q7:
Press 5 to view mangrov e01 near a water based cave.
Fireflies are static by default in their spawn locations.
Press F to activate their pseudo random movement around the tree.
Movement code found at main() 312 - 341
External information at AmethystEngine\data\config\complex\complex_GRAPHICS_MIDTERM.ini
as well as in the proceeding .ini files.