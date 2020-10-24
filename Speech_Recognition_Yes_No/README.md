# Algorithm Used In The Code
## Steps
	1. Store all the amplitudes in a vector by reading the audio.txt files.
	2. For first 16000 samples, is taken as idle state of the user, so using these amplitudes calculate the short term energy for noise and store the value.
	3. Take sum of all amplitudes after the first 16000 samples and average it over the number of samples, this is your amplitude offset.
	4. Subtract the value of amplitude offset from each amplitude as this is the offset provided by the noise and thus needs to be removed.
	5. Iterate through the samples and for every frame calculate its STE and ZCR count, where size of frame is equal to 300 samples.
	6. Now iterate through the frames finding the frame having short term energy greater than (some factor * noise energy), this is starting of the frame when user starts speaking.
	7. Now Iterate again to find similarly the ending of the user speech, ie. frame having short term energy less than (some factor * noise energy)
	8. Store the average ZCR count and average STE of this frame, it can either Yes Or No. Goto step 6(if more frames are left).
	9. As we know, 'Yes' will have more ZCR due to the presence of fricative 's' in it as compared to 'No'. Thus based on this value differentiate between them.

# Steps To Compile And Run 'Speech_Recognition_Yes_No' Project

	1. Open the project 'Speech_Recogntion_Yes_No' in Visual Studio 2010.
	2. Open the file 'Speech_Recognition_Yes_No.cpp'. 
	3. Build the project using 'Build Solution' under 'Build' option.
	4. Run the project using 'Start Without Debugging' under 'Debug' option.
	5. Terminal will open, and after configuring the sound hardware, recording starts.
	6. Record for 10 seconds, and also do give a silence of 1-2 seconds in the beginning.
	7. After recording when recording stops, Press Enter to get the results.
	
# Thank You