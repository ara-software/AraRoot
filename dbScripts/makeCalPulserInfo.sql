CREATE TABLE TESTBED(antCalId integer primary key, holeName text, antName text, pulserName text, antType text, polType text, antLocationX real, antLocationY real, antLocationZ real, cableDelay real, calibAntLocationX real, calibAntLocationY real, calibAntLocationZ real, calibCableDelay real);
CREATE TABLE ARA01(antCalId integer primary key, holeName text, antName text, pulserName text, antType text, polType text, antLocationX real, antLocationY real, antLocationZ real, cableDelay real, calibAntLocationX real, calibAntLocationY real, calibAntLocationZ real, calibCableDelay real);

CREATE TABLE ARA02(antCalId integer primary key, holeName text, antName text, pulserName text, antType text, polType text, antLocationX real, antLocationY real, antLocationZ real, cableDelay real, calibAntLocationX real, calibAntLocationY real, calibAntLocationZ real, calibCableDelay real);

CREATE TABLE ARA03(antCalId integer primary key, holeName text, antName text, pulserName text, antType text, polType text, antLocationX real, antLocationY real, antLocationZ real, cableDelay real, calibAntLocationX real, calibAntLocationY real, calibAntLocationZ real, calibCableDelay real);

CREATE TABLE ARA04(antCalId integer primary key, holeName text, antName text, pulserName text, antType text, polType text, antLocationX real, antLocationY real, antLocationZ real, cableDelay real, calibAntLocationX real, calibAntLocationY real, calibAntLocationZ real, calibCableDelay real);

CREATE TABLE ARA05(antCalId integer primary key, holeName text, antName text, pulserName text, antType text, polType text, antLocationX real, antLocationY real, antLocationZ real, cableDelay real, calibAntLocationX real, calibAntLocationY real, calibAntLocationZ real, calibCableDelay real);

CREATE TABLE ARA06(antCalId integer primary key, holeName text, antName text, pulserName text, antType text, polType text, antLocationX real, antLocationY real, antLocationZ real, cableDelay real, calibAntLocationX real, calibAntLocationY real, calibAntLocationZ real, calibCableDelay real);
--TESTBED Calpulsers
--Ant position only supplied for the pulser not the individual antennas

insert into TESTBED VALUES(0,"BH5","CH1","P1","kQuadSlot","kHorizontal",
0.17,-30.01,-17.61,0,
0.17,-30.01,-17.61,0
);
insert into TESTBED VALUES(1,"BH5","CV1","P1","kBicone","kVertical",
0.17,-30.01,-22.61,0,
0.17,-30.01,-22.61,0
);
insert into TESTBED VALUES(2,"BH6","CH2","P2","kQuadSlot","kHorizontal",
25.86,15.31,-34.26,0,
25.86,15.31,-34.26,0
);
insert into TESTBED VALUES(3,"BH6","CV2","P2","kBicone","kVertical",
25.86,15.31,-29.26,0,
25.86,15.31,-29.26,0
);
--Stick with only the 4 calibration pulsers since the other two aren't 
--And the AraStationInfo code needs fixing to handle more

--Insert into TESTBED VALUES(4,"BH7","CH3","P3","kQuadSlot","kHorizontal",
---26.81,13.2,-0.4,0,
---26.81,13.2,-0.4,0
--);
--insert into TESTBED VALUES(5,"BH7","CV3","P3","kBicone","kVertical",
---26.45,14.75,-0.4,0,
---26.45,14.75,-0.4,0
--);






--ARA01 Calpulers
--Ant position only supplied for the pulser not the individual antennas
--delay unkown (and not needed?)

insert into ARA01 VALUES(0,"BH5","CH1","P1","kQuadSlot","kHorizontal",
16.41430296,34.8273624,-36.1239816,0,
16.41430296,34.8273624,-36.1239816,0);
insert into ARA01 VALUES(1,"BH5","CV1","P1","kBicone","kVertical",
16.41430296,34.8273624,-36.1239816,0,
16.41430296,34.8273624,-36.1239816,0);
insert into ARA01 VALUES(2,"BH6","CH2","P2","kQuadSlot","kHorizontal",
36.585144,-18.2905908,-42.141648,0,
36.585144,-18.2905908,-42.141648,0);
insert into ARA01 VALUES(3,"BH6","CV2","P2","kBicone","kVertical",
36.585144,-18.2905908,-42.141648,0,
6.585144,-18.2905908,-42.141648,0);  


--ARA02 Calpulers
--Ant positions from Amy

-- Upated by BAC Jan 24 2019 (see DocDB 1791)

insert into ARA02 VALUES(0,"BH5","CH1","P1","kQuadSlot","kHorizontal",
37.8585,-17.7704,-194.261,0,
37.8585,-17.7704,-194.261,0
);
insert into ARA02 VALUES(1,"BH5","CV1","P1","kBicone","kVertical",
37.8585,-17.7704,-197.546,0,
37.8585,-17.7704,-197.546,0
);
insert into ARA02 VALUES(2,"BH6","CH2","P2","kQuadSlot","kHorizontal",
21.0882,42.2156,-171.804,0,
21.0882,42.2156,-171.804,0
);

insert into ARA02 VALUES(3,"BH6","CV2","P2","kBicone","kVertical",
21.0882,42.2156,-175.089,0,
21.0882,42.2156,-175.089,0
);

--updated 15-07-13

--ARA03 Calpulers

-- Upated by BAC Jan 24 2019 (see DocDB 1791)

insert into ARA03 VALUES(0,"BH5","CH1","P1","kQuadSlot","kHorizontal",
38.2336,-16.2148,-189.992,0,
38.2336,-16.2148,-189.992,0
);
insert into ARA03 VALUES(1,"BH5","CV1","P1","kBicone","kVertical",
38.2336,-16.2148,-193.277,0,
38.2336,-16.2148,-193.277,0
);
insert into ARA03 VALUES(2,"BH6","CH2","P2","kQuadSlot","kHorizontal",
18.096,36.7114,-188.676,0,
18.096,36.7114,-188.676,0
);
insert into ARA03 VALUES(3,"BH6","CV2","P2","kBicone","kVertical",
18.096,36.7114,-191.961,0,
18.096,36.7114,-191.961,0
);

--updated 15-07-13

--ARA04 Calpulers
-- ARA04 Calpulsers updated by Paramita DG on 05/26/2024
insert into ARA04 VALUES(0,"BH5","CH1","P1","kQuadSlot","kHorizontal",
0,0,0,0,
0,0,0,0
);
insert into ARA04 VALUES(1,"BH5","CV1","P1","kBicone","kVertical",
0,0,0,0,
0,0,0,0
);
insert into ARA04 VALUES(2,"BH6","CH2","P2","kQuadSlot","kHorizontal",
49.73993866,-15.07675222,-184.4708,0,
49.73993866,-15.07675222,-184.4708,0
);
insert into ARA04 VALUES(3,"BH6","CV2","P2","kBicone","kVertical",
49.73993866,-15.07675222,-187.4708,0,
49.73993866,-15.07675222,-187.4708,0
);

--updated 17-05-17

--ARA04 Calpulers

insert into ARA05 VALUES(0,"BH5","CH1","P1","kQuadSlot","kHorizontal",
-16.4944,52.8174,-171.07,0,
-16.4944,52.8174,-171.07,0
);
insert into ARA05 VALUES(1,"BH5","CV1","P1","kBicone","kVertical",
-16.4944,52.8174,-174.06,0,
-16.4944,52.8174,-174.06,0
);
insert into ARA05 VALUES(2,"BH6","CH2","P2","kQuadSlot","kHorizontal",
0,0,0,0,
0,0,0,0
);
insert into ARA05 VALUES(3,"BH6","CV2","P2","kBicone","kVertical",
0,0,0,0,
0,0,0,0
);

--updated 17-05-17

--ARA04 Calpulers

insert into ARA06 VALUES(0,"BH5","CH1","P1","kQuadSlot","kHorizontal",
0,0,0,0,
0,0,0,0
);
insert into ARA06 VALUES(1,"BH5","CV1","P1","kBicone","kVertical",
0,0,0,0,
0,0,0,0
);
insert into ARA06 VALUES(2,"BH6","CH2","P2","kQuadSlot","kHorizontal",
0,0,0,0,
0,0,0,0
);
insert into ARA06 VALUES(3,"BH6","CV2","P2","kBicone","kVertical",
0,0,0,0,
0,0,0,0
);

--updated 17-05-17
