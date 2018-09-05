// AllpassReverb.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "AudioFile.cpp"
#include <iostream>
#include <vector>
#include <string>
#include "AllpassReverbBased.h"
using namespace std;


int main()
{
	AudioFile<double> af;
	std::string current_dir = "D:\\Project\\Repository\\Reverb\\AllpassReverb\\AllpassReverb\\assets";
	std::string audioFile = current_dir + "\\first.wav";
	af.load(audioFile);
	af.printSummary();

	vector<double> left(af.samples[0]);
	vector<double> right(af.samples[1]);
	int fs = af.getSampleRate();
	int tap_ms = fs / 1000;
	cout << "ready to process" << endl;
	vector<vector<double>> output(2);				// Vector of vector must assign the size
	// process the left and right channel
	output[0] = sample_delay_line(tap_ms, af.getNumSamplesPerChannel(), left);
	output[1] = sample_delay_line(tap_ms, af.getNumSamplesPerChannel(), right);
	af.setAudioBuffer(output);

	cout << "ready to output" << endl;
	af.save(current_dir+"\\test_stereo2.wav");
	return 0;
}
