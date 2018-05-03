import numpy as np
import cv2
import copy
import time
import Adafruit_BBIO.GPIO as GPIO


top = 150
bottom = 20
left = 50
right = 20

thickness = 3
variance = 200
minArea = 80
maxArea = 200

h = 240-(top+bottom)
w = 320-(left+right)
lane = np.zeros([h,w],dtype = np.uint8)
cx = w//2
cy = 2
ly = h
ry = h
m1 = -cx/float((ly-cy))
m2 = 1.0*(w-cx)/(ry-cy)
b1 = -m1*ly
b2 = w-m2*ry
for i in range(0,h):
    for j in range(0,w):
        if((i > cy and j > i*m1 + b1 and j < i*m2 + b2)):
            lane[i,j] = 1

hh = 320
ww = 240
vc = cv2.VideoCapture(-1) #Change to 0 ######################
vc.set(3,hh) #height
vc.set(4,ww) #width

GPIO.setup("P8_10", GPIO.OUT)

if vc.isOpened(): # try to get the first frame
    rval, frame = vc.read()        
else:
    rval = False

for i in range(0,15):
    rval, frame = vc.read()
    key = cv2.waitKey(20)
#count = 0
while rval:
    tic = time.time()
    greyImg = cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)
    width = np.shape(greyImg)[1]
    height = np.shape(greyImg)[0]
    croppedImg = greyImg[top:height-bottom,left:width-right]
    t_otsu,timg = cv2.threshold(croppedImg,0,255,cv2.THRESH_BINARY_INV+cv2.THRESH_OTSU)
    height,width = np.shape(croppedImg)
    timg*=lane
    did_detect = False
    timg_copy = copy.deepcopy(timg)
    contours,hierarchy = cv2.findContours(timg_copy,cv2.RETR_LIST,cv2.CHAIN_APPROX_SIMPLE)
    contours.sort(key = lambda x:cv2.contourArea(x),reverse = True)
    #output = np.zeros([height,width])
    output = croppedImg
    n = 20 if 20 < len(contours) else len(contours)
    for i in range(0,n):
        area = cv2.contourArea(contours[i])
        if(area < maxArea and area > minArea):
            im2 = np.zeros([height,width],dtype = np.uint8)
            cv2.drawContours(im2, contours, i, 255, -1)
            d = cv2.distanceTransform(im2,cv2.cv.CV_DIST_L2,3)
            d = np.transpose(np.nonzero(d))
            dd = np.mean(d)
            mask = np.transpose(np.nonzero(im2))  
            test = croppedImg[mask[:,0],mask[:,1]]
            if(dd > thickness and np.var(test) > variance):
                did_detect = True
                #x, y, w, h = cv2.boundingRect(contours[i])
                #cv2.rectangle(output,(x,y),(x+w,y+h),255,2)
                #for i in range(0,height):
                    #for j in range(0,width):
                        #if(im2[i][j] != 0):
                            #output[i][j] = im2[i][j]
		GPIO.output("P8_10", GPIO.HIGH)
    		time.sleep(0.5)
    		GPIO.output("P8_10", GPIO.LOW)
    		#time.sleep(0.5)

                #break

    #print(did_detect)
    print('Time:',time.time()-tic)
    rval, frame = vc.read()
    key = cv2.waitKey(20)
    if key == 27: # exit on ESC
        break
    #count += 1
        
    #title = 'OutputFrameGST'+str(count)+'.jpg'
    #cv2.imwrite(title,output)
    #if(count == 150):
        #break
        
print('done')
vc.release()


