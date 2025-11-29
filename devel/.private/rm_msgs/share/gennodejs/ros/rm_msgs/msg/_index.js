
"use strict";

let TagMsg = require('./TagMsg.js');
let GimbalCmd = require('./GimbalCmd.js');
let ShootState = require('./ShootState.js');
let ShootCmd = require('./ShootCmd.js');
let LpData = require('./LpData.js');
let KalmanData = require('./KalmanData.js');
let LegCmd = require('./LegCmd.js');
let Dart = require('./Dart.js');
let ActuatorState = require('./ActuatorState.js');
let GpioData = require('./GpioData.js');
let GimbalPosState = require('./GimbalPosState.js');
let ChassisCmd = require('./ChassisCmd.js');
let VTReceiverControlData = require('./VTReceiverControlData.js');
let MovingAverageData = require('./MovingAverageData.js');
let DbusData = require('./DbusData.js');
let TofRadarData = require('./TofRadarData.js');
let ExchangerMsg = require('./ExchangerMsg.js');
let LocalHeatState = require('./LocalHeatState.js');
let SentryDeviate = require('./SentryDeviate.js');
let CustomControllerData = require('./CustomControllerData.js');
let ShootBeforehandCmd = require('./ShootBeforehandCmd.js');
let VTKeyboardMouseData = require('./VTKeyboardMouseData.js');
let GimbalDesError = require('./GimbalDesError.js');
let BalanceState = require('./BalanceState.js');
let MultiDofCmd = require('./MultiDofCmd.js');
let TagMsgArray = require('./TagMsgArray.js');
let BusState = require('./BusState.js');
let TargetDetection = require('./TargetDetection.js');
let TargetDetectionArray = require('./TargetDetectionArray.js');
let PriorityArray = require('./PriorityArray.js');
let TrackData = require('./TrackData.js');
let RadarMarkData = require('./RadarMarkData.js');
let ManualToReferee = require('./ManualToReferee.js');
let PowerManagementUnknownExceptionData = require('./PowerManagementUnknownExceptionData.js');
let CapacityData = require('./CapacityData.js');
let PowerManagementInitializationExceptionData = require('./PowerManagementInitializationExceptionData.js');
let SentryAttackingTarget = require('./SentryAttackingTarget.js');
let SentryInfo = require('./SentryInfo.js');
let GameRobotHp = require('./GameRobotHp.js');
let RobotHurt = require('./RobotHurt.js');
let EventData = require('./EventData.js');
let RadarInfo = require('./RadarInfo.js');
let PowerManagementSystemExceptionData = require('./PowerManagementSystemExceptionData.js');
let ClientMapSendData = require('./ClientMapSendData.js');
let RadarToSentry = require('./RadarToSentry.js');
let DartStatus = require('./DartStatus.js');
let Buff = require('./Buff.js');
let SupplyProjectileAction = require('./SupplyProjectileAction.js');
let GameRobotStatus = require('./GameRobotStatus.js');
let IcraBuffDebuffZoneStatus = require('./IcraBuffDebuffZoneStatus.js');
let StateCmd = require('./StateCmd.js');
let DartInfo = require('./DartInfo.js');
let ShootData = require('./ShootData.js');
let GameStatus = require('./GameStatus.js');
let RadarData = require('./RadarData.js');
let PowerManagementSampleAndStatusData = require('./PowerManagementSampleAndStatusData.js');
let SentryCmd = require('./SentryCmd.js');
let RfidStatus = require('./RfidStatus.js');
let BulletAllowance = require('./BulletAllowance.js');
let GameRobotPosData = require('./GameRobotPosData.js');
let VisualizeStateData = require('./VisualizeStateData.js');
let ClientMapReceiveData = require('./ClientMapReceiveData.js');
let PowerManagementProcessStackOverflowData = require('./PowerManagementProcessStackOverflowData.js');
let RobotsPositionData = require('./RobotsPositionData.js');
let PowerHeatData = require('./PowerHeatData.js');
let MapSentryData = require('./MapSentryData.js');
let EngineerUi = require('./EngineerUi.js');
let DartClientCmd = require('./DartClientCmd.js');
let EngineerActionResult = require('./EngineerActionResult.js');
let EngineerFeedback = require('./EngineerFeedback.js');
let EngineerActionFeedback = require('./EngineerActionFeedback.js');
let EngineerGoal = require('./EngineerGoal.js');
let EngineerAction = require('./EngineerAction.js');
let EngineerActionGoal = require('./EngineerActionGoal.js');
let EngineerResult = require('./EngineerResult.js');

module.exports = {
  TagMsg: TagMsg,
  GimbalCmd: GimbalCmd,
  ShootState: ShootState,
  ShootCmd: ShootCmd,
  LpData: LpData,
  KalmanData: KalmanData,
  LegCmd: LegCmd,
  Dart: Dart,
  ActuatorState: ActuatorState,
  GpioData: GpioData,
  GimbalPosState: GimbalPosState,
  ChassisCmd: ChassisCmd,
  VTReceiverControlData: VTReceiverControlData,
  MovingAverageData: MovingAverageData,
  DbusData: DbusData,
  TofRadarData: TofRadarData,
  ExchangerMsg: ExchangerMsg,
  LocalHeatState: LocalHeatState,
  SentryDeviate: SentryDeviate,
  CustomControllerData: CustomControllerData,
  ShootBeforehandCmd: ShootBeforehandCmd,
  VTKeyboardMouseData: VTKeyboardMouseData,
  GimbalDesError: GimbalDesError,
  BalanceState: BalanceState,
  MultiDofCmd: MultiDofCmd,
  TagMsgArray: TagMsgArray,
  BusState: BusState,
  TargetDetection: TargetDetection,
  TargetDetectionArray: TargetDetectionArray,
  PriorityArray: PriorityArray,
  TrackData: TrackData,
  RadarMarkData: RadarMarkData,
  ManualToReferee: ManualToReferee,
  PowerManagementUnknownExceptionData: PowerManagementUnknownExceptionData,
  CapacityData: CapacityData,
  PowerManagementInitializationExceptionData: PowerManagementInitializationExceptionData,
  SentryAttackingTarget: SentryAttackingTarget,
  SentryInfo: SentryInfo,
  GameRobotHp: GameRobotHp,
  RobotHurt: RobotHurt,
  EventData: EventData,
  RadarInfo: RadarInfo,
  PowerManagementSystemExceptionData: PowerManagementSystemExceptionData,
  ClientMapSendData: ClientMapSendData,
  RadarToSentry: RadarToSentry,
  DartStatus: DartStatus,
  Buff: Buff,
  SupplyProjectileAction: SupplyProjectileAction,
  GameRobotStatus: GameRobotStatus,
  IcraBuffDebuffZoneStatus: IcraBuffDebuffZoneStatus,
  StateCmd: StateCmd,
  DartInfo: DartInfo,
  ShootData: ShootData,
  GameStatus: GameStatus,
  RadarData: RadarData,
  PowerManagementSampleAndStatusData: PowerManagementSampleAndStatusData,
  SentryCmd: SentryCmd,
  RfidStatus: RfidStatus,
  BulletAllowance: BulletAllowance,
  GameRobotPosData: GameRobotPosData,
  VisualizeStateData: VisualizeStateData,
  ClientMapReceiveData: ClientMapReceiveData,
  PowerManagementProcessStackOverflowData: PowerManagementProcessStackOverflowData,
  RobotsPositionData: RobotsPositionData,
  PowerHeatData: PowerHeatData,
  MapSentryData: MapSentryData,
  EngineerUi: EngineerUi,
  DartClientCmd: DartClientCmd,
  EngineerActionResult: EngineerActionResult,
  EngineerFeedback: EngineerFeedback,
  EngineerActionFeedback: EngineerActionFeedback,
  EngineerGoal: EngineerGoal,
  EngineerAction: EngineerAction,
  EngineerActionGoal: EngineerActionGoal,
  EngineerResult: EngineerResult,
};
