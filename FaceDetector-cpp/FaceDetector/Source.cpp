

#include "opencv2/objdetect.hpp"

#include "opencv2/highgui.hpp"

#include "opencv2/imgproc.hpp"

#include "opencv2/video/tracking.hpp"

#include <Windows.h>

#include <iostream>

#include <stdio.h>

#include <string>

#include <opencv2/core/core.hpp>

#include "tserial.h"

using namespace std;

using namespace cv;

/* Function Headers */

void detectAndDisplay(Mat frame);

/* Global variables */

int i = 1;

String face_cascade_name = "C:/opencv/opencv/sources/data/haarcascades/haarcascade_frontalface_alt.xml";

String eyes_cascade_name = "C:/opencv/opencv/sources/data/haarcascades/haarcascade_eye.xml";

CascadeClassifier face_cascade;

CascadeClassifier eyes_cascade;

String window_name = "Capture - Face detection";

//Arduino global declarations
int arduino_command;
Tserial *arduino_com;
short MSBLSB = 0;
unsigned char MSB = 0;
unsigned char LSB = 0;

//cv::KalmanFilter kf(stateSize, measSize, contrSize, type);

/* @function main */

int main(void)

{

	

	//Load all of the kalman filter paramaters
	KalmanFilter KF(4, 2, 0);
	POINT facePos;
	//Mat_<float> temp= (Mat_<float>(4, 4) << 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1);
	//KF.transitionMatrix=*temp;
	KF.transitionMatrix=(Mat_<float>(4, 4) << 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1);
	Mat_<float> measurement(2, 1); measurement.setTo(Scalar(0));


	cv::VideoCapture capture(0);

	cv::Mat frame;

	KF.statePre.at<float>(0) = 310; //x
	KF.statePre.at<float>(1) = 250; //y
	KF.statePre.at<float>(2) = 0; //dx
	KF.statePre.at<float>(3) = 0; //dy

	setIdentity(KF.measurementMatrix);
	setIdentity(KF.processNoiseCov, Scalar::all(1e-4));
	setIdentity(KF.measurementNoiseCov, Scalar::all(10));
	setIdentity(KF.errorCovPost, Scalar::all(.1));

	vector<Point> mousev, kalmanv;
	//mousev.clear();
	//kalmanv.clear();


	arduino_com = new Tserial();
	if (arduino_com != 0) {
		arduino_com->connect("\\\\.\\COM10", 9600, spNONE);
	}
	if (arduino_com->connect("\\\\.\\COM13", 9600, spNONE) != 0) {
		arduino_com->connect("\\\\.\\COM10", 9600, spNONE);
	}
	if (arduino_com->connect("\\\\.\\COM13", 9600, spNONE) != 0) {
		arduino_com->connect("\\\\.\\COM11", 9600, spNONE);
	}

	//-- 1. Load the cascades


	if (!face_cascade.load(face_cascade_name)) { printf("--(!)Error loading face cascade\n"); system("PAUSE"); return -1; };

	if (!eyes_cascade.load(eyes_cascade_name)) { printf("--(!)Error loading eyes cascade\n"); system("PAUSE"); return -1; };

	//-- 2. Read the video stream


	//capture.open(-1);

	if (!capture.isOpened()) { printf("--(!)Error opening video capture\n");  system("PAUSE"); return -1; }

	while (capture.read(frame))

		//while (1)

	{

		//capture >> frame;

		if (frame.empty())

		{

			printf(" --(!) No captured frame -- Break!");

			break;

		}

		// KALMAN First predict the internal state
		Mat prediction = KF.predict();
		Point predictPt(prediction.at<float>(0), prediction.at<float>(1));
		//KALMAN END

		//-- 3. Apply the classifier to the frame

		cv::namedWindow(window_name, 1);

		//imshow(window_name, frame);



		//detectAndDisplay(frame);

		//printf("one");

		std::vector<Rect> faces;

		Mat frame_gray;

		//printf("two");

		cvtColor(frame, frame_gray, COLOR_BGR2GRAY); //or cv_bgr2gray

													 //printf("three");

		cv::equalizeHist(frame_gray, frame_gray);

		//printf("four");

		//-- Detect faces

		face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30)); //cv_haar_scale_image?

																										 //printf("five");

																										 //cout << faces.size();

		for (size_t i = 0; i < faces.size(); i++)

		{

			Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);

			//printf("point is complete.\n");

			ellipse(frame, center, Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);

			Mat faceROI = frame_gray(faces[i]);

			//printf("matrix is created..\n");

			std::vector<Rect> eyes;

			//printf("vector is created");

			//-- In each face, detect eyes

			eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 4, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

			//cout << eyes.size();

			for (size_t j = 0; j < eyes.size(); j++)

			{

				Point eye_center(faces[i].x + eyes[j].x + eyes[j].width / 2, faces[i].y + eyes[j].y + eyes[j].height / 2);

				int radius = cvRound((eyes[j].width + eyes[j].height)*0.25);

				//circle(frame, eye_center, radius, Scalar(255, 0, 0), 4, 8, 0); //display the eyes

			}

		}
		/*
		if (faces.size() == 1) {
			//cout << faces[0].x<<endl;
		}
		*/
		///*
		if (faces.size() == 1) {
			measurement(0) = faces[0].x+faces[0].width / 2;
			measurement(1) = faces[0].y+faces[0].height/2;

			Mat estimated = KF.correct(measurement);

			Point statePt(estimated.at<float>(0), estimated.at<float>(1));
			Point measPt(measurement(0), measurement(1));
			//Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
			//cout << measurement(0) << " " << measurement(1) << " " << faces[0].height<<" " << faces[0].width<<endl;

			//printf("point is complete.\n");
			//Point est_center(measurement(0) + faces[0].width / 2, faces[i].y + faces[i].height / 2);
			ellipse(frame, statePt, Size(faces[0].width / 2, faces[0].height / 2), 0, 0, 360, Scalar(255, 255, 0), 4, 8, 0);

			/*
			// use this for linux
			FILE *file;
			file = fopen("\\\\.\\COM10", "w"); //open the device. Which device filename?
			if (file == NULL)perror("Error opening arduino file");
			int i = 0;
			fprintf(file, "%d", measurement(0));
			fprintf(file, "%c", ','); //delimited by a comma
			Sleep(1);
			fprintf(file, "%d", measurement(1));
			fprintf(file, "%c", ','); //delimited by a comma
			Sleep(1);
			fprintf(file, "%d", faces[0].height); //print the face size
			fprintf(file, "%c", '\n'); //delimited by a comma
			Sleep(1);
			*/

			//this is for windows
			faces[0].x=measurement(0);
			faces[0].y = measurement(1);

			//faces[0].x=estimated.at<float>(0);
			//char x = '0'+faces[0].x;
			char y = faces[0].y;
			
			//int *x = new int[(cv::Rect)faces[0].x];

			//char x_char[] = sprintf('%d',faces[0].x);
			//char y= &faces[0].y;
			// convert x to string
			string print = "0" + faces[0].x;
			//signed char x={faces[0].x};

			char x;
			
			int hundreds = faces[0].x / 100;
			cout << "hundreds is " << hundreds << endl;
			int tens = (faces[0].x % 100)/10;
			cout << "tens is " << tens << endl;
			int ones = faces[0].x % 10/1;
			cout << "ones is " << ones << endl;

			x = hundreds+48;
			LSB = x & 0xff;
			MSB = (x >> 8) & 0xff;

			cout << MSB << " " << LSB << endl; //this is giving me strange characters
			arduino_com->sendChar(MSB);
			arduino_com->sendChar(LSB);

			x = tens + 48;
			LSB = x & 0xff;
			MSB = (x >> 8) & 0xff;

			cout << MSB << " " << LSB << endl; //this is giving me strange characters
			arduino_com->sendChar(MSB);
			arduino_com->sendChar(LSB);

			x = ones + 48;
			LSB = x & 0xff;
			MSB = (x >> 8) & 0xff;

			cout << MSB << " " << LSB << endl; //this is giving me strange characters
			arduino_com->sendChar(MSB);
			arduino_com->sendChar(LSB);

			/*
			for (i = 0; i < print.length(); i++) {
				char x = print[i];
				LSB = x & 0xff;
				MSB = (x >> 8) & 0xff;
				cout << MSB<<"string"<<LSB << endl;
				arduino_com->sendChar(MSB);
				arduino_com->sendChar(LSB);
			}*/
			x = 10; // corresponds to ascii new line
			x = 44; //corresponds to ascii 
			LSB = x & 0xff;
			MSB = (x >>8) & 0xff;

			cout << MSB << " " << LSB << endl; //this is giving me strange characters
			arduino_com->sendChar(MSB);
			arduino_com->sendChar(LSB);
			


			//arduino_com->sendChar(1);
			//cout << faces[0].x << " ";
			//arduino_com->sendChar(faces[0].x);
			//cout<<arduino_com->getChar()<< endl;
			//int c = waitKey(1000);

			// Send Y axis
			//LSB = faces[0].y & 0xff;
			//MSB = (faces[0].y >> 8) & 0xff;
			//arduino_com->sendChar(MSB);
			//arduino_com->sendChar(LSB);
			
			//arduino_com->sendChar(0);
			//arduino_com->sendChar(faces[0].y);


		}
		//*/

		//KALMAN get face point
		//&facePos from faces[0]
		//measurement(0) = faces[0].x;
		//measurement(1) = faces[1].y;
		
		//Mat estimated = KF.correct(measurement);
		//Point statePt(estimated.at<float>(0), estimated.at<float>(1));
		//Point measPt(measurement(0), measurement(1));
		//ellipse(frame, statePt, Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, Scalar(255, 255, 0), 4, 8, 0);
		//-- Show what you got

		//cv::namedWindow(window_name, 1);



		cv::imshow(window_name, frame);



		//		printf("Function complete");

		int c = waitKey(300);

		if ((char)c == 27) { cout << "true"; break; } // escape

	}
	arduino_com->disconnect();
	delete arduino_com;
	arduino_com = 0;
	return 0;

}
/*
//arduino code
// Camera control
// https://www.arduino.cc/en/Tutorial/StringToIntExample


#include <Servo.h>  //Used to control the Pan/Tilt Servos
#include <WString.h> //provides easy string handling
String readString = String(100);

//These are variables that hold the servo IDs.
char tiltChannel = 0, panChannel = 1;
String inString = "";
int n = 0;

//These are the objects for each servo.
Servo servoPan;

//This is a character that will hold data from the Serial port.
char serialChar = 0;


int pos = 90;
void setup() {
	//servoTilt.attach(2);  //The Tilt servo is attached to pin 2.
	servoPan.attach(3);   //The Pan servo is attached to pin 3.
						  //servoTilt.write(90);  //Initially put the servos both
	servoPan.write(pos);      //at 90 degress.

	Serial.begin(9600);  //Set up a serial connection for 57600 bps.
}


void loop() {
	while (Serial.available() > 0) {
		int inChar = Serial.read();
		if (isDigit(inChar)) {
			// convert the incoming byte to a char
			// and add it to the string:
			inString += (char)inChar;
		}
		// if you get a newline, print the string,
		// then the string's value:
		if (inChar == ',') {
			//Serial.print("Value:");
			//Serial.println(inString.toInt());
			//Serial.print("String: ");
			//Serial.println(inString);
			// clear the string for new input:
			n = inString.toInt();
			inString = "";
			Serial.println(n);
			int err;
			int bias;
			bias = 0;
			err = map(n, 0, 640, -12 + bias, 12 + bias);
			pos = pos - err;
			if (pos<0) {
				pos = 0;
			}
			else if (pos>180) {
				pos = 180;
			}
			servoPan.writeMicroseconds(pos);
			servoPan.write(pos);

		}
	}
}
*/