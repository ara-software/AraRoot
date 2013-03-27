CREATE TABLE ARA01(antCalId integer primary key, holeName text, antName text, pulserName text, antType text, polType text, antLocationX real, antLocationY real, antLocationZ real, cableDelay real, calibAntLocationX real, calibAntLocationY real, calibAntLocationZ real, calibCableDelay real);

CREATE TABLE ARA02(antCalId integer primary key, holeName text, antName text, pulserName text, antType text, polType text, antLocationX real, antLocationY real, antLocationZ real, cableDelay real, calibAntLocationX real, calibAntLocationY real, calibAntLocationZ real, calibCableDelay real);

CREATE TABLE ARA03(antCalId integer primary key, holeName text, antName text, pulserName text, antType text, polType text, antLocationX real, antLocationY real, antLocationZ real, cableDelay real, calibAntLocationX real, calibAntLocationY real, calibAntLocationZ real, calibCableDelay real);

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
--delay unkown (and not needed?)

insert into ARA02 VALUES(0,"BH5","CH1","P1","kQuadSlot","kHorizontal",
37.2065,-18.6219,-193.0014,0,
37.2065,-18.6219,-193.0014,0);
insert into ARA02 VALUES(1,"BH5","CV1","P1","kBicone","kVertical",
37.2181,-18.6121,-189.7165,0,
37.2181,-18.6121,-189.7165,0);
insert into ARA02 VALUES(2,"BH6","CH2","P2","kQuadSlot","kHorizontal",
17.1658,34.8388,-168.1035,0,
17.1658,34.8388,-168.1035,0);
insert into ARA02 VALUES(3,"BH6","CV2","P2","kBicone","kVertical",
17.1772,34.8485,-164.8183,0,
17.1772,34.8485,-164.8183,0);


--ARA03 Calpulers
--Unkown positions
--delay unkown (and not needed?)
insert into ARA03 VALUES(0,"BH5","CH1","P1","kQuadSlot","kHorizontal",
0,0,0,0,
0,0,0,0);
insert into ARA03 VALUES(1,"BH5","CV1","P1","kBicone","kVertical",
0,0,0,0,
0,0,0,0);
insert into ARA03 VALUES(2,"BH6","CH2","P2","kQuadSlot","kHorizontal",
0,0,0,0,
0,0,0,0);
insert into ARA03 VALUES(3,"BH6","CV2","P2","kBicone","kVertical",
0,0,0,0,
0,0,0,0);
