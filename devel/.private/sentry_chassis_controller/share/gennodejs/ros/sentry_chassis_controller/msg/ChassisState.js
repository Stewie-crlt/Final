// Auto-generated. Do not edit!

// (in-package sentry_chassis_controller.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;
let std_msgs = _finder('std_msgs');

//-----------------------------------------------------------

class ChassisState {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.header = null;
      this.cmd_vx = null;
      this.cmd_vy = null;
      this.cmd_wz = null;
      this.desired_wheel_speeds = null;
      this.actual_wheel_speeds = null;
      this.desired_steering_angles = null;
      this.actual_steering_angles = null;
      this.wheel_efforts = null;
      this.steering_efforts = null;
      this.cmd_vel_active = null;
    }
    else {
      if (initObj.hasOwnProperty('header')) {
        this.header = initObj.header
      }
      else {
        this.header = new std_msgs.msg.Header();
      }
      if (initObj.hasOwnProperty('cmd_vx')) {
        this.cmd_vx = initObj.cmd_vx
      }
      else {
        this.cmd_vx = 0.0;
      }
      if (initObj.hasOwnProperty('cmd_vy')) {
        this.cmd_vy = initObj.cmd_vy
      }
      else {
        this.cmd_vy = 0.0;
      }
      if (initObj.hasOwnProperty('cmd_wz')) {
        this.cmd_wz = initObj.cmd_wz
      }
      else {
        this.cmd_wz = 0.0;
      }
      if (initObj.hasOwnProperty('desired_wheel_speeds')) {
        this.desired_wheel_speeds = initObj.desired_wheel_speeds
      }
      else {
        this.desired_wheel_speeds = new Array(4).fill(0);
      }
      if (initObj.hasOwnProperty('actual_wheel_speeds')) {
        this.actual_wheel_speeds = initObj.actual_wheel_speeds
      }
      else {
        this.actual_wheel_speeds = new Array(4).fill(0);
      }
      if (initObj.hasOwnProperty('desired_steering_angles')) {
        this.desired_steering_angles = initObj.desired_steering_angles
      }
      else {
        this.desired_steering_angles = new Array(4).fill(0);
      }
      if (initObj.hasOwnProperty('actual_steering_angles')) {
        this.actual_steering_angles = initObj.actual_steering_angles
      }
      else {
        this.actual_steering_angles = new Array(4).fill(0);
      }
      if (initObj.hasOwnProperty('wheel_efforts')) {
        this.wheel_efforts = initObj.wheel_efforts
      }
      else {
        this.wheel_efforts = new Array(4).fill(0);
      }
      if (initObj.hasOwnProperty('steering_efforts')) {
        this.steering_efforts = initObj.steering_efforts
      }
      else {
        this.steering_efforts = new Array(4).fill(0);
      }
      if (initObj.hasOwnProperty('cmd_vel_active')) {
        this.cmd_vel_active = initObj.cmd_vel_active
      }
      else {
        this.cmd_vel_active = false;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type ChassisState
    // Serialize message field [header]
    bufferOffset = std_msgs.msg.Header.serialize(obj.header, buffer, bufferOffset);
    // Serialize message field [cmd_vx]
    bufferOffset = _serializer.float64(obj.cmd_vx, buffer, bufferOffset);
    // Serialize message field [cmd_vy]
    bufferOffset = _serializer.float64(obj.cmd_vy, buffer, bufferOffset);
    // Serialize message field [cmd_wz]
    bufferOffset = _serializer.float64(obj.cmd_wz, buffer, bufferOffset);
    // Check that the constant length array field [desired_wheel_speeds] has the right length
    if (obj.desired_wheel_speeds.length !== 4) {
      throw new Error('Unable to serialize array field desired_wheel_speeds - length must be 4')
    }
    // Serialize message field [desired_wheel_speeds]
    bufferOffset = _arraySerializer.float64(obj.desired_wheel_speeds, buffer, bufferOffset, 4);
    // Check that the constant length array field [actual_wheel_speeds] has the right length
    if (obj.actual_wheel_speeds.length !== 4) {
      throw new Error('Unable to serialize array field actual_wheel_speeds - length must be 4')
    }
    // Serialize message field [actual_wheel_speeds]
    bufferOffset = _arraySerializer.float64(obj.actual_wheel_speeds, buffer, bufferOffset, 4);
    // Check that the constant length array field [desired_steering_angles] has the right length
    if (obj.desired_steering_angles.length !== 4) {
      throw new Error('Unable to serialize array field desired_steering_angles - length must be 4')
    }
    // Serialize message field [desired_steering_angles]
    bufferOffset = _arraySerializer.float64(obj.desired_steering_angles, buffer, bufferOffset, 4);
    // Check that the constant length array field [actual_steering_angles] has the right length
    if (obj.actual_steering_angles.length !== 4) {
      throw new Error('Unable to serialize array field actual_steering_angles - length must be 4')
    }
    // Serialize message field [actual_steering_angles]
    bufferOffset = _arraySerializer.float64(obj.actual_steering_angles, buffer, bufferOffset, 4);
    // Check that the constant length array field [wheel_efforts] has the right length
    if (obj.wheel_efforts.length !== 4) {
      throw new Error('Unable to serialize array field wheel_efforts - length must be 4')
    }
    // Serialize message field [wheel_efforts]
    bufferOffset = _arraySerializer.float64(obj.wheel_efforts, buffer, bufferOffset, 4);
    // Check that the constant length array field [steering_efforts] has the right length
    if (obj.steering_efforts.length !== 4) {
      throw new Error('Unable to serialize array field steering_efforts - length must be 4')
    }
    // Serialize message field [steering_efforts]
    bufferOffset = _arraySerializer.float64(obj.steering_efforts, buffer, bufferOffset, 4);
    // Serialize message field [cmd_vel_active]
    bufferOffset = _serializer.bool(obj.cmd_vel_active, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type ChassisState
    let len;
    let data = new ChassisState(null);
    // Deserialize message field [header]
    data.header = std_msgs.msg.Header.deserialize(buffer, bufferOffset);
    // Deserialize message field [cmd_vx]
    data.cmd_vx = _deserializer.float64(buffer, bufferOffset);
    // Deserialize message field [cmd_vy]
    data.cmd_vy = _deserializer.float64(buffer, bufferOffset);
    // Deserialize message field [cmd_wz]
    data.cmd_wz = _deserializer.float64(buffer, bufferOffset);
    // Deserialize message field [desired_wheel_speeds]
    data.desired_wheel_speeds = _arrayDeserializer.float64(buffer, bufferOffset, 4)
    // Deserialize message field [actual_wheel_speeds]
    data.actual_wheel_speeds = _arrayDeserializer.float64(buffer, bufferOffset, 4)
    // Deserialize message field [desired_steering_angles]
    data.desired_steering_angles = _arrayDeserializer.float64(buffer, bufferOffset, 4)
    // Deserialize message field [actual_steering_angles]
    data.actual_steering_angles = _arrayDeserializer.float64(buffer, bufferOffset, 4)
    // Deserialize message field [wheel_efforts]
    data.wheel_efforts = _arrayDeserializer.float64(buffer, bufferOffset, 4)
    // Deserialize message field [steering_efforts]
    data.steering_efforts = _arrayDeserializer.float64(buffer, bufferOffset, 4)
    // Deserialize message field [cmd_vel_active]
    data.cmd_vel_active = _deserializer.bool(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += std_msgs.msg.Header.getMessageSize(object.header);
    return length + 217;
  }

  static datatype() {
    // Returns string type for a message object
    return 'sentry_chassis_controller/ChassisState';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return 'be09b6eddd3e724282cde2412802ba78';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    # 底盘状态消息
    Header header
    
    # 指令速度
    float64 cmd_vx
    float64 cmd_vy
    float64 cmd_wz
    
    # 轮子期望转速 (rad/s)
    float64[4] desired_wheel_speeds
    # 轮子实际转速 (rad/s)
    float64[4] actual_wheel_speeds
    
    # 轮子期望转向角度 (rad)
    float64[4] desired_steering_angles
    # 轮子实际转向角度 (rad)
    float64[4] actual_steering_angles
    
    # 轮子控制力矩
    float64[4] wheel_efforts
    # 转向控制力矩
    float64[4] steering_efforts
    
    # 是否接收到有效指令
    bool cmd_vel_active
    ================================================================================
    MSG: std_msgs/Header
    # Standard metadata for higher-level stamped data types.
    # This is generally used to communicate timestamped data 
    # in a particular coordinate frame.
    # 
    # sequence ID: consecutively increasing ID 
    uint32 seq
    #Two-integer timestamp that is expressed as:
    # * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')
    # * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')
    # time-handling sugar is provided by the client library
    time stamp
    #Frame this data is associated with
    string frame_id
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new ChassisState(null);
    if (msg.header !== undefined) {
      resolved.header = std_msgs.msg.Header.Resolve(msg.header)
    }
    else {
      resolved.header = new std_msgs.msg.Header()
    }

    if (msg.cmd_vx !== undefined) {
      resolved.cmd_vx = msg.cmd_vx;
    }
    else {
      resolved.cmd_vx = 0.0
    }

    if (msg.cmd_vy !== undefined) {
      resolved.cmd_vy = msg.cmd_vy;
    }
    else {
      resolved.cmd_vy = 0.0
    }

    if (msg.cmd_wz !== undefined) {
      resolved.cmd_wz = msg.cmd_wz;
    }
    else {
      resolved.cmd_wz = 0.0
    }

    if (msg.desired_wheel_speeds !== undefined) {
      resolved.desired_wheel_speeds = msg.desired_wheel_speeds;
    }
    else {
      resolved.desired_wheel_speeds = new Array(4).fill(0)
    }

    if (msg.actual_wheel_speeds !== undefined) {
      resolved.actual_wheel_speeds = msg.actual_wheel_speeds;
    }
    else {
      resolved.actual_wheel_speeds = new Array(4).fill(0)
    }

    if (msg.desired_steering_angles !== undefined) {
      resolved.desired_steering_angles = msg.desired_steering_angles;
    }
    else {
      resolved.desired_steering_angles = new Array(4).fill(0)
    }

    if (msg.actual_steering_angles !== undefined) {
      resolved.actual_steering_angles = msg.actual_steering_angles;
    }
    else {
      resolved.actual_steering_angles = new Array(4).fill(0)
    }

    if (msg.wheel_efforts !== undefined) {
      resolved.wheel_efforts = msg.wheel_efforts;
    }
    else {
      resolved.wheel_efforts = new Array(4).fill(0)
    }

    if (msg.steering_efforts !== undefined) {
      resolved.steering_efforts = msg.steering_efforts;
    }
    else {
      resolved.steering_efforts = new Array(4).fill(0)
    }

    if (msg.cmd_vel_active !== undefined) {
      resolved.cmd_vel_active = msg.cmd_vel_active;
    }
    else {
      resolved.cmd_vel_active = false
    }

    return resolved;
    }
};

module.exports = ChassisState;
