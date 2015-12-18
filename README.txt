==== GRIM DRUM ====

A grim-sounding digital drum machine

Oliver Reynolds, 2015 <oli_kester@zoho.com>

This program uses the Maximilian library, and openFrameworks with the 
ofxGui extension. It has six percussion voices, all of which are 
synthesised – no sample files used here. Each drum voice is selected by 
clicking its title, and then sequenced using the sixteen toggles at the 
bottom of the window. 

Appearance and functionality are loosely inspired by the Roland MC-505.

All synthesis parameters, sequences and the master tempo can be edited 
live with little latency and no glitches.

I have also provided visual feedback - the audio level meter responds 
to the master output, the step sequencer has LEDs to indicate the 
current step, and an LED flashes in time with the current tempo.

Interaction comes via the mouse, which is used to select drum voices, 
enter step sequences, and change parameters. The step sequencer and part 
mute toggles are also mapped to the keyboard, allowing for quick 
interaction and live performance possibilities. 

Due to the relative ease of adding new parameters to the ofxGuiGroup 
openFrameworks object, further control over the drum sounds could easily 
be added. Every parameter in the Maximilian percussion classes (and in 
my own custom classes) could be mapped to a new slider, or to the 
keyboard.

--- How to Run ---

1. Place the project folder inside the "apps/myApps" folder of a valid 
openFrameworks installation (I was using the 0.9.0 release). 

Make sure you have the ofxMaxim addon installed (it's also included in 
this folder)

2. Then we have two options, either - 

	- Use Qt Creator to open the .qbs project file. You should have it 
	setup according to this guide - 
	http://openframeworks.cc/setup/qtcreator/
	
	- *** N.B. - In order to get openFrameworks & Maximilian working 
	nicely with Qt Creator on my 64-bit Linux machine , I had to make 
	the following change to my of.qbs file *** - 
	
		Substitute 
	
		".concat(['-Wno-unused-parameter','-std=gnu++14'])"
	
		with
	
		".concat(['-Wno-unused-parameter','-std=gnu++11'])"
		
		on line ~434 of your of.qbs, located in - 
		
		OF_ROOT/libs/openFrameworksCompiled/project/qtcreator/modules/of/
	
	- Hit Run on your Desktop kit (this should first activate the build 
	step, then run automatically)

OR
	
	- Use the openFrameworks Project generator to port the project to 
	your current platform  - 
	http://openframeworks.cc/tutorials/introduction/002_projectGenerator.html
	
	This method might be tricky.	
	
3. If neither of these work, you can always copy the main.cpp, ofApp.h 
and ofApp.cpp resources into a new openframeworks project. Make sure it 
includes the ofxMaxim addon. 

Failing all else, there is always my 64-bit Linux binary in the bin/ 
folder, which you are welcome to try running.

If you encounter any problems, please contact me via the email above.
