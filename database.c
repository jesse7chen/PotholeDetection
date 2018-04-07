#include "database.h"
#include <math.h>
#include <string.h>
#include "flash.h"
//#define _USE_MATH_DEFINES

#define M_PI 3.14159265358979323846
#define DATABASEIDX_P 0x00007000
#define DATABASE_P 0x00007008

//#define COSINES 1
//#define HAVERSINE 1
#define EQUIRECT 1

static location_t* database;
static unsigned int databaseIdx;

static uint8_t warningStatus = 0;

int databaseInit(void){
    databaseIdx = * ((unsigned int*)DATABASEIDX_P);
    database = (location_t*)DATABASE_P;
    return 0;
}

double distBetweenLocs(location_t loc1, location_t loc2){
    // Haversine implementation

#ifdef HAVERSINE
    double R = 6371000.0; // Earth's radius in km
    double lat1, lat2, lon1, lon2;
    double deltaLat, deltaLon;
    double a, c;
    
    lat1 = degToRads(loc1.latitude);
    lat2 = degToRads(loc2.latitude);
    lon1 = degToRads(loc1.longitude);
    lon2 = degToRads(loc2.longitude);
    
    deltaLat = lat2 - lat1;
    deltaLon = lon2 - lon1;
    
    a = pow(sin(deltaLat/2.0),2.0) + cos(lat1)*cos(lat2) * pow(sin(deltaLon/2.0),2.0);
    c = 2.0*atan2(sqrt(a), sqrt(1-a));
    
    return (R*c);

#elif COSINES  
    // Law of cosines implementation
    double R = 6371000.0; // Earth's radius in km
    double lat1, lat2, lon1, lon2;
    double deltaLat, deltaLon;
    double d;
    
    lat1 = degToRads(loc1.latitude);
    lat2 = degToRads(loc2.latitude);
    lon1 = degToRads(loc1.longitude);
    lon2 = degToRads(loc2.longitude);
    
    deltaLon = lon2 - lon1;

    d = acos(sin(lat1)*sin(lat2) + cos(lat1)*cos(lat2)*cos(deltaLon))*R;
    
    return d;
    
#elif EQUIRECT
    double R = 6371000.0; // Earth's radius in km
    double lat1, lat2, lon1, lon2;
    double deltaLat, deltaLon;
    double x,y,d;
    
    lat1 = degToRads(loc1.latitude);
    lat2 = degToRads(loc2.latitude);
    lon1 = degToRads(loc1.longitude);
    lon2 = degToRads(loc2.longitude);
    
    deltaLat = lat2 - lat1;
    deltaLon = lon2 - lon1;
    
    x = (deltaLon)*cos((lat1+lat2)/2);
    y = deltaLat;
    
    d = sqrt(x*x + y*y) * R;

    return d;
#endif
    
}

int insertLocation(location_t loc){
    // Need to write a minimum of 8 location entries
    location_t data [8];
    unsigned int dest;
    
    // Set all data to 0xFF, so we don't set any more flash bits to 0 than we need to (otherwise we have to rewrite entire block);
    memset(data, 0xFF, sizeof(location_t)*8);
    data[0] = loc;
    dest = (DATABASE_P + (databaseIdx*sizeof(location_t)));
    
    if(writeFlash(dest, (unsigned int)data, 256) != CMD_SUCCESS){
        return -1;
    }
    
    return 0;
}

static uint8_t searchDatabase(location_t currLoc){
        return 1;
    
}



static double degToRads(double degrees){
    return degrees * M_PI / 180;
}

