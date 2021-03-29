#include <Arduino.h>

#include "MotorControl.h"
#include "settings.h"

MotorController::MotorController(int l0, int l1, int l2, int l3) {
    pinMode(M0_STEP_PIN, OUTPUT);
    pinMode(M1_STEP_PIN, OUTPUT);
    pinMode(M2_STEP_PIN, OUTPUT);
    pinMode(M3_STEP_PIN, OUTPUT);
    pinMode(M0_DIR_PIN, OUTPUT);
    pinMode(M1_DIR_PIN, OUTPUT);
    pinMode(M2_DIR_PIN, OUTPUT);
    pinMode(M3_DIR_PIN, OUTPUT);

    // initialize the lenghts to integers; however, these are not calibrated, yet
    m0len = l0; 
    m1len = l1; 
    m2len = l2;
    m3len = l3;

    cable_length_queue = xQueueCreate(20, sizeof(t_CableLengths));

    // start the task on the second core
    xTaskCreatePinnedToCore(
                    this->set_cable_lengths_task,   /* Task function. */
                    "set_cable_lenths_task",     /* name of task. */
                    10000,       /* Stack size of task */
                    this,        /* parameter of the task */
                    1,           /* priority of the task */
                    &cable_task,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
}

void MotorController::motorOut(bool m0, bool m1, bool m2, bool m3) {
    if (m0) digitalWrite(M0_DIR_PIN, LOW);
    if (m1) digitalWrite(M1_DIR_PIN, LOW);
    if (m2) digitalWrite(M2_DIR_PIN, LOW);
    if (m3) digitalWrite(M3_DIR_PIN, LOW);
}

void MotorController::motorIn(bool m0, bool m1, bool m2, bool m3) {
    if (m0) digitalWrite(M0_DIR_PIN, HIGH);
    if (m1) digitalWrite(M1_DIR_PIN, HIGH);
    if (m2) digitalWrite(M2_DIR_PIN, HIGH);
    if (m3) digitalWrite(M3_DIR_PIN, HIGH);
}

void MotorController::motorStep(bool m0, bool m1, bool m2, bool m3) {
    if (m0) digitalWrite(M0_STEP_PIN, HIGH);
    if (m1) digitalWrite(M1_STEP_PIN, HIGH);
    if (m2) digitalWrite(M2_STEP_PIN, HIGH);
    if (m3) digitalWrite(M3_STEP_PIN, HIGH);
    delay(2);
    if (m0) digitalWrite(M0_STEP_PIN, LOW);
    if (m1) digitalWrite(M1_STEP_PIN, LOW);
    if (m2) digitalWrite(M2_STEP_PIN, LOW);
    if (m3) digitalWrite(M3_STEP_PIN, LOW);
    delay(2);
}

/*
void MotorController::set_m0_length(int l0) {
    
}
void MotorController::set_m1_length(int l1) {

}
void MotorController::set_m2_length(int l2) {

}
void MotorController::set_m3_length(int l3) {

}*/

void MotorController::setCableLenghts(int l0, int l1, int l2, int l3) {
    t_CableLengths msg;
    msg.len0 = l0;
    msg.len1 = l1;
    msg.len2 = l2;
    msg.len3 = l3;
    if (xQueueSend(cable_length_queue, &msg, portMAX_DELAY) != pdTRUE) {
        Serial.println("Error: sending to queue failed.");
    }
}

void MotorController::set_cable_lengths_task(void *params) {
    MotorController *mc = (MotorController *) params; 
    int s0, s1, s2, s3, move0, move1, move2, move3, move0o, move1o, move2o, move3o, lmax;
    bool m0in, m1in, m2in, m3in;

    t_CableLengths msg;

    // run forever
    for (;;) {
        if (xQueueReceive(mc->cable_length_queue, &msg, portMAX_DELAY) != pdPASS) {
            Serial.println("Error: receiving from queue failed");
            continue;
        }
        Serial.print("Moving to ");
        Serial.print(msg.len0);
        Serial.print(" ");
        Serial.print(msg.len1);
        Serial.print(" ");
        Serial.print(msg.len2);
        Serial.print(" ");
        Serial.println(msg.len3);

        // determine if we need to reel in or out
        m0in = msg.len0 < mc->m0len;
        m1in = msg.len1 < mc->m1len;
        m2in = msg.len2 < mc->m2len;
        m3in = msg.len3 < mc->m3len;
        mc->motorIn(m0in, m1in, m2in, m3in);
        mc->motorOut(!m0in, !m1in, !m2in, !m3in);

        // get the required movement length (*2 because each step is 0.5mm)
        move0o = move0 = (m0in ? mc->m0len-msg.len0 : msg.len0-mc->m0len) * 2;
        move1o = move1 = (m1in ? mc->m1len-msg.len1 : msg.len1-mc->m1len) * 2;
        move2o = move2 = (m2in ? mc->m2len-msg.len2 : msg.len2-mc->m2len) * 2;
        move3o = move3 = (m3in ? mc->m3len-msg.len3 : msg.len3-mc->m3len) * 2;

        // chech which cable has to move the most
        lmax = max(move0, move1, move2, move3);
        s0 = (move0 > 0 ? lmax / move0 : -1);
        s1 = (move1 > 0 ? lmax / move1 : -1);
        s2 = (move2 > 0 ? lmax / move2 : -1);
        s3 = (move3 > 0 ? lmax / move3 : -1);

        // do the movement for the appropriate number of times
        while (move0>1 || move1>1 || move2>1 || move3>1) {
            mc->motorStep(--s0==0, --s1==0, --s2==0, --s3==0);
            if (move0o > 0 && s0 == 0) {
                move0--;
                s0 = lmax / move0o;
            }
            if (move1o > 0 && s1 == 0) {
                move1--;
                s1 = lmax / move1o;
            }
            if (move2o > 0 && s2 == 0) {
                move2--;
                s2 = lmax / move2o;
            }
            if (move3o > 0 && s3 == 0) {
                move3--;
                s3 = lmax / move3o;
            }
        }
        // do the remaining step, because the loop may miss one
        mc->motorStep(move0>0, move1>0, move2>0, move3>0);

        // remember the new lengths
        mc->m0len = msg.len0;
        mc->m1len = msg.len1;
        mc->m2len = msg.len2;
        mc->m3len = msg.len3;
    }
}

int max(int a, int b, int c, int d) {
    if (a >= b && a >= c && a >= d) 
        return a;
    if (b >= c && b >= d) 
        return b;
    if (c >= d)
        return c;
    return d;
}