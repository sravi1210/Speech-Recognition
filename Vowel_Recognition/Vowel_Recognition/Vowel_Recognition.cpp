// Vowel_Recognition.cpp : Defines the entry point for the console application.

// Included all header files required.
#include "stdafx.h"       
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>

using namespace std;

string normalized = "FALSE";                            // All global variables for storing the different parameters of speech recognition.
long double samples=0, bitsPerSample=16, channel=1, sampleRate=16000;  // Speech parameters.  
int frameSize = 320;        // Frame size of the frames taken.
int frameCount = 5; // Number of frames to be taken in one recording of the vowel.
long double scaleAmplitude = 500; // Amplitude to be scaled during normalization. 


bool ReadFile(vector<long double> &amplitude, string fileName){
	fstream Istream;        // File pointer for reading the information in the recording.
	Istream.open(fileName);    
	
	if(!Istream){                            // If the file is not found.
		cout<<"Failed to open the file "<<fileName<<endl;
		Istream.close();
		return false;
	}

	string word;
	 
	while(Istream >> word){                   // reading the file and storing the corresponding information of speech (Samples, Amplitudes, Bits Per Sample, Channels, Sample Rate, Normalization).
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
			amplitude.push_back(amp);
		}
	}
	Istream.close();
	return true;
}

bool WriteFile(vector<vector<long double>> &avgCIS, string fileName){
	ofstream Ostream(fileName);
	 
	if(!Ostream){                            // If the file is not found.
		cout<<"Failed to open the file "<<fileName<<endl;
		Ostream.close();
		return false;
	}

	int row = avgCIS.size();
	int col = avgCIS[0].size();
	for(int i=0;i<row;i++){
		for(int j=0;j<col;j++){
			Ostream<<avgCIS[i][j]<<" ";
		}
		Ostream<<endl;
	}
	return true;
}

void Normalization(vector<long double> &amplitude){
	long double amplitudeMax = 0;
	int size = amplitude.size();
	for(int i=0;i<size;i++){
		amplitudeMax = max(amplitudeMax, amplitude[i]);   // Calculate the maximum amplitude value from the given amplitudes array.
	}
	for(int i=0;i<size;i++){
		amplitude[i] /= amplitudeMax;        // Divide by maximum value and multiply to scale the amplitude.
		amplitude[i] *= scaleAmplitude;
	}
	return;
}

void DCShift(vector<long double> &amplitude){
	int size = amplitude.size();
	long double DCOffset = 0;
	for(int i=0;i<size;i++){          // Take the sum of all the amplitudes in the recording.
		DCOffset += amplitude[i];
	}
	DCOffset /= (long double)size;    // Take the average of the sum values.
	for(int i=0;i<size;i++){
		amplitude[i] -= DCOffset;  // Subtract the DCOffset from each of the amplitude values.
	}
	return;
}

bool Trim(vector<long double> &trimAmplitude, vector<long double> &amplitude){
	
	int size = trimAmplitude.size();
	return (size >= (frameSize*frameCount))?true:false;
}

void CalculateCIS(vector<long double> &CIS, vector<long double> &AIS){
	for(int i=1;i<=12;i++){
		long double sum = 0.0;
		for(int j=1;j<i;j++){
			sum += ((((long double)j)*CIS[j]*AIS[i-j])/(long double)i);
		}
		long double C = AIS[i] + sum;
		CIS.push_back(C);
	}
	return;
}

void CalculateAIS(vector<long double> &AIS, vector<long double> &RIS){
	vector<long double> Energy(13, 0.0);
	vector<long double> K(13, 0.0);
	vector<long double> alpha(13, 0.0);
	vector<long double> prevAlpha(13, 0.0);
	Energy[0] = RIS[0];
	for(int i=1;i<=12;i++){
		if(i == 1){
			K[i] = RIS[i]/RIS[0];
		}
		else{
			long double sum = 0.0;
			for(int j=1;j<i;j++){
				prevAlpha[j] = alpha[j];
				sum += (prevAlpha[j]*RIS[i-j]);
			}
			K[i] = (RIS[i] - sum)/Energy[i-1];
		}
		alpha[i] = K[i];
		for(int j=1;j<i;j++){
			alpha[j] = prevAlpha[j] - (K[i]*prevAlpha[i-j]);
		}
		Energy[i] = (1 - (K[i] * K[i])) * Energy[i-1];
	}
	for(int i=1;i<=12;i++){
		AIS[i] = alpha[i];
	}
	return;
}

vector<long double> CalculateRIS(vector<long double> &trimAmplitude, int start, int end){
	vector<long double> RIS;
	vector<long double> AIS(13, 0.0);
	vector<long double> CIS;

	long double N = (long double)frameSize;
	for(int i=0;i<=12;i++){
		long double sum = 0.0;
		for(int j=start;j+i<end;j++){
			sum += (trimAmplitude[i] * trimAmplitude[i+j]);
		}
		sum /= N;
		RIS.push_back(sum);
	}
	CalculateAIS(AIS, RIS);
	CIS.push_back(logl(RIS[0])); // First cepstral coefficient is logarithm of the gain term of LPC model.
	CalculateCIS(CIS, AIS);
	return CIS;
}


int _tmain(int argc, _TCHAR* argv[]){

	string folder = ".\\Recordings\\Sample\\";  // Folder containing the sample and test recording.
	string fileName = "";
	string extension = ".txt";      // Extension of the file used.
	char vowels[5] = {'a', 'e', 'i', 'o', 'u'};   // Vowels recorded and recognized.
	string rollNo = "170101053";  // Roll no.
	string underScore = "_";      // Underscore.

	for(int i=0;i<5;i++){
		vector<vector<long double>> avgCIS(5, vector<long double>(12, 0.0));   // Average C'i values of each frame of all recordings of each vowel.
		vector<vector<long double>> allCIS;		// All C'i values of each recording and frame.

		for(long long int j=1;j<=10;j++){
			fileName = folder + rollNo + underScore + vowels[i] + underScore + to_string(j) + extension;
			vector<long double> amplitude;
			if(!ReadFile(amplitude, fileName)){
				return 0;
			}
			DCShift(amplitude);
			Normalization(amplitude);
			vector<long double> trimAmplitude;
			if(!Trim(trimAmplitude, amplitude)){
				cout<<"Recording " << fileName << " is too small"<<endl;
				return 0;
			}
			for(int k=0;k<frameCount;k++){
				int start = (frameSize * k);
				int end = (frameSize * (k+1));
				vector<long double> CIS = CalculateRIS(trimAmplitude, start, end);
				allCIS.push_back(CIS);
			}
		}

		for(int j=0;j<(frameCount*10);j++){
			for(int k=0;k<12;k++){
				int x = j%frameCount;
				int y = k;
				avgCIS[x][y] += allCIS[j][k]; 
			}
		}

		for(int j=0;j<frameCount;j++){
			for(int k=0;k<12;k++){
				avgCIS[j][k] /= 10.0;
			}
		}
		
		string outputfileName = folder + vowels[i] + extension;
		WriteFile(avgCIS, outputfileName);

	}

	return 0;
}

