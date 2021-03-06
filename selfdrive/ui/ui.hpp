#pragma once

#include <atomic>
#include <map>
#include <memory>
#include <string>
#include <sstream>

#include "nanovg.h"

#include "camerad/cameras/camera_common.h"
#include "common/mat.h"
#include "common/visionimg.h"
#include "common/modeldata.h"
#include "common/params.h"
#include "common/glutil.h"
#include "common/util.h"
#include "common/transformations/orientation.hpp"
#include "messaging.hpp"
#include "visionipc.h"
#include "visionipc_client.h"

#include "qt/sound.hpp"

#include <QObject>
#include <QTimer>

#define COLOR_BLACK nvgRGBA(0, 0, 0, 255)
#define COLOR_BLACK_ALPHA(x) nvgRGBA(0, 0, 0, x)
#define COLOR_WHITE nvgRGBA(255, 255, 255, 255)
#define COLOR_WHITE_ALPHA(x) nvgRGBA(255, 255, 255, x)
#define COLOR_YELLOW nvgRGBA(218, 202, 37, 255)
#define COLOR_RED nvgRGBA(201, 34, 49, 255)
#define COLOR_OCHRE nvgRGBA(218, 111, 37, 255)
#define COLOR_OCHRE_ALPHA(x) nvgRGBA(218, 111, 37, x)
#define COLOR_GREEN nvgRGBA(0, 255, 0, 255)
#define COLOR_GREEN_ALPHA(x) nvgRGBA(0, 255, 0, x)
#define COLOR_BLUE nvgRGBA(0, 0, 255, 255)
#define COLOR_BLUE_ALPHA(x) nvgRGBA(0, 0, 255, x)
#define COLOR_ORANGE nvgRGBA(255, 175, 3, 255)
#define COLOR_ORANGE_ALPHA(x) nvgRGBA(255, 175, 3, x)
#define COLOR_RED_ALPHA(x) nvgRGBA(201, 34, 49, x)
#define COLOR_YELLOW_ALPHA(x) nvgRGBA(218, 202, 37, x)
#define COLOR_GREY nvgRGBA(191, 191, 191, 1)

typedef struct Rect {
  int x, y, w, h;
  int centerX() const { return x + w / 2; }
  int centerY() const { return y + h / 2; }
  int right() const { return x + w; }
  int bottom() const { return y + h; }
  bool ptInRect(int px, int py) const {
    return px >= x && px < (x + w) && py >= y && py < (y + h);
  }
} Rect;

const int bdr_s = 15;
const int header_h = 420;
const int footer_h = 280;
const Rect settings_btn = {50, 35, 200, 117};
const Rect home_btn = {60, 1080 - 180 - 40, 180, 180};
const Rect map_overlay_btn = {0, 465, 150, 150};
const Rect map_btn = {1585, 905, 140, 140};
const Rect rec_btn = {1745, 905, 140, 140};
const Rect laneless_btn = {1425, 905, 140, 140};
const Rect monitoring_btn = {50, 830, 140, 140};
const Rect ml_btn = {1265, 905, 140, 140};

const int UI_FREQ = 20;   // Hz

typedef enum NetStatus {
  NET_CONNECTED,
  NET_DISCONNECTED,
  NET_ERROR,
} NetStatus;

typedef enum UIStatus {
  STATUS_OFFROAD,
  STATUS_DISENGAGED,
  STATUS_ENGAGED,
  STATUS_WARNING,
  STATUS_ALERT,
} UIStatus;

static std::map<UIStatus, NVGcolor> bg_colors = {
  {STATUS_OFFROAD, nvgRGBA(0x0, 0x0, 0x0, 0xff)},
  {STATUS_DISENGAGED, nvgRGBA(0x17, 0x33, 0x49, 0xc8)},
  {STATUS_ENGAGED, nvgRGBA(0x17, 0x86, 0x44, 0xf1)},
  {STATUS_WARNING, nvgRGBA(0xDA, 0x6F, 0x25, 0xf1)},
  {STATUS_ALERT, nvgRGBA(0xC9, 0x22, 0x31, 0xf1)},
};

typedef struct {
  float x, y;
} vertex_data;

typedef struct {
  vertex_data v[TRAJECTORY_SIZE * 2];
  int cnt;
} line_vertices_data;

typedef struct UIScene {

  bool mlButtonEnabled;

  mat3 view_from_calib;
  bool world_objects_visible;

  bool is_rhd;
  bool driver_view;

  std::string alert_text1;
  std::string alert_text2;
  std::string alertTextMsg1;
  std::string alertTextMsg2;
  std::string alert_type;
  float alert_blinking_rate;
  cereal::ControlsState::AlertSize alert_size;

  cereal::PandaState::PandaType pandaType;

  bool brakePress;
  bool recording;
  bool touched;
  bool map_on_top;

  float gpsAccuracyUblox;
  float altitudeUblox;
  float bearingUblox;

  int cpuPerc;
  float cpuTemp;
  bool rightblindspot;
  bool leftblindspot;
  bool leftBlinker;
  bool rightBlinker;
  int blinker_blinkingrate;
  int blindspot_blinkingrate = 120;
  int car_valid_status_changed = 0;
  float angleSteers;
  float steerRatio;
  bool brakeLights;
  float angleSteersDes;
  float curvature;
  bool steerOverride;
  float output_scale; 
  int batteryPercent;
  bool batteryCharging;
  char batteryStatus[64];
  char ipAddr[20];
  int fanSpeed;
  float tpmsPressureFl;
  float tpmsPressureFr;
  float tpmsPressureRl;
  float tpmsPressureRr;
  int lateralControlMethod;
  float radarDistance;
  int long_plan_source;
  bool standStill;
  float limitSpeedCamera;
  float limitSpeedCameraDist;
  float vSetDis;
  bool cruiseAccStatus;
  int laneless_mode;
  int recording_count;
  int recording_quality;
  float steerMax_V;
  int speed_lim_off;
  int monitoring_mode;
  int setbtn_count;
  int homebtn_count;
  bool forceGearD;

  NetStatus athenaStatus;

  cereal::DeviceState::Reader deviceState;
  cereal::RadarState::LeadData::Reader lead_data[2];
  cereal::CarState::Reader car_state;
  cereal::ControlsState::Reader controls_state;
  cereal::DriverState::Reader driver_state;
  cereal::DriverMonitoringState::Reader dmonitoring_state;

  cereal::CarState::GearShifter getGearShifter;
  cereal::LateralPlan::Reader lateral_plan;

  // gps
  int satelliteCount;
  bool gpsOK;

  // modelV2
  float lane_line_probs[4];
  float road_edge_stds[2];
  line_vertices_data track_vertices;
  line_vertices_data lane_line_vertices[4];
  line_vertices_data road_edge_vertices[2];

  // lead
  vertex_data lead_vertices[2];

  float light_sensor, accel_sensor, gyro_sensor;
  bool started, ignition, is_metric, longitudinal_control, end_to_end, model_long;
  uint64_t started_frame;


  // atom
  struct _screen
  {
     int  nTime;
     int  autoScreenOff;
     int  brightness;
     int  nVolumeBoost;
     int  awake;
  } scr;

  struct _LiveParams
  {
    float angleOffset;
    float angleOffsetAverage;
    float stiffnessFactor;
    float steerRatio;
  } liveParams;

  struct _LateralPlan
  {
    float laneWidth;
    float steerRateCost;
    int standstillElapsedTime = 0;

    float dProb;
    float lProb;
    float rProb;

    float angleOffset;
    bool lanelessModeStatus;
    float steerActuatorDelay;
  } lateralPlan;
} UIScene;

typedef struct UIState {
  VisionIpcClient * vipc_client;
  VisionIpcClient * vipc_client_front;
  VisionIpcClient * vipc_client_rear;
  VisionBuf * last_frame;

  // framebuffer
  int fb_w, fb_h;

  // NVG
  NVGcontext *vg;

  // images
  std::map<std::string, int> images;

  std::unique_ptr<SubMaster> sm;

  std::unique_ptr<Sound> sound;
  UIStatus status;
  UIScene scene;

  // graphics
  std::unique_ptr<GLShader> gl_shader;
  std::unique_ptr<EGLImageTexture> texture[UI_BUF_COUNT];

  GLuint frame_vao[2], frame_vbo[2], frame_ibo[2];
  mat4 rear_frame_mat, front_frame_mat;

  bool awake;

  bool is_speed_over_limit;
  bool is_OpenpilotViewEnabled;
  bool nDebugUi1;
  bool nDebugUi2;
  bool nOpkrBlindSpotDetect;
  bool driving_record;
  bool sidebar_view;

  Rect video_rect, viz_rect;
  float car_space_transform[6];
  bool wide_camera;
  float zoom;
} UIState;


class QUIState : public QObject {
  Q_OBJECT

public:
  QUIState(QObject* parent = 0);

  // TODO: get rid of this, only use signal
  inline static UIState ui_state = {0};

signals:
  void uiUpdate(const UIState &s);
  void offroadTransition(bool offroad);

private slots:
  void update();

private:
  QTimer *timer;
  bool started_prev = true;
};


// device management class

class Device : public QObject {
  Q_OBJECT

public:
  Device(QObject *parent = 0);

private:
  // auto brightness
  const float accel_samples = 5*UI_FREQ;

  bool awake;
  int awake_timeout = 300;
  float accel_prev = 0;
  float gyro_prev = 0;
  float brightness_b = 0;
  float brightness_m = 0;
  float last_brightness = 0;
  FirstOrderFilter brightness_filter;

  QTimer *timer;

  void updateBrightness(const UIState &s);
  void updateWakefulness(const UIState &s);
  void ScreenAwake();

signals:
  void displayPowerChanged(bool on);

public slots:
  void setAwake(bool on, bool reset);
  void update(const UIState &s);
};
