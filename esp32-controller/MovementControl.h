#ifndef MOVEMENT_CONTROL_H
#define MOVEMENT_CONTROL_H

#include "MotorControl.h"

class MovementController {
    private:
        MotorController *motor_controller;

        // remember our current position
        int curPos [3] = {100, 370, 540};

        // the lenght of a movement section in mm
        int section_length = 5;

        // the positions of the pullies
        int m0 [3] = {0, 0, 800};
        int m1 [3] = {0, 800, 800};
        int m2 [3] = {1500, 800, 800};
        int m3 [3] = {1500, 0, 800};

        void get_cable_lengths(double x, double y, double z, int &l0, int &l1, int &l2, int &l3);

    public:
        MovementController(int x, int y, int z);

        void move_to(int x0, int x1, int x2);

};

#endif