#include "opencv2/core/core.hpp"
#include "opencv2/flann/miniflann.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/photo/photo.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/contrib/contrib.hpp"

#include "opencv2/core/core_c.h"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/imgproc/imgproc_c.h"

#include <iostream>
#include <fstream>
#include <cstdio>
#include <ctime>
#include <windows.h>

using namespace std;
using namespace cv;

int main(int argc, char* argv[])
{
	bool isRunning = true;
	String command = "";
	ofstream record;
	VideoCapture cap;
	String fileName;
	String fileType;
	ofstream signalChange;
	bool everySecond;
	int fps = 0;
	int x = 0;
	int y2 = 0;
	int y3 = 0;
	int y1 = 0;


	while (isRunning){
		cout << "Please select the video clips for traffic light detection\n\nType 'e' for exit\n" << endl;
		cout << "Type '1' for the first sample: cloudy morning\n" << endl;
		cout << "Type '2' for the second sample: sunny afternoon\n" << endl;
		cout << "Type '3' for the third sample: sunny morning\n" << endl;
		cout << "Type '4' for the forth sample: another sunny afternoon...not working so well\n" << endl;
		cout << "Type '5' for the fifth sample: the newest sunny morning" << endl;

		cin >> command;

		
		if ((command.compare("e") == 0)||(command.compare("E") == 0)){
			isRunning = false;
		}
		//11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111
		//Play the cloudy day video about EE238_Cam
		bool cReadable = true;
		bool notPressEsc = true;
		if (command.compare("1") == 0){
			x = 680+4;
			y1 = 211+4;
			while (cReadable){ //If not readable, loop back
				SetCurrentDirectoryA("D:\\\ECE698_Proj\\\EE238_Cam\\");
				fileName = "0325_detected";
				fileType = ".avi";
				cap.open(fileName + fileType);
				if (!cap.isOpened())  // if not success, exit program 
				{
					cout << "Cannot open the video cam" << endl;
				}
				else{
					cReadable = false;
				}
			}
			fps = cap.get(CV_CAP_PROP_FPS); //get the frames per seconds of the video
		}
		if (command.compare("2") == 0){
			x = 680 + 2;
			y1 = 199 + 3;
			while (cReadable){ //If not readable, loop back
				SetCurrentDirectoryA("D:\\\ECE698_Proj\\\EE238_Cam\\");
				fileName = "manual_20160418152559";
				fileType = ".avi";
				cap.open(fileName + fileType);
				if (!cap.isOpened())  // if not success, exit program 
				{
					cout << "Cannot open the video cam" << endl;
				}
				else{
					cReadable = false;
				}
			}
			fps = cap.get(CV_CAP_PROP_FPS); //get the frames per seconds of the video
		}
		if (command.compare("3") == 0){
			x = 739;
			y1 = 310;
			while (cReadable){ //If not readable, loop back
				SetCurrentDirectoryA("D:\\\ECE698_Proj\\\EE238_Cam\\");
				fileName = "3";
				fileType = ".avi";
				cap.open(fileName + fileType);
				if (!cap.isOpened())  // if not success, exit program 
				{
					cout << "Cannot open the video cam" << endl;
				}
				else{
					cReadable = false;
				}
			}
			fps = cap.get(CV_CAP_PROP_FPS); //get the frames per seconds of the video
		}
		if (command.compare("4") == 0){
			x = 656;
			y1 = 207;
			while (cReadable){ //If not readable, loop back
				SetCurrentDirectoryA("D:\\\ECE698_Proj\\\EE238_Cam\\");
				fileName = "20160318123914";
				fileType = ".avi";
				cap.open(fileName + fileType);
				if (!cap.isOpened())  // if not success, exit program 
				{
					cout << "Cannot open the video cam" << endl;
				}
				else{
					cReadable = false;
				}
			}
			fps = cap.get(CV_CAP_PROP_FPS); //get the frames per seconds of the video
		}
		if (command.compare("5") == 0){
			x = 682;
			y1 = 202;
			while (cReadable){ //If not readable, loop back
				SetCurrentDirectoryA("D:\\\ECE698_Proj\\\EE238_Cam\\");
				fileName = "schedule_20160418_111214";
				fileType = ".avi";
				cap.open(fileName + fileType);
				if (!cap.isOpened())  // if not success, exit program 
				{
					cout << "Cannot open the video cam" << endl;
				}
				else{
					cReadable = false;
				}
			}
			fps = 30; //get the frames per seconds of the video

		}
		record.open("traffic_record_" + fileName + ".txt");
		signalChange.open("tmp.txt");
			int delay = 1;
			cout << "Traffic Light Display from EE238\n\nFrame per seconds :\t" << fps << endl;

			namedWindow(command, CV_WINDOW_AUTOSIZE);

			Mat frame;
			int counter = 0;
			int framenum = 0;
			double previousStdG = 0;
			double previousStdY = 0;
			double previousStdR = 0;
			
			//Sequence:
			//First[3]: Green Light, Yellow Light, Red Light
			//Second[3]: B, G, R
			double previous [3][3];

			//Avoid multiple (red) signal
			//Theoretically, the signal sequence is 1-2-3-1(cycle)
			//Present signal status: Green-1, Yellow-2, Red-3
			int presentSignal = 1; //Default Value-Green should be the very beginning signal.

			record << "\tGreen\t\t\t" << "\tYellow\t\t\t" << "\tRed" << endl;
			record << "Second" << "\tB\tG\tR\tSTD" << "\tB\tG\tR\tSTD"<< "\tB\tG\tR\tSTD" << endl;

			while (notPressEsc){
				bool bSuccess = cap.read(frame); // read a new frame from video
				if (!bSuccess) //if not success, break loop
				{
					cout << "No more frame from video file\n" << endl;
					record << "Finished" << endl;
					break;
				}

				//Left top point of traffic light
				//int x = 680;
				//int y1 = 211;
				y2 = y1 + 8;
				y3 = y2 + 8;

				moveWindow("EE238_Cam", 0, 0);
				imshow("ee238_01", frame);
				namedWindow("ee238_01", CV_WINDOW_AUTOSIZE);

				counter++;
				if (counter % fps == 0){
					framenum++;
					cout << framenum << " sec" << endl;
				}
				everySecond = (counter % fps == 0);
				waitKey(delay);


				//Here are three parts of light:Red, Green, Blue
				Rect r(x, y1, 4, 4); //ROI of red light
				Rect y(x, y2, 4, 4); //ROI of yellow light
				Rect g(x, y3, 4, 4); //ROI of green light
				Size enlarge2(100, 100);
				Mat rr;
				Mat yy;
				Mat gg;
				Mat red;
				Mat yellow;
				Mat green;
				
				//04/22/2016 Notes
				//Next Target: Calculate the dynamic fluctruation rate about B,G,R components,
				//then apply Gaussian model, if abs(diff) > 2 * STD---signal changed.
				//Test-able: When diff(R)>=std---Red Signal?
				
				//Question: Is the color component variance qualifies Gaussian distribution?

				//Green light
				frame(g).copyTo(gg);
				resize(gg, green, enlarge2);//resize image
				moveWindow("green", 1175, 200);
				imshow("green", green);
				namedWindow("green", CV_WINDOW_AUTOSIZE);
				Scalar avgPixelIntensityG = mean(gg);
				if (everySecond){
					int cb = (int)avgPixelIntensityG.val[0];
					int cg = (int)avgPixelIntensityG.val[1];
					int cr = (int)avgPixelIntensityG.val[2];
					double m = (cb + cg + cr) / 3;
					double std = sqrt(((cb )*(cb ) + (cg )*(cg ) + (cr )*(cr )) / 3);
					bool greenRule = (framenum > 1) && (presentSignal != 1) && ((cb - previous[0][0] > previousStdG) && (cg - previous[0][1] > previousStdG));
					if (greenRule){
						//if ((framenum > 1) && ((std - previousStdG)>previousStdG)){
						presentSignal = 1;
						cout << "\n\n\t\tGreen light signal!\n" << endl;
						signalChange << "Green light signal: " << framenum << "th Sec\n";
					}
					previous[0][0] = cb;
					previous[0][1] = cg;
					previous[0][2] = cr;
					previousStdG = std;
					cout << "Green:\t" << cb << "\t " << cg << "\t " << cr << "\t " << (int)std;
					record << framenum << "\t" << cb << "\t " << cg << "\t " << cr << "\t " << (int)std;

				}
				waitKey(delay);

				//Yellow Light
				frame(y).copyTo(yy);
				resize(yy, yellow, enlarge2);//resize image
				moveWindow("yellow", 1175, 100);
				imshow("yellow", yellow);
				namedWindow("yellow", CV_WINDOW_AUTOSIZE);
				Scalar avgPixelIntensityY = mean(yy);
				if (everySecond){
					int cb = (int)avgPixelIntensityY.val[0];
					int cg = (int)avgPixelIntensityY.val[1];
					int cr = (int)avgPixelIntensityY.val[2];
					double m = (cb + cg + cr) / 3;
					double std = sqrt(((cb )*(cb ) + (cg )*(cg ) + (cr )*(cr )) / 3);
					bool yellowRule = (framenum > 1) && (presentSignal != 2) && (cg - previous[1][1] > previousStdY) && (cr - previous[1][2] > previousStdY);
					if (yellowRule){
						//if ((framenum > 1) && ((std - previousStdY)>previousStdY)){
						presentSignal = 2;
						cout << "\n\n\t\tYellow light signal!\n" << endl;
						signalChange << "Yellow light signal: " << framenum << "th Sec\n";

					}
					previous[1][0] = cb;
					previous[1][1] = cg;
					previous[1][2] = cr; 
					previousStdY = std;
					cout << "\tYellow:\t" << cb << "\t " << cg << "\t " << cr << "\t " << (int)std;
					record << "\t" << cb << "\t " << cg << "\t " << cr << "\t " << (int)std;
				}
				waitKey(delay);

				//Red Light
				frame(r).copyTo(rr);
				resize(rr, red, enlarge2);//resize image
				moveWindow("red", 1175, 0);
				imshow("red", red);
				namedWindow("red", CV_WINDOW_AUTOSIZE);
				Scalar avgPixelIntensityR = mean(rr);
				if (everySecond){
					int cb = (int)avgPixelIntensityR.val[0];
					int cg = (int)avgPixelIntensityR.val[1];
					int cr = (int)avgPixelIntensityR.val[2];
					double m = (cb + cg + cr) / 3;
					double std = sqrt(((cb )*(cb ) + (cg )*(cg ) + (cr )*(cr )) / 3);
					cout << "\tRed:\t" << cb << "\t " << cg << "\t " << cr << "\t " << int(std) << endl;
					record << "\t" << cb << "\t " << cg << "\t " << cr << "\t " << int(std) << endl;
					bool redPrerequisite = (presentSignal != 3);
					bool redRule = (framenum > 1) && redPrerequisite && ((cr - previous[2][2])>previousStdR);
					if (redRule){
						//if ((framenum > 1) && ((std - previousStdR)>previousStdR)){
						presentSignal = 3;
						cout << "\n\n\t\tRed light signal!" << endl;
						signalChange << "Red light signal: " << framenum << "th Sec\n";
					}
					previous[2][0] = cb;
					previous[2][1] = cg;
					previous[2][2] = cr;
					previousStdR = std;

				}
				waitKey(delay);

				if (waitKey(delay) == 27) //wait for 'esc' key press for delay ms. If 'esc' key is pressed, break loop
				{
					notPressEsc = false;
					destroyAllWindows();
				}
			}
	}
	signalChange.close();
	record.close(); 
}