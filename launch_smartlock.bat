@echo off
title Smart Access Terminal Launcher
echo ==============================================================================
echo   ⚡ DSD HYBRID HARDWARE FSM SMART ACCESS TERMINAL
echo ==============================================================================
echo.
echo [1/2] Starting Mock ESP8266 REST API Server on http://127.0.0.1:8080...
start /b python mock_esp8266_server.py
timeout /t 1 /nobreak >nul
echo.
echo [2/2] Launching Smart Access Terminal Dashboard...
start fsm_supervisor_dashboard.html
echo.
echo ==============================================================================
echo   System running. Close this window or press Ctrl+C to stop the mock server.
echo ==============================================================================
pause
