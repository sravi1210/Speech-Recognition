// K_Means.cpp : Defines the entry point for the console application.
//

// Included all header files required.
#include "stdafx.h"       
#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <map>
#include <numeric>

using namespace std;

// Global Parameters.
long double delta = 1;   // Delta value for distortion calculations.
int codebook = 8; // Size of the codebook calculated.
vector<vector<int>> assignedRegions;

// Function to initialize the global values and parameters.
void Initialize(){
	assignedRegions.clear();
	assignedRegions.resize(codebook);
	return;
}

// Function to print the 2D-matrix given.
void PrintMachine(vector<vector<long double>> &dp){
	int row = dp.size();
	if(row == 0){
		return;
	}
	int col = dp[0].size();
	for(int i=0;i<row;i++){
		for(int j=0;j<col;j++){
			cout<<dp[i][j]<<" ";
		}
		cout<<endl;
	}
	return;
}

// Function to read the values from the given fileName.
bool ReadUniverse(vector<vector<long double>> &universe, string fileName){
	fstream Istream;        // File pointer for reading the information in the recording.
	Istream.open(fileName);    

	if(!Istream){                            // If the file is not found.
		cout<<"Failed to open the file "<<fileName<<endl;
		Istream.close();
		return false;
	}
	string word;
	while(Istream >> word){       // When the input can be read.
		vector<long double> temp;
		long double tempCI = stold(word);
		temp.push_back(tempCI);

		for(int i=1;i<=11;i++){      // One row contains 12 C'i values.
			Istream >> word;
			long double tempCI = stold(word);
			temp.push_back(tempCI);
		}
		universe.push_back(temp);    // Fill the universe array.
	}

	Istream.close();
	return true;
}

// Function to calculate the euclidean distance in the current-region with the current-universe.
long double EuclideanDistance(vector<long double> &currentRegion, vector<long double> &currentUniverse){
	long double ans = 0;
	for(int i=0;i<12;i++){
		ans += (currentRegion[i]-currentUniverse[i])*(currentRegion[i]-currentUniverse[i]);  // Using formula for euclidean distance = (a[i]-b[i]) ** 2
	}
	return ans;
}

// Function to assign clusters to vectors in the universe set, by calculating minimum euclidean distance.
void AssignRegions(vector<vector<long double>> &universe, vector<vector<long double>> &regions){
	int size = universe.size();
	for(int i=0;i<size;i++){
		vector<long double> currentUniverse = universe[i];   
		long double minDistance = LDBL_MAX;
		int index = 0;
		for(int j=0;j<codebook;j++){
			vector<long double> currentRegion = regions[j];
			long double distance = EuclideanDistance(currentRegion, currentUniverse);  // Calculate euclidean distance between the current-region and the current-universe.
			if(distance < minDistance){       // If current-region distance is less, than update.
				minDistance = distance;
				index = j;
			}
		}
		assignedRegions[index].push_back(i);  // Fill the assigned region for each entry.
	}
	return;
}

// Function to calculate the average total distortion for a given universe and codebook regions.
long double TotalDistortion(vector<vector<long double>> &universe, vector<vector<long double>> &regions){
	long double tDistortion = 0;
	long double total = 0;
	for(int i=0;i<codebook;i++){
		int size = assignedRegions[i].size();
		total += (long double)size;
		for(int j=0;j<size;j++){
			int index = assignedRegions[i][j];
			tDistortion += EuclideanDistance(regions[i], universe[index]);       // Use the formula for the total distortion = sum of (euclidean distances between regions and universe vectors).
		}
	}
	long double average = tDistortion/total;    // Average distortion.
	return average;
}

// Function to update regions for the next iterations in the K-Means algorithm.
void UpdateRegions(vector<vector<long double>> &universe, vector<vector<long double>> &regions){
	for(int i=0;i<codebook;i++){
		vector<long double> temp(12, 0.0);
		int size = assignedRegions[i].size();
		for(int j=0;j<size;j++){
			int index = assignedRegions[i][j];
			for(int k=0;k<12;k++){
				temp[k] += universe[index][k];
			}
		}
		for(int j=0;j<12;j++){
			regions[i][j] = temp[j]/((long double)size);            // Calculate the centroid and update the region vector values.
		}
	}
	return;
}

// Function to handle K-Means algorithms.
void Lloyds_KMeans(vector<vector<long double>> &universe, vector<vector<long double>> &regions){
	long double oldDistortion = 0;   // Previous iteration distortion.
	AssignRegions(universe, regions);
	long double tDistortion = TotalDistortion(universe, regions);
	while(abs(tDistortion - oldDistortion) > delta){         // While absolute value of the difference between previous distortion and current distortion is greater than delta, then loop.
		UpdateRegions(universe, regions);
		assignedRegions.clear();
		assignedRegions.resize(codebook);         // Update assigned regions for each entry in the universe.
		AssignRegions(universe, regions);
		oldDistortion = tDistortion;
		tDistortion = TotalDistortion(universe, regions);
	}
	return;
}

// Main Function.
int _tmain(int argc, _TCHAR* argv[]) {
	Initialize();   // Basic Initilization.

	string fileName = "Universe.csv";        // FileName to be read for universe.

	vector<vector<long double>> universe;        // Data Structure to hold all vectors in the universe.
	vector<vector<long double>> regions;         // Data Structure to hold all the codebook vectors.

	if(!ReadUniverse(universe, fileName)){        // Read the contents of the universe file.
		return 0;
	}

	vector<int> randomVectors(codebook);
	iota(randomVectors.begin(), randomVectors.end(), 0);

	for(int i=0;i<codebook;i++){
		int index = randomVectors[i];
		regions.push_back(universe[index]);
	}

	Lloyds_KMeans(universe, regions);         // Run K-Means algorithm.

	PrintMachine(regions);

	return 0;
}