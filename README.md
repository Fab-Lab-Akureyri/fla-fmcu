# fmcu-fla

Clone the repo and update code to your needs.

## Set your labs ID

In the `.ino` file, insert your fablabs.io ID: 

```
const char id[]     = ""; 	// your fablabs.io lab id 
```

## Set SSID & Password

Edit `config.h` with the following lines and edit them accordingly. 

Changes to `config.h` are ignored so you don't accidentally upload your WIFI credentials.

```
#define WLAN_SSID ""    // Your wifi name
#define WLAN_PASS ""    // Your wifi password
```
