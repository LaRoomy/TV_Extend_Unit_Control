/*
 * LaRoomy_XNG001.h
 *  Author: Hans Philipp Zimmermann
 *
 * Global Includes and Definitions
 *
 *		SEARCHKEY::MODIFY (for different devices)
 */

#ifndef LAROOMY_XNG001_H_
#define LAROOMY_XNG001_H_
//*************************************************************************************************************************************
//*************************************************************************************************************************************
#define LAROOMY_XNG001_BLUETOOTHNAME "Laroomy TVM\0"
#define LAROOMY_XNG001_AUTHENTICATIONSTRING "xPsM0-33wSp_mmT\0"
#define LAROOMY_XNG001_AUTHENTICATIONRESPONSE "Auth:rsp:true\0"
#define LAROOMY_XNG001_AUTHENTICATIONRESPONSE_BINDINGPASSKEYREQUIRED "Auth:rsp:bind\0"
#define LAROOMY_XNG001_AUTHENTICATIONRESPONSE_BINDINGPASSKEYINVALID "Auth:rsp:pkerr\0"
#define LAROOMY_XNG001_CONNECTIONTESTSTRING "vXtest385_26\0"

// #define		LAROOMY_XNG001_DEVICETYPEAUTHENTICATIONREQUEST	"rQST:DTP:ID$"
// #define		LAROOMY_XNG001_DEVICETYPEAUTHENTICATIONRESPONSE		"DtI:00000000000F$\0"

#define LAROOMY_DEVICEINFO_HEADER_START_ENTRY "DnDIHs7"
#define LAROOMY_DEVICEINFO_HEADER_ENDNOTIFICATION "DnDIHe+\0"
//*************************************************************************************************************************************
#define DEVICEBINDING_STATUS_REQUIRED 1
#define DEVICEBINDING_STATUS_NONE 0
//*************************************************************************************************************************************
#include "deviceProperty.h"
//*************************************************************************************************************************************
#include "../HM-19/HMxx_Command.h"
#include "CommandAnalyzer.h"
//*************************************************************************************************************************************
//		The Property Section...
#include "propertyDispatcher.h"
#include "propertyStateDispatcher.h"

//*************************************************************************************************************************************

#endif /* LAROOMY XNG001_H_ */