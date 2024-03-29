#include "GPS.h"
#include "TinyGPS++.h"

TinyGPSPlus gps_p;

void GPS_m::StartTrack()
{
    StopTrack();
    Serial2.begin(Speed_s);
}

void GPS_m::StopTrack()
{
    Serial2.end();
}

void GPS_m::Work()
{
    while(Serial2.available()>0)
    {
        existRec_=true;
        gps_p.encode(Serial2.read());
    }
}

String GPS_m::LonLatAlt()
{
    return (String(Longitude(),5)+":"+String(Latitude(),5)+":"+String(Altitude(),2));
}

bool GPS_m::IsUpdated()
{
    existRec=existRec_;
    return gps_p.location.isUpdated();
}

double GPS_m::Longitude()
{
    return gps_p.location.lng();
}

double GPS_m::Latitude()
{
    return gps_p.location.lat();
}

double GPS_m::Speed()
{
    return gps_p.speed.mps();
}

double GPS_m::Altitude()
{
    return gps_p.altitude.meters();
}
