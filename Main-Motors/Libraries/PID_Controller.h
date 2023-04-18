#ifndef PID_CONTROLLER_H
#define	PID_CONTROLLER_H

class PID_Controller
{
    private:
        float kp, kd, ki, umax;
        float eprev, eintegral;
    public:
        PID_Controller(): kp(1), kd(0), ki(0), umax(255), eprev(0.0), eintegral(0.0) {}
        
        // A function to set the parameters
        void setParams(float kpIn, float kdIn, float kiIn, float umaxIn)
        {
            kp = kpIn; kd = kdIn; ki = kiIn; umax = umaxIn;
        }

        // A function to compute the control signal
        void evalu(int value, int target, float deltaT, int &pwr, int &dir){
            // error
            int e = target - value;
        
            // derivative
            float dedt = (e-eprev)/(deltaT);
        
            // integral
            eintegral = eintegral + e*deltaT;
        
            // control signal
            float u = kp*e + kd*dedt + ki*eintegral;
        
            // motor power
            pwr = (int) fabs(u);
            if( pwr > umax ){
            pwr = umax;
            }
        
            // motor direction
            dir = 1;
            if(u<0){
            dir = -1;
            }
        
            // store previous error
            eprev = e;
        }
};

#endif