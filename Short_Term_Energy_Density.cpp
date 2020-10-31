#include <bits/stdc++.h>
#include "AudioFile.h"

using namespace std;

int main(){
	AudioFile<double> audioFile;
	audioFile.load("./170101053_VowelRec.wav");
	
	int sampleRate = audioFile.getSampleRate();

	int channel = audioFile.getNumChannels();
	int numSamples = audioFile.getNumSamplesPerChannel();

	double sum = 0.0;

	audioFile.printSummary();

	for(int i=0;i<numSamples;i++){
		double currentSample = audioFile.samples[channel-1][i];
		sum += (currentSample * currentSample);
	}

	double L = audioFile.getLengthInSeconds();

	double short_term_energy_density = sum/L;

	cout<<"Short Term Energy Density: "<<fixed<<setprecision(10)<<short_term_energy_density<<endl;

	return 0;
}
