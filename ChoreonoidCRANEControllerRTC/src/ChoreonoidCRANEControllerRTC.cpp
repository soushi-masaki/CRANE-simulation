// -*- C++ -*-
/*!
 * @file  ChoreonoidCRANEControllerRTC.cpp
 * @brief ChoreonoidCRANEControllerRTC
 * @date $Date$
 *
 * $Id$
 */

#include "ChoreonoidCRANEControllerRTC.h"
#include "Crane.h"
#include "ChoreonoidControl.h"

// Module specification
// <rtc-template block="module_spec">
static const char* choreonoidcranecontrollerrtc_spec[] =
  {
    "implementation_id", "ChoreonoidCRANEControllerRTC",
    "type_name",         "ChoreonoidCRANEControllerRTC",
    "description",       "ChoreonoidCRANEControllerRTC",
    "version",           "1.0.0",
    "vendor",            "rsdlab",
    "category",          "Category",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    // Configuration variables
    "conf.default.Mode", "0",
    "conf.default.port_name", "/dev/ttyUSB0",

    // Widget
    "conf.__widget__.Mode", "radio",
    "conf.__widget__.port_name", "text",
    // Constraints
    "conf.__constraints__.Mode", "(0,1,2)",

    "conf.__type__.Mode", "short",
    "conf.__type__.port_name", "string",

    ""
  };
// </rtc-template>
double targetPos[5];
double oldPos[5];
double JointPos[ARM_FREEDOM-1];
const double pgain[5] = {20, 30, 20, 15, 15};
const double dgain[5] = {10, 10, 10, 10, 10};


/*!
 * @brief constructor
 * @param manager Maneger Object
 */
ChoreonoidCRANEControllerRTC::ChoreonoidCRANEControllerRTC(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_angleIn("angle", m_angle),
    m_torqueOut("torque", m_torque),
    m_ManipulatorCommonInterface_CommonPort("ManipulatorCommonInterface_Common"),
    m_ManipulatorCommonInterface_MiddlePort("ManipulatorCommonInterface_Middle")

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
ChoreonoidCRANEControllerRTC::~ChoreonoidCRANEControllerRTC()
{
}



RTC::ReturnCode_t ChoreonoidCRANEControllerRTC::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("angle", m_angleIn);
  
  // Set OutPort buffer
  addOutPort("torque", m_torqueOut);
  
  // Set service provider to Ports
  m_ManipulatorCommonInterface_CommonPort.registerProvider("ManipulatorCommonInterface_Common", "JARA_ARM::ManipulatorCommonInterface_Common", m_ManipulatorCommonInterface_Common);
  m_ManipulatorCommonInterface_MiddlePort.registerProvider("ManipulatorCommonInterface_Middle", "JARA_ARM::ManipulatorCommonInterface_Middle", m_ManipulatorCommonInterface_Middle);
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  addPort(m_ManipulatorCommonInterface_CommonPort);
  addPort(m_ManipulatorCommonInterface_MiddlePort);
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("Mode", m_Mode, "0");
  bindParameter("port_name", m_port_name, "/dev/ttyUSB0");
  // </rtc-template>
  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t ChoreonoidCRANEControllerRTC::onFinalize()
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ChoreonoidCRANEControllerRTC::onStartup(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ChoreonoidCRANEControllerRTC::onShutdown(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/


RTC::ReturnCode_t ChoreonoidCRANEControllerRTC::onActivated(RTC::UniqueId ec_id)
{
  usleep(10000);
  c_Mode = m_Mode;
  if(c_Mode==1||c_Mode==2){
  crane.openSerialPort(m_port_name.c_str());
  std::cout << "openSerialPort" << std::endl;
  crane.initArm();
  std::cout << "initArm" << std::endl;
  }

  simcode = 101;

  m_torque.data.length(5);
  m_angleIn.read();
  
  for(size_t i=0; i < m_angle.data.length(); ++i)
    {
      targetPos[i] =  m_angle.data[i];
      oldPos[i] = targetPos[i]; 
    }
  return RTC::RTC_OK;
}


RTC::ReturnCode_t ChoreonoidCRANEControllerRTC::onDeactivated(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t ChoreonoidCRANEControllerRTC::onExecute(RTC::UniqueId ec_id)
{
  if(c_Mode == 0||c_Mode == 2)
    {
      ActCommand();
    }

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t ChoreonoidCRANEControllerRTC::onAborting(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ChoreonoidCRANEControllerRTC::onError(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ChoreonoidCRANEControllerRTC::onReset(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ChoreonoidCRANEControllerRTC::onStateUpdate(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

/*
RTC::ReturnCode_t ChoreonoidCRANEControllerRTC::onRateChanged(RTC::UniqueId ec_id)
{
  return RTC::RTC_OK;
}
*/

RTC::ReturnCode_t ChoreonoidCRANEControllerRTC::ActCommand()
{
  double nowPos[5];
  double deltaPos[5];
  double velocity[5];
  double rad;
  double deg;
  
  rad = 180/M_PI;    //radをdegへ
  deg = M_PI/180;    //degをradへ

switch(simcode)
    {

    case(101)://ServoON or 現在位置の維持に使用

      m_angleIn.read();
      for(size_t i=0; i < m_angle.data.length(); ++i)
	{
	  nowPos[i] =  m_angle.data[i];
	}
      
      for(size_t j=0; j < m_torque.data.length(); ++j)
	{
	  deltaPos[j] = targetPos[j] - nowPos[j];
	  velocity[j] = (nowPos[j]-oldPos[j])/(0.001);
	  m_torque.data[j] =  deltaPos[j]*pgain[j] + (0.0 - velocity[j])*dgain[j];
	  oldPos[j] = nowPos[j];
	}

      m_torqueOut.write();
      break;

    case(102)://ServoOFF
      break;

    case(201)://closeGripper

      targetPos[4] = (Hand_LimitMin - 150)*M_PI/180;

      m_angleIn.read();
      for(size_t i=0; i < m_angle.data.length(); ++i)
	{
	  nowPos[i] =  m_angle.data[i];
	}
      
      for(size_t j=0; j < m_torque.data.length(); ++j)
	{
	  deltaPos[j] = targetPos[j] - nowPos[j];
	  velocity[j] = (nowPos[j]-oldPos[j])/(0.001);
	  m_torque.data[j] =  deltaPos[j]*pgain[j] + (0.0 - velocity[j])*dgain[j];
	  oldPos[j] = nowPos[j];
	}
      
      m_torqueOut.write();
      break;

    case(202)://getFeedbackPosCartesian

      double x;
      double y;
      double z;
      double Rz;
        
      m_angleIn.read();
      std::cout << "シミュレータのアーム" << std::endl;
      for(size_t i=0; i < m_angle.data.length(); ++i)
	{
	  nowPos[i] =  m_angle.data[i];
	  std::cout << "nowPos["<<i<<"] = " << nowPos[i]*rad << "[deg]" <<  std::endl;
	} 

      x = cos(nowPos[0]) * (r1*cos(nowPos[1] + M_PI/2) + r2*cos(nowPos[1] + M_PI/2 + nowPos[2]) + r3*cos(nowPos[1] + M_PI/2 + nowPos[2] + nowPos[3]))+14.5;//[mm]
      y = sin(nowPos[0]) * (r1*cos(nowPos[1] + M_PI/2) + r2*cos(nowPos[1] + M_PI/2 + nowPos[2]) + r3*cos(nowPos[1] + M_PI/2 + nowPos[2] + nowPos[3]));//[mm]
      z =r1*sin(nowPos[1] + M_PI/2) + r2*sin(nowPos[1] + M_PI/2 + nowPos[2]) + r3*sin(nowPos[1] + M_PI/2 + nowPos[2] + nowPos[3])+45.4;//[mm]
		
      Rz = (nowPos[1]+M_PI/2 + nowPos[2] + nowPos[3]);//[rad]

      std::cout<<" x  : "<< x << "[mm]" <<std::endl;
      std::cout<<" y  : "<< y << "[mm]" <<std::endl;
      std::cout<<" z  : "<< z << "[mm]" <<std::endl;
      std::cout<<" Rz : "<< Rz*rad << "[deg]" << std::endl;
      
      simcode = 101;
      break;

    case(203)://moveGripper
      targetPos[4] = (Hand_LimitMax-Hand_LimitMin)*(C_angleRatio*0.01)+Hand_LimitMin;//angleRatioは%で代入、Minのほうがサーボモーターに送る値は大きい
      targetPos[4] = (targetPos[4]-150)*deg;
      m_angleIn.read();
      for(size_t i=0; i < m_angle.data.length(); ++i)
	{
	  nowPos[i] =  m_angle.data[i];
	}
      
      for(size_t j=0; j < m_torque.data.length(); ++j)
	{
	  deltaPos[j] = targetPos[j] - nowPos[j];
	  velocity[j] = (nowPos[j]-oldPos[j])/(0.001);
	  m_torque.data[j] =  deltaPos[j]*pgain[j] + (0.0 - velocity[j])*pgain[j];
	  oldPos[j] = nowPos[j];
	}
      
      m_torqueOut.write();
      break;

    case(204)://moveLinearCartesianAbs

      crane.kinematics(C_carPos[0][3],C_carPos[1][3],C_carPos[2][3],JointPos);
        
      targetPos[0] =JointPos[0]*deg;
      targetPos[1] =JointPos[1]*deg - M_PI/2; //地面からの角度で計算されるため，アームの初期位置からの角度で計算するように変換
      targetPos[2] =JointPos[2]*deg;
      targetPos[3] =JointPos[3]*deg;

      m_angleIn.read();
      for(size_t i=0; i < m_angle.data.length(); ++i)
	{
	  nowPos[i] =  m_angle.data[i];
	}
      
      for(size_t j=0; j < m_torque.data.length(); ++j)
	{
	  deltaPos[j] = targetPos[j] - nowPos[j];
	  velocity[j] = (nowPos[j]-oldPos[j])/(0.001);
	  m_torque.data[j] =  deltaPos[j]*pgain[j] + (0.0 - velocity[j])*dgain[j];
	  oldPos[j] = nowPos[j];
	}
      
      m_torqueOut.write();
      break;

    case(205)://openGripper

     targetPos[4] = (Hand_LimitMax - 150)*M_PI/180;

      m_angleIn.read();
      for(size_t i=0; i < m_angle.data.length(); ++i)
	{
	  nowPos[i] =  m_angle.data[i];
	}
      
      for(size_t j=0; j < m_torque.data.length(); ++j)
	{
	  deltaPos[j] = targetPos[j] - nowPos[j];
	  velocity[j] = (nowPos[j]-oldPos[j])/(0.001);
	  m_torque.data[j] =  deltaPos[j]*pgain[j] + (0.0 - velocity[j])*dgain[j];
	  oldPos[j] = nowPos[j];
	}
      
      m_torqueOut.write();
      break;

    case(206)://movePTPJointAbs
      targetPos[0] = (C_jointPoint[0])*M_PI/180;//地面からの角度が入力されるため，アームの初期位置からの角度で計算するように変換
      targetPos[1] = -C_jointPoint[1]*M_PI/180;
      targetPos[2] = -C_jointPoint[2]*M_PI/180;
      targetPos[3] = -C_jointPoint[3]*M_PI/180;
      m_angleIn.read();
      for(size_t i=0; i < m_angle.data.length(); ++i)
	{
	  nowPos[i] =  m_angle.data[i];
	}
      
      for(size_t j=0; j < m_torque.data.length(); ++j)
	{
	  deltaPos[j] = targetPos[j] - nowPos[j];
	  velocity[j] = (nowPos[j]-oldPos[j])/(0.001);
	  m_torque.data[j] =  deltaPos[j]*pgain[j] + (0.0 - velocity[j])*dgain[j];
	  oldPos[j] = nowPos[j];
	}
      
      m_torqueOut.write();
      break;
      
 case(207)://goHome
   
      targetPos[0] = HomeMotorPosition[0]*deg;
      targetPos[1] = HomeMotorPosition[1]*deg - M_PI/2;
      targetPos[2] = HomeMotorPosition[2]*deg;
      targetPos[3] = HomeMotorPosition[3]*deg;

      m_angleIn.read();
      for(size_t i=0; i < m_angle.data.length(); ++i)
	{
	  nowPos[i] =  m_angle.data[i];
	}
      
      for(size_t j=0; j < m_torque.data.length(); ++j)
	{
	  deltaPos[j] = targetPos[j] - nowPos[j];
	  velocity[j] = (nowPos[j]-oldPos[j])/(0.001);
	  m_torque.data[j] =  deltaPos[j]*pgain[j] + (0.0 - velocity[j])*dgain[j];
	  oldPos[j] = nowPos[j];
	}
      
      m_torqueOut.write();
      break;
   
    }

  return RTC::RTC_OK;
}


extern "C"
{
 
  void ChoreonoidCRANEControllerRTCInit(RTC::Manager* manager)
  {
    coil::Properties profile(choreonoidcranecontrollerrtc_spec);
    manager->registerFactory(profile,
                             RTC::Create<ChoreonoidCRANEControllerRTC>,
                             RTC::Delete<ChoreonoidCRANEControllerRTC>);
  }
  
};

