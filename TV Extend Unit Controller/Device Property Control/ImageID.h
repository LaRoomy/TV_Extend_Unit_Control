/*
 * ImageID.h
 *
 * Created: 05.03.2020 08:11:50
 *
 * Author: Hans Philipp Zimmermann
 *
 *	This file contains the definitions for the image resource (used by the remote application)
 */ 


#ifndef IMAGEID_H_
#define IMAGEID_H_
// 0 means the control has no image !
#define			IMAGE_ID_NONE					0
#define			IMAGE_ID_PLACEHOLDER			1
#define			IMAGE_ID_RGB_BARS_COLORED		2
#define			IMAGE_ID_LIGHTBULB_DARK			3
#define			IMAGE_ID_LIGHTBULB_LIGHT		4
#define			IMAGE_ID_LIGHTBULB_MONOCROM		5
#define			IMAGE_ID_LIGHTBULB_LRSTYLE		6
#define			IMAGE_ID_SUN_YELLOW				7
#define			IMAGE_ID_SUN_WHITE				8
#define			IMAGE_ID_SUN_BLUE_DUOCROM		9
#define			IMAGE_ID_SUN_BLUE_MONOCROM		10
#define			IMAGE_ID_CLOCK_RELOAD			11
#define			IMAGE_ID_TIME_SETUP				12
#define			IMAGE_ID_PROPERTYGROUP_LRSTYLE	13
#define			IMAGE_ID_RELOADARROW_LEFTBLUE	14
#define			IMAGE_ID_RELOADARROW_RIGHTWHITE	15
#define			IMAGE_ID_RELOADARROW_LEFTWHITE	16
#define			IMAGE_ID_SEGMENTED_CIRCLE		17
#define			IMAGE_ID_SYNC_WHITE				18
#define			IMAGE_ID_SYNC_LRSTYLE			19
#define			IMAGE_ID_SCALE_UP				20
#define			IMAGE_ID_INDIFFERENT_BARS		21
#define			IMAGE_ID_INCREASING_BARS_BLUE	22
#define			IMAGE_ID_INCREASING_BARS_WHITE	23//  /|___ both are the same, thats not a mistake !
#define			IMAGE_ID_LEVEL_100PERC			23//  \|	They have two purposes...
#define			IMAGE_ID_LEVEL_75PERC			24
#define			IMAGE_ID_LEVEL_50PERC			25
#define			IMAGE_ID_LEVEL_25PERC			26
#define			IMAGE_ID_LEVEL_0PERC			27
#define			IMAGE_ID_WARNING_LRSTYLE		28
#define			IMAGE_ID_WARNING_WHITE			29
#define			IMAGE_ID_WARNING_YELLOW			30
#define			IMAGE_ID_WARNING_RED			31
#define			IMAGE_ID_SETTINGS_LRSTYLE		32
#define			IMAGE_ID_SETTINGS1_WHITE		33
#define			IMAGE_ID_SETTINGS1_BLUE			34
#define			IMAGE_ID_SETTINGS2_BLUE_WHITE	35
#define			IMAGE_ID_SETTINGS3_LRSTYLE		36
#define			IMAGE_ID_TOOL_LRSTYLE			37
#define			IMAGE_ID_HUMAN_SETUP_BLUEWHITE	38
#define			IMAGE_ID_HUMAN_SETUP_LRSTYLE	39
#define			IMAGE_ID_STAR_BLUE				40
#define			IMAGE_ID_CHECKMARK_BLUE			41
#define			IMAGE_ID_STAR_WHITE				42
#define			IMAGE_ID_CHECKMARK_WHITE		43
#define			IMAGE_ID_QUESTIONCIRCLE_WHITE	44
#define			IMAGE_ID_QUESTIONCIRCLE_BLUE	45
#define			IMAGE_ID_STARCIRCLE_WHITE		46
#define			IMAGE_ID_STARCIRCLE_BLUE		47
#define			IMAGE_ID_LOCK_LRSTYLE			48
#define			IMAGE_ID_LOCATION_LRSTYLE		49
#define			IMAGE_ID_PRINTER_LRSTYLE		50
#define			IMAGE_ID_LISTINGITEMS_LRSTYLE	51
#define			IMAGE_ID_WORLD_LRSTYLE			52
#define			IMAGE_ID_GLOBE_LRSTYLE			53
#define			IMAGE_ID_BATTERY_LOADING		54
#define			IMAGE_ID_BATTERY_100PERC		55//	/|			
#define			IMAGE_ID_BATTERY_75PERC			56//   / |____		
#define			IMAGE_ID_BATTERY_50PERC			57//  <   ____| NOTE: If used in conjunction with a level indicator, the displayed image depends on the level of the control.
#define			IMAGE_ID_BATTERY_25PERC			58//   \ |
#define			IMAGE_ID_BATTERY_EMPTY			59//    \|  
#define			IMAGE_ID_BATTERY_EMPTY_RED		60
#define			IMAGE_ID_RECTANGLE_EMPTY		61
#define			IMAGE_ID_RECTANGLE_CHECKED		62
#define			IMAGE_ID_RECTANGLE_CROSSED		63
#define			IMAGE_ID_CIRCLE_EMPTY			64
#define			IMAGE_ID_CIRCLE_CHECKED			65
#define			IMAGE_ID_CIRCLE_CROSSED			66
#define			IMAGE_ID_CLOUD					67
#define			IMAGE_ID_CLOUD_LRSTYLE			68
#define			IMAGE_ID_HOME_BLUE				69
#define			IMAGE_ID_HOME_WHITE				70
#define			IMAGE_ID_HOME_LRSTYLE			71
#define			IMAGE_ID_SHARE					72
#define			IMAGE_ID_WIFI					73
#define			IMAGE_ID_CALCULATOR				74
#define			IMAGE_ID_PEOPLE					75
#define			IMAGE_ID_SEARCH					76
#define			IMAGE_ID_HIERACHY				77
#define			IMAGE_ID_DOUBLE_HELIX			78
#define			IMAGE_ID_AT_BLUE				79
#define			IMAGE_ID_AT_WHITE				80
#define			IMAGE_ID_ONE_CIRCLE_BLUE		81
#define			IMAGE_ID_TWO_CIRCLE_BLUE		82
#define			IMAGE_ID_THREE_CIRCLE_BLUE		83
#define			IMAGE_ID_ONE_CIRCLE_WHITE		84
#define			IMAGE_ID_TWO_CIRCLE_WHITE		85
#define			IMAGE_ID_THREE_CIRCLE_WHITE		86
#define			IMAGE_ID_ARROW_UP_WHITE			87
#define			IMAGE_ID_ARROW_RIGHT_WHITE		88
#define			IMAGE_ID_ARROW_DOWN_WHITE		89
#define			IMAGE_ID_ARROW_LEFT_WHITE		90
#define			IMAGE_ID_ARROW_UP_BLUE			91
#define			IMAGE_ID_ARROW_RIGHT_BLUE		92
#define			IMAGE_ID_ARROW_DOWN_BLUE		93
#define			IMAGE_ID_ARROW_LEFT_BLUE		94
#define			IMAGE_ID_TV_WHITE				95
#define			IMAGE_ID_TV_BLUEWHITE			96
#define			IMAGE_ID_ARROW_UPDOWN			97
#define			IMAGE_ID_ARROW_LEFTRIGHT		98
#define			IMAGE_ID_HAND					99
#define			IMAGE_ID_INFOCIRCLE				100
#define			IMAGE_ID_STOPSIGN				101
#define			IMAGE_ID_SHIELD_POWER			102
#define			IMAGE_ID_SHIELD_OK				103
#define			IMAGE_ID_SHIELD_EXCLAMATION		104
#define			IMAGE_ID_LOCK_UNLOCK_LRSTYLE	105
#define			IMAGE_ID_KEY_WHITE				106
#define			IMAGE_ID_KEY_BLUE				107
#define			IMAGE_ID_FACEID					108
#define			IMAGE_ID_UNDO_WHITE				109
#define			IMAGE_ID_REDO_WHITE				110
#define			IMAGE_ID_FLASH_BLUE				111
#define			IMAGE_ID_FLASH_YELLOW			112

#endif /* IMAGEID_H_ */