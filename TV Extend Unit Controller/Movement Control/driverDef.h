/*
 * driverDef.h
 *
 * Created: 17.11.2020 12:05:12
 *  Author: Hans Philipp Zimmermann
 */ 


#ifndef DRIVERDEF_H_
#define DRIVERDEF_H_

#define		STOP		0
#define		MOVE_OUT	1
#define		MOVE_IN		2

#define		POSITION_UNDEFINED	0
#define		FRONT_POSITION		1
#define		MID_POSITION		2
#define		BACK_POSITION		3
#define		ERROR_POSITION		8
#define		SENSOR_ERROR		255

#define		DRIVEMODE_NONE				0
#define		DRIVEMODE_LINEAR_OUT		1
#define		DRIVEMODE_LINEAR_IN			2
#define		DRIVEMODE_TILT_OUT			3
#define		DRIVEMODE_TILT_IN			4
#define		DRIVEMODE_LINEAROUT_TILTOUT	5
#define		DRIVEMODE_TILTIN_LINEARIN	6

#define		DRIVEMODE_EMERGENCY_STOP	255
//#define		DRIVEMODE_DRIVE_INTERRUPTED	9

#define		DRIVETYPE_NONE				0
#define		DRIVETYPE_SINGLE_DRIVE		1
#define		DRIVETYPE_FUSION_DRIVE		2
#define		DRIVETYPE_SECURITY_DRIVE	3





#endif /* DRIVERDEF_H_ */