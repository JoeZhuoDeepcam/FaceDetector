# FaceDetector
"""This is a cross platform reincarnation of FaceDetector.cpp
"""


import cv2
import numpy as np
import serial
import serial.tools.list_ports


# def detectAndDisplay(frame):

def main():
    # seelct the opencv algorithms
    face_cascade_name="C:/opencv/opencv/sources/data/haarcascades/haarcascade_frontalface_alt.xml"
    eyes_cascade_name = "C:/opencv/opencv/sources/data/haarcascades/haarcascade_eye.xml"

    # open a serial port to the arduino
    ports = list(serial.tools.list_ports.comports())
    for p in ports:
        print p
        if "Arduino" in p[1]:
            print "This is an Arduino!"
        ser=serial.Serial(p[0],9600)

    # Create a cascaded classifier object
    faceCascade = cv2.CascadeClassifier(face_cascade_name)

    video_capture = cv2.VideoCapture(0) # This opens the default camera

    meas=[]
    pred=[]
    mp = np.array((2,1), np.float32) # measurement
    tp = np.zeros((2,1), np.float32) # tracked / prediction

    # cv2.namedWindow("kalman")
    # cv2.setMouseCallback("kalman",onmouse);
    kalman = cv2.KalmanFilter(4,2)
    kalman.measurementMatrix = np.array([[1,0,0,0],[0,1,0,0]],np.float32)
    kalman.transitionMatrix = np.array([[1,0,1,0],[0,1,0,1],[0,0,1,0],[0,0,0,1]],np.float32)
    kalman.processNoiseCov = np.array([[1,0,0,0],[0,1,0,0],[0,0,1,0],[0,0,0,1]],np.float32) * 0.3
    #kalman.measurementNoiseCov = np.array([[1,0],[0,1]],np.float32) * 0.00003

    while True:
        # Capture frame-by-frame
        ret, frame = video_capture.read()

        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

        faces = faceCascade.detectMultiScale(
            gray,
            scaleFactor=1.1,
            minNeighbors=5,
            minSize=(30, 30),
            flags=cv2.CASCADE_SCALE_IMAGE
        )

        # Draw a rectangle around the faces
        i=0
        av_x=0
        for (x, y, w, h) in faces:
            cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 2)
            ser.write(str(x+w/2)+','+'\n')
            if(i==0):
                kalman.correct(np.array([[np.float32(x)],[np.float32(y)]]))
                tp=kalman.predict()
                cv2.rectangle(frame, (tp[0], tp[1]), (tp[0]+w, tp[1]+h), (255, 255, 0), 2)
                # ser.write(str(np.asarray((tp[0]+x)/2).astype(np.int32)+w/2)+','+'\n')
            i=i+1
            # if(i>0):
            #     av_x=(x+av_x*i)/(i+1)
            # else:
            #     av_x=x
            # ser.write(str(av_x.astype(np.int32)+w/2)+','+'\n')

            # print(ser.readline())

        # Display the resulting frame
        cv2.imshow('Video', frame)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    # When everything is done, release the capture
    video_capture.release()
    cv2.destroyAllWindows()
    # KalmanFilter KF(4,2,0)
    # KF.transitionMatrix=np.asmat([[1, 0,1, 0],[0,1,0,1],[0,0,1,0,],[0,0,0,1]])

    # open camera

    # initialize x, y, dx, dy  for kalman filter


    # open serial port


    # predict kalman filter


    # convert colour to greyscale
    # histogram equilization
    # detect the face using detect Multiscale

    # for loop to manage faces, points and eyes_cascade_name
    # estyimate kalman filter
    # draw circle around face_cascade_name

    # send xy position over serial port

    # show the final product
if __name__=="__main__":
    main()
