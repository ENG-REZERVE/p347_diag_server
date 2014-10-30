#!/bin/sh
protoc -I=./ --cpp_out=./ ./channel_manager.proto ./hardware_monitor.proto ./task_manager.proto