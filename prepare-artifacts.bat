@echo off

set version=1.0.0.2

7z a -mmt12 -mx9 -t7z -x!*.ini TouchSwitch.%version%.x64.Release.7z ./x64/Release/*
7z a -mmt12 -mx9 -t7z -x!*.ini TouchSwitch.%version%.x64.Debug.7z ./x64/Debug/*

7z a -mmt12 -mx9 -t7z -x!*.ini TouchSwitch.%version%.x86.Release.7z ./Release/*
7z a -mmt12 -mx9 -t7z -x!*.ini TouchSwitch.%version%.x86.Debug.7z ./Debug/*

7z a -mmt12 -mx9 -t7z -x!*.ini TouchSwitch.%version%.ARM64.Release.7z ./ARM64/Release/*
7z a -mmt12 -mx9 -t7z -x!*.ini TouchSwitch.%version%.ARM64.Debug.7z ./ARM64/Debug/*
