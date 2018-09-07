// AllpassReverb.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "AudioFile.cpp"
#include <iostream>
#include <vector>
#include <string>
#include "AllpassReverbBased.h"
using namespace std;

vector<double> delay_line_mode(int tap_ms, int total_samples, vector<double> &data_in);
int sampleDelayLineBasedAllpass(double &data_in, double &data_out, double  gain);

int print_vec(vector<double> to_pring) {	// for test
	cout << "he data is:" << endl;
	for (auto data : to_pring) {
		cout << data << endl;
	}
	return 0;
}
int main()
{
	
	AudioFile<double> af;
	// The location of the .wav file
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
	// 原有的实现方式，由于理解有误导致最终的实现不符合预期
	//output[0] = sample_delay_line(tap_ms, af.getNumSamplesPerChannel(), left);
	//output[1] = sample_delay_line(tap_ms, af.getNumSamplesPerChannel(), right);


	// 测试 sampleDelayLineBasedAllpass
	output[0] = delay_line_mode(tap_ms, af.getNumSamplesPerChannel(), left);
	output[1] = delay_line_mode(tap_ms, af.getNumSamplesPerChannel(), right);

	af.setAudioBuffer(output);

	cout << "ready to output" << endl;
	af.save(current_dir+"\\test_stereo_single_delay_line_t4.wav");

	// 测试 sampleDelayLineBasedAllpass

	// test 01 
	//vector<double> data_test(40,0);		// 20 个0
	//int nn = 0;
	//for (auto &data : data_test) {
	//	data = ++nn;
	//}
	//vector<double> tt = delay_line_mode(1, 40, data_test);	// 输入1-20，进行测试

	//cout << "The output is :" << endl;
	//for (auto data : tt) {
	//	cout << data << " ";
	//}
	// test 01 



	




	return 0;
}

vector<double> delay_line_mode(int tap_ms, int total_samples, vector<double> &data_in)
{
	// use a single delay line to implement the sample delay line demo - pass 2018年9月7日18:53:32
	
	int delay_len = 110 * tap_ms;
	//int delay_len = 10 * tap_ms;	// 测试时 delay_line设置为10
	double * delay_line = new double[delay_len];	// initialize the delay line  // 不能直接用110
	int aa = 0;
	for (int count = 0; count != delay_len; ++count) {
		delay_line[count] = 0;
	}
	int head = 0;			// the next position to push data in
	int tail = 0;
	vector<double> out;
	for (int n = 0; n != total_samples; ++n) {



		head = ( (head+1)*tap_ms ) % delay_len;	
		out.push_back(delay_line[head]);			// pop()
		delay_line[head] = data_in[n];				// 在push 数据进line的时候，需要加上 pop该位置的数据的操作（循环队列为满的时候）

		// 根据不同的 reverb ，设计不同的处理方案，在以下位置修改
		sampleDelayLineBasedAllpass( delay_line[(head + 25 * tap_ms) % delay_len], delay_line[(head + 75 * tap_ms) % delay_len], 0.50);
		sampleDelayLineBasedAllpass( delay_line[(head + 40 * tap_ms) % delay_len], delay_line[(head + 60 * tap_ms) % delay_len], 0.30);
		sampleDelayLineBasedAllpass( delay_line[(head + 80 * tap_ms) % delay_len], delay_line[tail], 0.70);
		
		// ↑
		tail = head;
	}

	return out;

	/*
	- 待删除-
		for (int n = 0; n != total_samples; ++n) {
		head = ( (head+1)*tap_ms ) % delay_len;
		delay_line[( head + 0*tap_ms)%delay_len] = data_in[n];		// push data in 
		delay_line[(head + 75*tap_ms) % delay_len] = delay_line[(head + (75-50)*tap_ms) % delay_len] * 0.5 + delay_line[((head + (75 - 20))*tap_ms) % delay_len] * 0.3;
		delay_line[tail] = delay_line[(tail-30*tap_ms) % delay_len]*0.7 + delay_line[tail];
		out.push_back(delay_line[tail]);
		tail = head;
	}
	
	*/
}

int sampleDelayLineBasedAllpass(double & data_in, double & data_out, double gain)
{
	// -已解决- 是不是这个 函数引用的形式有问题，导致最后的输出基本没有呢？ 2018年9月6日18:22:15
	// 使用 & 才能对传进来的

	// The implement of the sample delay line based allpass filter.
	// H_z = (G_z - g ) / (1 - g * G_z), G_z is the delay_line.
	// The length of delay line is the distance between "data_in" and "data_out".
	double temp = data_in * (-gain) + data_out;
	data_in = data_in + gain * temp;
	data_out = temp;
	return 0;
}
