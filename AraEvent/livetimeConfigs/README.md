# Livetime configuration logs

## Recommended usage
- Files should follow the following naming convention: a${station}_livetimeConfigs.txt
- Files should be comma separated

## File content
- The file contain three columns:
  - `configStart`: The run number at which the configuration begins (inclusive) in ascending order
  - `configNum`: The corresponding livetime configuration number (should correspond to the config number used
                 for noise and gain models)
  - `year`: A representative year during which the station was in this configuration (e.g. station had the
            channel mappings assumed for this configuration). Each `configNum` should have a unique `year` associated with it.
- The file should have a header to label each column: `RunNo,Config`
- All file entries should be non-negative integers
- Example:  
          0,1,2018  
          135,2,2019  
          233,3,2021  
          501,1,2018  
  - The above file would correspond to the station being in:
    - Config 1 for runs 0-134
    - Config 2 for runs 135-232
    - Config 3 for runs 233-500
    - Config 1 for runs >= 501
 
