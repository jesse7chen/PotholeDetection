#include "database.h"
#include <math.h>
#include <string.h>
#include "flash.h"
#include "test_database.h"
//#define _USE_MATH_DEFINES

#define M_PI 3.14159265358979323846
#define DATABASEIDX_P 0x00007000
#define DATABASE_P 0x00007100
#define MAX_NUM_ENTRIES 240

//#define COSINES 1
//#define HAVERSINE 1
#define EQUIRECT 1

static database_loc_t* database;
static unsigned int databaseIdx; // Should point to next free spot in database
static uint8_t warningStatus = 0;

// Threshold is 25m
static double distThreshold = 25;
// Our bearing should be within this many degrees of expected bearing to trigger warning
static double bearingThreshold = 60;

// For use in stage 2
static database_loc_t dangerousPothole;
static database_loc_t previousLoc;


// TODO: Add error checking to insertLocation so we don't overwrite SRAM


int databaseInit(void){
    databaseIdx = findDatabaseIdx();
    database = (database_loc_t*)DATABASE_P;
    
    return 0;
}

// Should return distance in m
double distBetweenLocs(database_loc_t loc1, database_loc_t loc2){
    // Haversine implementation

#ifdef HAVERSINE
    double R = 6371000.0; // Earth's radius in m
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
    double R = 6371000.0; // Earth's radius in m
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
    double R = 6371000.0; // Earth's radius in m
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

double bearingBetweenLocs(database_loc_t loc1, database_loc_t loc2){
    // Lambda is longitude, phi is latitude
    double lat1, lat2, lon1, lon2;
    double deltaLon;
    double y,x,b;
    
    lat1 = degToRads(loc1.latitude);
    lat2 = degToRads(loc2.latitude);
    lon1 = degToRads(loc1.longitude);
    lon2 = degToRads(loc2.longitude);
    deltaLon = lon2 - lon1;
    
    y = sin(deltaLon) * cos(lat2);
    x = cos(lat1)*sin(lat2) - sin(lat1)*cos(lat2)*cos(deltaLon);
    b = atan2(y,x);
    
    return 360.0*b/(2*M_PI);
}

// Warning: If program crashes while writing a location, database/database index might get corrupted. 
int insertLocation(location_t loc){
    // database_lot_t should have a size of 16 bytes
    // Need to write a minimum of 16 location entries
    database_loc_t data [16];
    unsigned int dest;
    unsigned int page;
    unsigned int entry_num;
    unsigned int i = 0;
    
    // Check if we're gonna overflow
    if(databaseIdx >= MAX_NUM_ENTRIES){
        return DATABASE_WRITE_ERROR;
    }
    
    
    // We operate in pages of 256 bytes, so load in any potholes that are on our page
    page = databaseIdx/16;
    entry_num = databaseIdx % 16;
    for(i = 0; i < entry_num; i++){
        data[i] = database[(16*page)+i];
    }
    // Add in our pothole
    data[entry_num].latitude = loc.latitude;
    data[entry_num].longitude = loc.longitude;
    
    // Set rest of data to 0xFF, so we don't set any more flash bits to 0 than we need to (otherwise we have to rewrite entire block)
    // Only do this if we're not on the last entry of the page
    if(entry_num != 15){
        memset(data+entry_num+1, 0xFF, sizeof(database_loc_t)*(16-entry_num-1));
    }

    dest = (DATABASE_P + (page*256));
    
    if(writeFlash(dest, (unsigned int)data, 256) != CMD_SUCCESS){
        return DATABASE_WRITE_ERROR;
    }
    
    // Update the database idx. Set the first databaseIdx bytes to 0s, and the rest to 0xFF
    memset(data, 0xFF, sizeof(uint8_t)*256);
    // We add databaseIdx+1 bytes, since indexes start counting at zero, but we can't write zero bytes
    // Ex. If database index points to 5, that means we have 5 entries already and next open spot is at database[5]. 
    // So we write 5+1 zeros to signify we now have 6 bytes. 
    memset(data, 0x00, sizeof(uint8_t)*(databaseIdx+1));
    dest = DATABASEIDX_P;
    if(writeFlash(dest, (unsigned int)data, 256) != CMD_SUCCESS){
        return DATABASE_WRITE_ERROR;
    }
    
    // Update database index number
    databaseIdx++;
    
    return 0;
}

static unsigned int findDatabaseIdx(void){
    // The first 256 bytes of our database is reserved for the index. We find the current index by counting
    // the number of bytes set to 0. We want to return a pointer to the next free spot.
    unsigned int offset = 0;
    uint8_t* temp_p = (uint8_t*)DATABASEIDX_P;
    
    while(*(temp_p + offset) != 0xFF){
        offset++;
    }
    
    return offset;
    
}

uint8_t searchDatabase(location_t currLoc){
    database_loc_t db_currLoc;
    unsigned int searchIdx;
    double expectedBearing;
    
    db_currLoc.latitude = currLoc.latitude;
    db_currLoc.longitude = currLoc.longitude;
    
    if(warningStatus == 0){
        // Not at risk of hitting pothole yet
        for(searchIdx = 0; searchIdx < databaseIdx; searchIdx++){
            if(distBetweenLocs(db_currLoc, database[searchIdx]) < distThreshold){
                previousLoc = db_currLoc;
                dangerousPothole = database[searchIdx];
                warningStatus = 1;
                
            }
        }
    }
    else if(warningStatus == 1){
        // Are we still within the warning distance?
        if(distBetweenLocs(db_currLoc, dangerousPothole) < distThreshold){
            // Already near pothole, check if we're heading towards pothole
            // Calculate bearing of GPS coordinate with bearing we would expect if headed towards pothole
            expectedBearing = bearingBetweenLocs(previousLoc, dangerousPothole);   
            if ((fabs(expectedBearing - currLoc.bearing) < bearingThreshold) || (fabs(expectedBearing - currLoc.bearing) > (360-bearingThreshold))){
                // Reset warning status
                warningStatus = 0;
                return 1;             
            }
        }
        else{
            // If not, reset warningStatus and return no dangerous potholes nearby
            warningStatus = 0;
        }     
    }
    // No potholes dangerous
    return 0;   
}

database_loc_t retrieveLocation(unsigned int idx){
    database_loc_t output = {.latitude = 99.99, .longitude = 99.99};
    if(idx < databaseIdx){
        return database[idx];
    }
    
    return output;
}


static double degToRads(double degrees){
    return degrees * M_PI / 180;
}

