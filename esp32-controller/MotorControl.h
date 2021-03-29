#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

struct t_CableLengths {
    int len0;
    int len1;
    int len2;
    int len3;
};

int max(int, int, int, int); 

class MotorController {
    public:
        MotorController(int l0, int l1, int l2, int l3);


        // higher-level positioning functions
        /**
         * Sets the lengths of the cables to the given lengths in mm. 
         */
        void setCableLenghts(int l0, int l1, int l2, int l3); 

        /**
         * Set the given motors to increase the cable length with each step. 
         */
        void motorOut(bool m0, bool m1, bool m2, bool m3);

        /**
         * Set the given motors to decrease the cable length with each step. 
         */
        void motorIn(bool m0, bool m1, bool m2, bool m3);

        /**
         * One increase/decrease step for the given motors. 
         */
        void motorStep(bool m0, bool m1, bool m2, bool m3);
    private:

        TaskHandle_t cable_task; 

        QueueHandle_t cable_length_queue; 

        static void set_cable_lengths_task(void *);

        int m0len; 
        int m1len; 
        int m2len; 
        int m3len; 
};


#endif