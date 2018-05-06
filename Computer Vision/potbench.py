import numpy as np
import time
import cv2

top = 30
bottom = 10
left = 10
right = 10

h = 300
w = 200

tic = time.time()
lane = np.zeros([w,h],dtype = np.uint8)
cx = 150
cy = 20
ly = 100
ry = 100
m1 = -cx/(ly-cy)
m2 = (w-cx)/(ry-cy)
b1 = -m1*ly
b2 = w-m2*ry
for i in range(0,w):
    for j in range(0,h):
        if((i > cy and j > i*m1 + b1 and j < i*m2 + b2)):
            lane[i,j] = 1
print('Lane Init:',time.time()-tic)

h = 320
w = 240
vc = cv2.VideoCapture(0) #Change to 0 ######################
vc.set(3,h) #height
vc.set(4,w) #width
if vc.isOpened(): # try to get the first frame
    rval, frame = vc.read()
else:
    rval = False

tic = time.time()
for i in range(0,15):
    rval, frame = vc.read()
    key = cv2.waitKey(20)
print('Init frames', time.time()-tic)

while rval:
    tic1 = time.time()
    tic = time.time()
    #greyImg = cv2.cvtColor(cv2.resize(frame, (0,0), fx=0.5, fy=0.5) , cv2.COLOR_BGR2GRAY)
    greyImg = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    print('grayscale:', time.time()-tic)
    width = np.shape(greyImg)[1]
    height = np.shape(greyImg)[0]
    croppedImg = greyImg[top:height-bottom,left:width-right]
    tic = time.time()
    t_otsu,timg = cv2.threshold(croppedImg,0,255,cv2.THRESH_BINARY_INV+cv2.THRESH_OTSU)
    print('Thresholding:', time.time()-tic)
    height,width = np.shape(croppedImg)
    #print(np.shape(croppedImg))
    tic = time.time()
    #timg*=lane
    print('lane filter:', time.time()-tic)
    did_detect = False
    tic = time.time()
    contours, hierarchy = cv2.findContours(timg,cv2.RETR_LIST,cv2.CHAIN_APPROX_NONE)
    print('Find contours:', time.time()-tic)
    tic = time.time()
    contours.sort(key = len,reverse = True)
    print('Sort by len:',time.time()-tic)
    contours, hierarchy = cv2.findContours(timg,cv2.RETR_LIST,cv2.CHAIN_APPROX_SIMPLE)
    tic = time.time()
    contours.sort(key = lambda x:cv2.contourArea(x),reverse = True)
    print('Sort by area:',time.time()-tic)
    n = 20 if 20 < len(contours) else len(contours)
    output = np.zeros([height,width])
    #output = croppedImg
    for i in range(0,n):
        area = cv2.contourArea(contours[i])
        if(area < 100000 and area > 100):
            im2 = np.zeros([height,width],dtype = np.uint8)
            tic = time.time()
            cv2.drawContours(im2, contours, i, 255, -1)
            print('Draw time:', time.time()-tic)
            tic = time.time()
            d = cv2.distanceTransform(im2,cv2.cv.CV_DIST_L2,3)
            print('Distance Transform:',time.time()-tic)
            d = np.transpose(np.nonzero(d))
            dd = np.mean(d)
            tic = time.time()
            mask = np.transpose(np.nonzero(im2))  
            test = croppedImg[mask[:,0],mask[:,1]]
            if(dd > 5 and np.var(test) > 100):
                did_detect = True
                #break
        print('Variance:',time.time()-tic)


                
    print('Total time:',time.time()-tic1)
    cv2.imwrite('test.jpg',timg)
    rval, frame = vc.read()
    key = cv2.waitKey(20)
        
vc.release()


