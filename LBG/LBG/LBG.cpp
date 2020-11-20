// LBG.cpp : Defines the entry point for the console application.
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
long double delta = 1.0;
long double epsilon = 0.03;
int codebook = 1; // Size of the codebook calculated.
vector<vector<int>> assignedRegions;

void Initialize(){
	codebook = 1;
	assignedRegions.clear();
	assignedRegions.resize(codebook);
	return;
}

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

long double EuclideanDistance(vector<long double> &currentRegion, vector<long double> &currentUniverse){
	long double ans = 0;
	for(int i=0;i<12;i++){
		ans += (currentRegion[i]-currentUniverse[i])*(currentRegion[i]-currentUniverse[i]);
	}
	return ans;
}

void AssignRegions(vector<vector<long double>> &universe, vector<vector<long double>> &regions){
	int size = universe.size();
	for(int i=0;i<size;i++){
		vector<long double> currentUniverse = universe[i];
		long double minDistance = LDBL_MAX;
		int index = 0;
		for(int j=0;j<codebook;j++){
			vector<long double> currentRegion = regions[j];
			long double distance = EuclideanDistance(currentRegion, currentUniverse);
			if(distance < minDistance){
				minDistance = distance;
				index = j;
			}
		}
		assignedRegions[index].push_back(i);
	}
	return;
}

long double TotalDistortion(vector<vector<long double>> &universe, vector<vector<long double>> &regions){
	long double tDistortion = 0;
	long double total = 0;
	for(int i=0;i<codebook;i++){
		int size = assignedRegions[i].size();
		total += (long double)size;
		for(int j=0;j<size;j++){
			int index = assignedRegions[i][j];
			tDistortion += EuclideanDistance(regions[i], universe[index]);
		}
	}
	long double average = tDistortion/total;
	return average;
}

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
			regions[i][j] = temp[j]/((long double)size);
		}
	}
	return;
}

vector<long double> LBG_Initialization(vector<vector<long double>> &universe){
	vector<long double> temp(12, 0.0);
	int size = universe.size();
	for(int i=0;i<size;i++){
		for(int j=0;j<12;j++){
			temp[j] += universe[i][j];
		}
	}
	for(int j=0;j<12;j++){
		temp[j] = temp[j]/((long double)size);
	}
	return temp;
}

void Lloyds_KMeans(vector<vector<long double>> &universe, vector<vector<long double>> &regions){
	assignedRegions.clear();
	assignedRegions.resize(codebook);
	long double oldDistortion = 0.0;
	AssignRegions(universe, regions);
	long double tDistortion = TotalDistortion(universe, regions);
	while(abs(tDistortion - oldDistortion) > delta){
		UpdateRegions(universe, regions);
		assignedRegions.clear();
		assignedRegions.resize(codebook);
		AssignRegions(universe, regions);
		oldDistortion = tDistortion;
		tDistortion = TotalDistortion(universe, regions);
	}
	return;
}
vector<vector<long double>> CreateCopy(vector<vector<long double>> regions){
	return regions;
}

void UpdateCodeBook(vector<vector<long double>> &regions, vector<vector<long double>> &temp){
	int size = temp.size();
	for(int i=0;i<size;i++){
		vector<long double> temp1;
		vector<long double> temp2;
		for(int j=0;j<12;j++){
			long double val1 = (temp[i][j] * (1 + epsilon));
			long double val2 = (temp[i][j] * (1 - epsilon));
			temp1.push_back(val1);
			temp2.push_back(val2);
		}
		regions.push_back(temp1);
		regions.push_back(temp2);
	}
	return;
}

void LBG(vector<vector<long double>> &universe, vector<vector<long double>> &regions){
	while(codebook <= 8){
		Lloyds_KMeans(universe, regions);
		if(codebook >= 8){
			return;
		}
		vector<vector<long double>> temp = CreateCopy(regions);
		regions.clear();
		codebook *= 2;
		UpdateCodeBook(regions, temp);
	}
	return;
}

int _tmain(int argc, _TCHAR* argv[]) {
	Initialize();

	string fileName = "Universe.csv";

	vector<vector<long double>> universe;
	vector<vector<long double>> regions;

	if(!ReadUniverse(universe, fileName)){
		return 0;
	}

	regions.push_back(LBG_Initialization(universe));

	LBG(universe, regions);

	PrintMachine(regions);

	return 0;
}





