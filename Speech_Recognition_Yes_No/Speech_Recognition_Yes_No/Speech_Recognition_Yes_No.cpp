// Speech_Recognition_Yes_No.cpp : Defines the entry point for the console application.

// Included All Header Files Required.
#include "stdafx.h"       
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>

using namespace std;

string normalized = "FALSE";                            // All Global Variables For Storing The Different Parameters Of Speech Recognition.
long double samples=0, bitsPerSample=16, channel=1, sampleRate=16000;  // Speech Parameters.
long double amp_normalized = 1000;                // Normalization Factor For The Amplitudes.
long double noiseDensity;                       // Noise Density - Short Term Energy For First One Second.   
int frameSize = 3000;                           // Frame Size - Consider The Size Of Samples Taken At A Time.
long double STE_JumpFactor = 5;              // STE Jump From Noise To Actual Sound.
long double ZCR_Distinction = 0.085;             // Distinction Between ZCR Value Of Yes/No Of User.

// Main Function.
int _tmain(int argc, _TCHAR* argv[]) {
	
	string fileName = "";

	/*
		// For Two Different Test Cases

		cout<<"Which Test Case You Want To Test Audio Files (ENTER) :- 1 (For Ravi's Voice) OR 2 (For Anubhav's Voice) OR 3 (For Sir's Voice) ? "<<endl;
		int value;
		cin>>value;
		if(value!=1 && value!=2 && value!=3){            
			cout<<"Wrong Value Given. Run Again!"<<endl;
			return 0;
		}
		else if(value == 1){
			fileName = "Ravi_Yes_No.txt";
			STE_JumpFactor = 5;
			ZCR_Distinction = 0.085;
		}
		else if(value == 2){
			fileName = "Anubhav_Yes_No.txt";
			STE_JumpFactor = 0.8;
			ZCR_Distinction = 0.12;
		}
		else if(value == 3){
			fileName = "Sir_Yes_No.txt";
			STE_JumpFactor = 5;
			ZCR_Distinction = 0.20;
		}
	*/

	system("Recording_Module.exe 10 input_file.wav input_file.txt");

	fileName = "input_file.txt";

	fstream Istream;        // File Pointer For Reading The Information In Recording Yes/No.
	Istream.open(fileName);    
	ofstream Ostream("Result.txt");       // Results Stores In The Result.txt file.
	
	if(!Istream){                            // If File Is Not Found.
		cout<<"Failed to open the file "<<fileName<<endl;
		return 0;
	}

	string word;
	long double DCOffset = 0;
	vector<long double> amplitude;            // Vector To Store The Amplitudes Of Each Sample.
	 
	while(Istream >> word){                   // Reading The Yes_No.txt File and Storing The Corresponding Information Of Speech (Samples, Amplitudes, Bits Per Sample, Channels, Sample Rate, Normalization).
		if(word == "SAMPLES:"){
			Istream >> word;
			samples = (long double)stoi(word);
		}
		else if(word == "BITSPERSAMPLE:"){
			Istream >> word;
			bitsPerSample = (long double)stoi(word);
		}
		else if(word == "CHANNELS:"){
			Istream >> word;
			channel = (long double)stoi(word);
		}
		else if(word == "SAMPLERATE:"){
			Istream >> word;
			sampleRate = (long double)stoi(word);
		}
		else if(word == "NORMALIZED:"){
			Istream >> word;
			normalized = word;
		}
		else{
			samples++;
			long double amp = (long double)stoi(word);
			amp = amp/amp_normalized;                  // Normalizing All The Amplitudes For Better Store.
			DCOffset = DCOffset + amp;
			amplitude.push_back(amp);
		}
	}

	int size = amplitude.size();
	DCOffset = DCOffset/samples;             // DCOffset To Cancel The Noise Amp Present In Idle System.
	noiseDensity = 0;

	for(int i=0;i<16000;i++){              // Looping For Storing The Noise Density For First One Second Of Idle Machine ie. 16000 Samples. 
		amplitude[i] -= DCOffset;
		noiseDensity += (amplitude[i] * amplitude[i]);		// Storing The Noise Density For First One Second.
	}

	vector<int> ZCRCount;                 // Vector To Store ZCR Count For Each Frame.
	vector<long double> STEnergy;		  // Vector To Store Short Term Energy For Each Frame.

	for(int i=16000;i<size;i++){             // Looping For Other Samples.
		int zeroCount = 0;                 // ZCR For Each Frame.
		long double energyDensity = 0;        // Short Term Energy For Each Frame.

		for(int j=i;(j<i+frameSize && j<size);j++){       // Looping Through Each Frame, Calculate ZCR And STE Accordingly.
			amplitude[j]-= DCOffset;
			if(amplitude[j] == 0){
				zeroCount++;
			}
			else if(j-1 >= i){
				long double a = amplitude[j-1];
				long double b = amplitude[j];
				if((a>0 && b<0) || (a<0 && b>0)){
					zeroCount++;
				}
			}
			energyDensity += (amplitude[j] * amplitude[j]);         
		}
		
		STEnergy.push_back(energyDensity); 
		ZCRCount.push_back(zeroCount);
		int j = min(i+frameSize, size);      // Incrementing i Accordingly With The Value Of Minimum(i+FrameSize, Size).
		i = j-1;
	}

	size = ZCRCount.size();

	vector<long double> SoundSTE;
	vector<long double> TotalZCR;

	int start = -1, end =- 1;

	for(int i=0; i<size;i++){             // Detecting When The User Speaks, When The STE Becomes Greater Than (STE_JumpFactor * Noise Density). 
		long double density = STEnergy[i]; 
		if(density >= noiseDensity * STE_JumpFactor){
			start = i;
			for(int j=i+1;j<size;j++){
				end = j-1;
				if(STEnergy[j] < noiseDensity * STE_JumpFactor){        // Detect The End Of The Word Yes/No In Speech.
					end = j-1;
					i = j-1;
					goto here;
				}
			}
		}
		here:   
		long double energy = 0;
		int count = 0;
		if(start != -1 && end != -1){
			for(int j=start;j<=end;j++){               // Looping Through Start To End And Calculating The STE And ZCR Value Of The Word User Spoke.
				count += ZCRCount[j];
				energy += STEnergy[j];
			}
			TotalZCR.push_back((long double)count/(long double)((end-start+1)*frameSize));         // Final ZCR Value Of The Word Spoken.
			SoundSTE.push_back(energy/((end-start+1)*frameSize));       // Final STE Energy Value Of The Word Spoken.
			start = -1;
			end = -1;
		}
	}

	size = TotalZCR.size();

	// Printing All Important Data Of Speech In Results.txt

	Ostream<<"Speech Details Of The User:"<<endl;        // Speech Details Of The User.
	Ostream<<"Number Of Samples Taken: "<<samples<<endl;
	Ostream<<"Bits Per Sample: "<<bitsPerSample<<endl;
	Ostream<<"Number Of Channel: "<<channel<<endl;
	Ostream<<"Normalization: "<<"Done With The Amplitude Factor - "<<amp_normalized<<endl;
	Ostream<<"STE Noise: "<<(noiseDensity/16000)<<endl<<endl;
	
	Ostream<<"User Spoke:"<<endl;
	for(int i=0;i<size;i++){          // Write "Yes/No" In The File According To The ZCR Values And ZCR_Distinction.
		if(TotalZCR[i] >= ZCR_Distinction){
			Ostream<<"Yes ";
			cout<<"Yes ";
		}
		else{
			Ostream<<"No ";
			cout<<"No ";
		}
	}
	cout<<endl;
	Ostream<<endl<<endl;

	Ostream<<"Details Of Each Word: "<<endl;
	for(int i=0;i<size;i++){                    // Details Of Each Word.
		if(TotalZCR[i] >= ZCR_Distinction){
			Ostream<<i+1<<". Yes:"<<endl;
			Ostream<<"	Average ZCR: "<<TotalZCR[i]<<endl;
			Ostream<<"	Average STE: "<<SoundSTE[i]<<endl;
		}
		else{
			Ostream<<i+1<<". No:"<<endl;
			Ostream<<"	Average ZCR: "<<TotalZCR[i]<<endl;
			Ostream<<"	Average STE: "<<SoundSTE[i]<<endl;
		}
	}
	cout<<"For More Details Open Result.txt To Watch Speech Details And Recognition"<<endl;
	Istream.close();
	Ostream.close();
	samples = 0;
	return 0;
}

