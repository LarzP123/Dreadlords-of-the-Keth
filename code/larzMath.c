#include "larzMath.h"
#include <stdlib.h>

bool getPosFromPerpAxes(double ax1[3],double ax2[3],double *posReturn[3],double angle) {
    *posReturn[0] = ax1[0]*cos(angle)+ax2[0]*sin(angle);
    *posReturn[1] = ax1[1]*cos(angle)+ax2[1]*sin(angle);
    *posReturn[2] = ax1[2]*cos(angle)+ax2[2]*sin(angle);
    return true;
}

int mod(int a, int b) {
    int r = a % b;
    return r < 0 ? r + b : r;
}

bool norm3DVec(double vec[3],double newLength) {
    double magn = sqrt(vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]);
    for (int i=0;i<3;i++){
        vec[i]/=magn;
        vec[i]*=newLength;
    }
    return true;
}

bool scaleMaxMin(double *inputData, int inputSize, double targetMin, double targetMax, double *returnSlope, double *returnOffset) {

    double minInput=*inputData;double maxInput=*inputData;
    for (int i = 0; i < inputSize; i++) {
        double testValue = *(inputData+i);
        if (testValue>maxInput){
            maxInput = testValue;
        } else if (testValue < minInput){
            minInput = testValue;
        }
    }

    *returnSlope = (targetMax-targetMin)/(maxInput-minInput);
    *returnOffset = targetMax-(*returnSlope)*maxInput;
    return true;
}

double correctAngleDiff(double angleDif) {
    if (angleDif<-aPi){
        return angleDif+=2*aPi;
    } else if (angleDif>aPi){
        return angleDif-=2*aPi;
    } else{
        return angleDif;
    }
}

bool crossProd(double v1[3],double v2[3],double vOut[3]) {
    vOut[0] = v1[1] * v2[2] - v1[2] * v2[1];
    vOut[1] = v1[2] * v2[0] - v1[0] * v2[2];
    vOut[2] = v1[0] * v2[1] - v1[1] * v2[0];
    return true;
}

struct TimeDisplay convertDaysToConvenient (double days) {
    struct TimeDisplay timeDisplay;
    if (days > 365.0) {
        // Years
        timeDisplay.Value = days/365.0;
        timeDisplay.Unit = 'y';
    } else if (days > 1.0) {
        // Days
        timeDisplay.Value = days;
        timeDisplay.Unit = 'd';
    } else {
        // Hours
        timeDisplay.Value = days/24.0;
        timeDisplay.Unit = 'h';
    }
    return timeDisplay;
}

// We only need to have refPos as a parameter here in debug. That is because in debug we want to check if any triangles are flipped and flip them correctly,
// but in release, we should go ahead and not waste compute time on that and assume that edges were passed correctly
#ifdef DEBUG
void calculateNormal(double edge1[3], double edge2[3], double normal[3], double refPos[3]) {
#else // DEBUG
void calculateNormal(double edge1[3], double edge2[3], double normal[3]) {
#endif // DEBUG
    //OpenGL requires that I pass a normal for all edges of the triangle.
    //The way I do this here is by calculating the normal from taking the cross product of the edges and
    //Then normalizing it to have length 1.
    crossProd(edge1,edge2,normal);
    norm3DVec(normal,1.0);

#ifdef DEBUG
    double dist1 = sqrt(refPos[0] * refPos[0] + refPos[1] * refPos[1] + refPos[2] * refPos[2]);
    double distSetup2[3] = {refPos[0]+normal[0]*0.05f, refPos[1]+normal[1]*0.05f, refPos[2]+normal[2]*0.05f};
    double dist2 = sqrt(distSetup2[0] * distSetup2[0] + distSetup2[1] * distSetup2[1] + distSetup2[2] * distSetup2[2]);

    if (dist2<dist1) {//Flip the normal to always face outwards from the center
        normal[0] *= -1;
        normal[1] *= -1;
        normal[2] *= -1;
        printf("Flipped\n");
    }
#endif // DEBUG
}

// This function was written like I was smoking crack
void convertDateToString (char* dateDisplay, double gameDate, int displaySystem) {
    #define SecondsInYear 31536000
    #define SecondsInDay 86400
    int dayDigit, monthDigit, yearDigit;
    switch (displaySystem) {
        case 0 : { dayDigit = 8; monthDigit=5; yearDigit = 0; break; } // Year/Month/Day
        case 1 : { dayDigit = 3; monthDigit=0; yearDigit = 6; break; } // Month/Day/Year
        case 2 : { dayDigit = 0; monthDigit=3; yearDigit = 6; break; } // Year/Month/Day
        #ifdef DEBUG
        default: { fprintf(stderr, "The date display system was not one of the acceptable numbers. This should not happen"); }
        #endif // DEBUG
    }

    int year = (int)floor(gameDate);
	// If we didn't have a good value for displaySystem we could get trash values for *Digit variables. But we already handle that with default giving error output
	// NOLINTBEGIN(clang-analyzer-core.UndefinedBinaryOperatorResult)
    dateDisplay[yearDigit+0] = '3' + year/1000; // year starts at 3000, so we can keep datetime var smallish
    dateDisplay[yearDigit+1] = '0' + (year%1000)/100;
    dateDisplay[yearDigit+2] = '0' + (year%100)/10;
    dateDisplay[yearDigit+3] = '0' + year%10;
    dateDisplay[yearDigit+4] = '/';

    // This is how leap years are calculated in the Gregorian Calendar
    // https://en.wikipedia.org/wiki/Leap_year
    // Also do the most common checks first so least amount of checks. No point in doing mod 4000 checks for most years
    bool isLeapYear = (year % 4 == 0 && (year % 100 != 0 || year % 4000 == 0));

    unsigned long seconds = (gameDate-3000) * SecondsInYear;
    int day = ((seconds % SecondsInYear) * (isLeapYear ? 366 : 365) / SecondsInYear);

    int daysInMonth[] = {31, isLeapYear ? 29 : 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    int month = 0;
    while (day >= daysInMonth[month]){
        day -= daysInMonth[month];
        month++;
    }

    dateDisplay[monthDigit+0] = '0' + month/10;
    dateDisplay[monthDigit+1] = '0' + month%10;
    dateDisplay[monthDigit+2] = '/';
    dateDisplay[dayDigit+0] = '0' + day/10;
    dateDisplay[dayDigit+1] = '0' + day%10;
    dateDisplay[dayDigit+2] = ' ';

    // Get Hour
    int hour = (seconds % 86400) / 3600;
    dateDisplay[11] = '0' + (hour / 10);
    dateDisplay[12] = '0' + (hour % 10);
    dateDisplay[13] = ':';
    // Get Min
    int minutes = (seconds % 3600) / 60;
    dateDisplay[14] = '0' + (minutes / 10);
    dateDisplay[15] = '0' + (minutes % 10);
    dateDisplay[16] = ':';
    // Get Sec
    seconds = seconds % 60;
    dateDisplay[17] = '0' + (seconds / 10);
    dateDisplay[18] = '0' + (seconds % 10);
    dateDisplay[19] = '\0';
	// NOLINTEND(clang-analyzer-core.UndefinedBinaryOperatorResult)
}
