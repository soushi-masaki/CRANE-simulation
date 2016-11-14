#!/bin/sh

COM="gnome-terminal -e"

echo "ChoreonoidCRANEControllerRTCとManipulatorControlSampleを実行します。"
echo "Choreonoidでプロジェクトファイル[CARNEsimulation]を読み込んでください。"

#コンポーネント実行
$COM "../ChoreonoidCRANEControllerRTC/build/src/ChoreonoidCRANEControllerRTCComp -f ../ChoreonoidCRANEControllerRTC/rtc.conf" -t ChoreonoidCRANEControllerRTC
$COM "../ManipulatorControlSample/build/src/ManipulatorControlSampleComp -f ../ManipulatorControlSample/rtc.conf" -t ManipulatorControlSample
