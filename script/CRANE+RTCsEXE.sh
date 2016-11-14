#!/bin/sh

COM="gnome-terminal -e"

echo ""
echo "RT_CRANE_RTCとManipulatorControlSampleを実行します。"
echo "CRANE+とパソコンを接続し，CRANE+の電源をつけてください。"
echo ""

#コンポーネント実行
$COM "../RT_CRANE_RTC/build/src/RT_CRANE_RTCComp -f ../RT_CRANE_RTC/rtc.conf" -t RT_CRANE_RTC
$COM "../ManipulatorControlSample/build/src/ManipulatorControlSampleComp -f ../ManipulatorControlSample/rtc.conf" -t ManipulatorControlSample