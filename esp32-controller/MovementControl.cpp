#include <math.h>
#include <Arduino.h>

#include "MovementControl.h"
#include "MotorControl.h"

MovementController::MovementController(int x, int y, int z) {
    curPos[0] = x;
    curPos[1] = y;
    curPos[2] = z;

    int l0, l1, l2, l3; 
    get_cable_lengths(x, y, z, l0, l1, l2, l3); 

    motor_controller = new MotorController(l0, l1, l2, l3);
}

void MovementController::move_to(int x, int y, int z) {
    Serial.print("Trying to move to ");
    Serial.print(x); Serial.print(", "); Serial.print(y); Serial.print(", "); Serial.println(z);
    Serial.print("Current pos: ");
    Serial.print(curPos[0]); Serial.print(", "); Serial.print(curPos[1]); Serial.print(", "); Serial.println(curPos[2]);

    // define variables
    int v0, v1, v2, l0, l1, l2, l3;
    double l;
    double s0, s1, s2, p0, p1, p2;

    // calculate the movement vector v
    v0 = x-curPos[0]; 
    v1 = y-curPos[1]; 
    v2 = z-curPos[2];

    // calculate the lenght l of v
    l = sqrt(pow(v0,2) + pow(v1,2) + pow(v2,2));
    Serial.print("l: ");
    Serial.println(l);

    while (l > section_length) {
        // calculate the section vector (unit vector times section length)
        s0 = (v0/l)*section_length; 
        s1 = (v1/l)*section_length; 
        s2 = (v2/l)*section_length;

        // calculate new section position
        p0 = curPos[0] + s0; 
        p1 = curPos[1] + s1; 
        p2 = curPos[2] + s2; 

        // calculate desired motor cable lengths
        get_cable_lengths(p0, p1, p2, l0, l1, l2, l3);

        // move the motors accordingly
        motor_controller->setCableLenghts(l0, l1, l2, l3);

        // set our new position
        curPos[0] = p0;
        curPos[1] = p1;
        curPos[2] = p2;

        Serial.print("Current pos: ");
        Serial.print(curPos[0]); Serial.print(", "); Serial.print(curPos[1]); Serial.print(", "); Serial.println(curPos[2]);
        // calculate new movement vector and its length
        v0 = x-curPos[0]; 
        v1 = y-curPos[1]; 
        v2 = z-curPos[2];
        l = sqrt(pow(v0,2) + pow(v1,2) + pow(v2,2));
    }

    // if l is shoreter than the section length, we simply adjust the motors an are done
    Serial.println("almost done");
    // calculate the desired motor cable lengths
    get_cable_lengths(x, y, z, l0, l1, l2, l3);
    // move the motors accordingly
    motor_controller->setCableLenghts(l0, l1, l2, l3);
    // set our new position 
    curPos[0] = x;
    curPos[1] = y;
    curPos[2] = z;

    Serial.print("Current pos: ");
    Serial.print(curPos[0]); Serial.print(", "); Serial.print(curPos[1]); Serial.print(", "); Serial.println(curPos[2]);
}

void MovementController::get_cable_lengths(double x, double y, double z, int &l0, int &l1, int &l2, int &l3) {
    double vm0 [3] = { m0[0]-x, m0[1]-y, m0[2]-z };
    double vm1 [3] = { m1[0]-x, m1[1]-y, m1[2]-z };
    double vm2 [3] = { m2[0]-x, m2[1]-y, m2[2]-z };
    double vm3 [3] = { m3[0]-x, m3[1]-y, m3[2]-z };
    l0 = round(sqrt(pow(vm0[0],2) + pow(vm0[1],2) + pow(vm0[2],2)));
    l1 = round(sqrt(pow(vm1[0],2) + pow(vm1[1],2) + pow(vm1[2],2)));
    l2 = round(sqrt(pow(vm2[0],2) + pow(vm2[1],2) + pow(vm2[2],2)));
    l3 = round(sqrt(pow(vm3[0],2) + pow(vm3[1],2) + pow(vm3[2],2)));
}