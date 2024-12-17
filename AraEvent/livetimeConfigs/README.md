# Livetime configuration logs

## Recommended usage
- Files should follow the following naming convention: a${station}_livetimeConfigs.txt
- Files should be comma separated

## File content
- The file contain five columns:
  - `configStart`: The run number at which the configuration begins (inclusive) in ascending order
  - `configNum`: The corresponding livetime configuration number (should correspond to the config number used for noise and gain models)
  - `repYear`: A representative year during which the station was in this configuration (in order to ensure the appropriate AraRoot SQL database is applied). Each `configNum` should have a unique `repYear` associated with it.
  - `trigWind`: The corresponding trigger window size in this configuration. 10 = 110 ns, 16 = 170 ns, 30 = 310 ns etc.
  - `blkWind`: The corresponding RF trigger's readout window length (number of blocks used) in this configuration. 20 ~ 400 ns, 26 ~ 520 ns, 30 ~ 600 ns etc.
- The file should have a header to label each column: `RunNo,Config,RepYear,TriggerWindow,ReadoutWindow`
- All file entries should be non-negative integers
- Example:
 
          RunNo,Config,RepYear,TriggerWindow,ReadoutWindow  
          0,1,2018,10,20  
          135,2,2019,16,26  
          233,3,2021,30,26  
          501,1,2018,10,28 
  - The above file would correspond to the station being in:
    - Config 1 for runs 0-134 with trigger window 110 ns and RF readout window 400 ns
    - Config 2 for runs 135-232 with trigger window 170 ns and RF readout window 520 ns
    - Config 3 for runs 233-500 with trigger window 310 ns and RF readout window 520 ns
    - Config 1 for runs >= 501 with trigger window 110 ns and RF readout window 560 ns
 
