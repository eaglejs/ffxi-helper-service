    # Example wrapper script (MyServiceWrapper.ps1)
    $exePath = "C:\Users\SL4X3\repos\ffxi-helper-service\build\FFXIHelperService_d.exe"
    $arguments = "" # Optional arguments for the EXE

    # Start-Process -FilePath $exePath -ArgumentList $arguments -NoNewWindow -Wait
    Start-Process -FilePath $exePath -NoNewWindow -Wait
