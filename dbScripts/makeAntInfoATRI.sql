CREATE TABLE ARA01(antId integer primary key, holeName text, antName text, polType text, antPolNum integer, daqChanNum integer, daqTrigChan integer, foamId integer, foamChanNum integer, antType text, antLocationX real, antLocationY real, antLocationZ real, cableDelay real, calibAntLocationX real, calibAntLocationY real, calibAntLocationZ real, calibCableDelay real, antOrientX real, antOrientY real, antOrientZ real, highPassFilterMhz real, lowPassFilterMhz real, avgNoiseFigure real);

CREATE TABLE ARA01_2018(antId integer primary key, holeName text, antName text, polType text, antPolNum integer, daqChanNum integer, daqTrigChan integer, foamId integer, foamChanNum integer, antType text, antLocationX real, antLocationY real, antLocationZ real, cableDelay real, calibAntLocationX real, calibAntLocationY real, calibAntLocationZ real, calibCableDelay real, antOrientX real, antOrientY real, antOrientZ real, highPassFilterMhz real, lowPassFilterMhz real, avgNoiseFigure real);

CREATE TABLE ARA02(antId integer primary key, holeName text, antName text, polType text, antPolNum integer, daqChanNum integer, daqTrigChan integer, foamId integer, foamChanNum integer, antType text, antLocationX real, antLocationY real, antLocationZ real, cableDelay real, calibAntLocationX real, calibAntLocationY real, calibAntLocationZ real, calibCableDelay real, antOrientX real, antOrientY real, antOrientZ real, highPassFilterMhz real, lowPassFilterMhz real, avgNoiseFigure real);

CREATE TABLE ARA03(antId integer primary key, holeName text, antName text, polType text, antPolNum integer, daqChanNum integer, daqTrigChan integer, foamId integer, foamChanNum integer, antType text, antLocationX real, antLocationY real, antLocationZ real, cableDelay real, calibAntLocationX real, calibAntLocationY real, calibAntLocationZ real, calibCableDelay real, antOrientX real, antOrientY real, antOrientZ real, highPassFilterMhz real, lowPassFilterMhz real, avgNoiseFigure real);

CREATE TABLE ARA03_2018(antId integer primary key, holeName text, antName text, polType text, antPolNum integer, daqChanNum integer, daqTrigChan integer, foamId integer, foamChanNum integer, antType text, antLocationX real, antLocationY real, antLocationZ real, cableDelay real, calibAntLocationX real, calibAntLocationY real, calibAntLocationZ real, calibCableDelay real, antOrientX real, antOrientY real, antOrientZ real, highPassFilterMhz real, lowPassFilterMhz real, avgNoiseFigure real);

CREATE TABLE ARA04(antId integer primary key, holeName text, antName text, polType text, antPolNum integer, daqChanNum integer, daqTrigChan integer, foamId integer, foamChanNum integer, antType text, antLocationX real, antLocationY real, antLocationZ real, cableDelay real, calibAntLocationX real, calibAntLocationY real, calibAntLocationZ real, calibCableDelay real, antOrientX real, antOrientY real, antOrientZ real, highPassFilterMhz real, lowPassFilterMhz real, avgNoiseFigure real);

CREATE TABLE ARA05(antId integer primary key, holeName text, antName text, polType text, antPolNum integer, daqChanNum integer, daqTrigChan integer, foamId integer, foamChanNum integer, antType text, antLocationX real, antLocationY real, antLocationZ real, cableDelay real, calibAntLocationX real, calibAntLocationY real, calibAntLocationZ real, calibCableDelay real, antOrientX real, antOrientY real, antOrientZ real, highPassFilterMhz real, lowPassFilterMhz real, avgNoiseFigure real);

CREATE TABLE ARA06(antId integer primary key, holeName text, antName text, polType text, antPolNum integer, daqChanNum integer, daqTrigChan integer, foamId integer, foamChanNum integer, antType text, antLocationX real, antLocationY real, antLocationZ real, cableDelay real, calibAntLocationX real, calibAntLocationY real, calibAntLocationZ real, calibCableDelay real, antOrientX real, antOrientY real, antOrientZ real, highPassFilterMhz real, lowPassFilterMhz real, avgNoiseFigure real);


--chanNum,hole,antName,polType 
--polNum,daqChan,daqtrig,foam,foamchan
--xPos,yPos,zPos,tDelay
--xPos,yPos,zPos,tDelay --calibrated
--orientX,orientY,orientZ,highPass,lowPass,noiseFig


-- not clear which Hole corresponds to which FOAM. FOAM+T/B+H/V+daqChan OK
-- which hole which trigChan?

-- Pre 2018 table is exactly the same as post 2018 table in A1 -- Mohammad

-- VPol Calibrated
insert into ARA01 VALUES(0 , "BH1", "TVPol", "kVertical",
0, 0, 1, 1, 4, "kBicone",
-4.723530629289786, 6.662151862410533, -59.294452621852656, 18.75834846874593,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA01 VALUES(1 , "BH2", "TVPol", "kVertical",
1, 9, 5, 2, 4, "kBicone",
7.546106860532129, 0.462745992675436, -61.19677534370688, 18.707291803991595,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA01 VALUES(2 , "BH3", "TVPol", "kVertical",
2, 16, 9, 3, 4, "kBicone",
-8.729738743571378, -4.902328765426568, -57.15756240053563, 8.686012870849746,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA01 VALUES(3 , "BH4", "TVPol", "kVertical",
3, 24, 13, 3, 4, "kBicone",
2.8710039221454253, -9.92759713198006, -61.65897045960078, 8.697370722349651,
0,0,0,0,
0,0,0,140,800,0);

insert into ARA01 VALUES(4 , "BH1", "BVPol", "kVertical",
4, 1, 0, 1, 2, "kBicone",
-4.638275330528526, 6.311655328770542, -81.30350109416544, 80.82911486393992,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA01 VALUES(5 , "BH2", "BVPol", "kVertical",
5, 8, 4, 2, 2, "kBicone",
7.578874172345894, 0.40277267926680715, -81.07877116620143, 76.94696871816556,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA01 VALUES(6 , "BH3", "BVPol", "kVertical",
6, 17, 8, 3, 2, "kBicone",
-8.772965055222278, -4.768747316323455, -71.34443275152898, 90.79201315221765,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA01 VALUES(7 , "BH4", "BVPol", "kVertical",
7, 25, 12, 4, 2, "kBicone",
2.6891978366651625, -9.574807533351018, -77.51516422536584, 75.73873371477006,
0,0,0,0,
0,0,0,140,800,0);

-- HPol Calibrated
insert into ARA01 VALUES(8, "BH1", "THPol", "kHorizontal",
0, 2, 2, 1, 1, "kQuadSlot",
-4.723530629289786, 6.662151862410533, -57.735953608626225, 7.262116839535381,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA01 VALUES(9, "BH2", "THPol", "kHorizontal",
1, 10, 6, 2, 1, "kQuadSlot",
7.546106860532129, 0.462745992675436, -59.463375669638694, 5.485582140155802,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA01 VALUES(10, "BH3", "THPol", "kHorizontal",
2, 18, 10, 3, 1, "kQuadSlot",
-8.729738743571378, -4.902328765426568, -54.600162421543864, 1.6943201067890215,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA01 VALUES(11, "BH4", "THPol", "kHorizontal",
3, 26, 14, 4, 1, "kQuadSlot",
2.8710039221454253, -9.92759713198006, -59.10157046395337, 1.694320224190091,
0,0,0,0,
0,0,0,140,800,0);

insert into ARA01 VALUES(12, "BH1", "BHPol", "kHorizontal",
4, 3, 3, 1, 3, "kQuadSlot",
-4.638275330528526, 6.311655328770542, -78.21898958232835, 79.60388008528658,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA01 VALUES(13 , "BH2", "BHPol", "kHorizontal",
5, 11, 7, 2, 3, "kQuadSlot",
7.578874172345894, 0.40277267926680715, -77.12437160901453, 68.16800143805973,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA01 VALUES(14 , "BH3", "BHPol", "kHorizontal",
6, 19, 11, 3, 3, "kQuadSlot",
-8.772965055222278, -4.768747316323455, -68.10404650104135 , 79.77191854390931,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA01 VALUES(15 , "BH4", "BHPol", "kHorizontal",
7, 27, 15, 4, 3, "kQuadSlot",
2.6891978366651625, -9.574807533351018, -74.8655642245048, 78.66799999811364,
0,0,0,0,
0,0,0,140,800,0);

--Surface Positions incorrect

insert into ARA01 VALUES(16, "S1", "SA1", "kSurface", 
0, 4, 16, 0, 0, "kFatDipole",
57.8711568,25.9488432,1.156344144,0,
0,0,0,0,
0,0,0,25,116,0);
insert into ARA01 VALUES(17, "S2", "SA2", "kSurface", 
1, 5, 17, 0, 0, "kFatDipole",
-12.316968,-59.128152,0.80206596,0,
0,0,0,0,
0,0,0,25,116,0);
insert into ARA01 VALUES(18, "S3", "SA3", "kSurface", 
2, 28, 18, 0, 0, "kFatDipole",
-46.0638144,37.100256,0.649775688,0,
0,0,0,0,
0,0,0,25,116,0);
insert into ARA01 VALUES(19, "S4", "SA4", "kSurface", 
3, 29, 19, 0, 0, "kFatDipole",
-7.65517392,3.30881736,0.943410864,0,
0,0,0,0,
0,0,0,25,116,0);

------------------------New ARA01 2018 table------------------------
-- Pre 2018 table is exactly the same as post 2018 table in A1 -- Mohammad
-- VPol Calibrated
insert into ARA01_2018 VALUES(0 , "BH1", "TVPol", "kVertical",
0, 0, 1, 1, 4, "kBicone",
-4.723530629289786, 6.662151862410533, -59.294452621852656, 18.75834846874593,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA01_2018 VALUES(1 , "BH2", "TVPol", "kVertical",
1, 9, 5, 2, 4, "kBicone",
7.546106860532129, 0.462745992675436, -61.19677534370688, 18.707291803991595,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA01_2018 VALUES(2 , "BH3", "TVPol", "kVertical",
2, 16, 9, 3, 4, "kBicone",
-8.729738743571378, -4.902328765426568, -57.15756240053563, 8.686012870849746,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA01_2018 VALUES(3 , "BH4", "TVPol", "kVertical",
3, 24, 13, 3, 4, "kBicone",
2.8710039221454253, -9.92759713198006, -61.65897045960078, 8.697370722349651,
0,0,0,0,
0,0,0,140,800,0);

insert into ARA01_2018 VALUES(4 , "BH1", "BVPol", "kVertical",
4, 1, 0, 1, 2, "kBicone",
-4.638275330528526, 6.311655328770542, -81.30350109416544, 80.82911486393992,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA01_2018 VALUES(5 , "BH2", "BVPol", "kVertical",
5, 8, 4, 2, 2, "kBicone",
7.578874172345894, 0.40277267926680715, -81.07877116620143, 76.94696871816556,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA01_2018 VALUES(6 , "BH3", "BVPol", "kVertical",
6, 17, 8, 3, 2, "kBicone",
-8.772965055222278, -4.768747316323455, -71.34443275152898, 90.79201315221765,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA01_2018 VALUES(7 , "BH4", "BVPol", "kVertical",
7, 25, 12, 4, 2, "kBicone",
2.6891978366651625, -9.574807533351018, -77.51516422536584, 75.73873371477006,
0,0,0,0,
0,0,0,140,800,0);

-- HPol Calibrated
insert into ARA01_2018 VALUES(8, "BH1", "THPol", "kHorizontal",
0, 2, 2, 1, 1, "kQuadSlot",
-4.723530629289786, 6.662151862410533, -57.735953608626225, 7.262116839535381,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA01_2018 VALUES(9, "BH2", "THPol", "kHorizontal",
1, 10, 6, 2, 1, "kQuadSlot",
7.546106860532129, 0.462745992675436, -59.463375669638694, 5.485582140155802,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA01_2018 VALUES(10, "BH3", "THPol", "kHorizontal",
2, 18, 10, 3, 1, "kQuadSlot",
-8.729738743571378, -4.902328765426568, -54.600162421543864, 1.6943201067890215,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA01_2018 VALUES(11, "BH4", "THPol", "kHorizontal",
3, 26, 14, 4, 1, "kQuadSlot",
2.8710039221454253, -9.92759713198006, -59.10157046395337, 1.694320224190091,
0,0,0,0,
0,0,0,140,800,0);

insert into ARA01_2018 VALUES(12, "BH1", "BHPol", "kHorizontal",
4, 3, 3, 1, 3, "kQuadSlot",
-4.638275330528526, 6.311655328770542, -78.21898958232835, 79.60388008528658,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA01_2018 VALUES(13 , "BH2", "BHPol", "kHorizontal",
5, 11, 7, 2, 3, "kQuadSlot",
7.578874172345894, 0.40277267926680715, -77.12437160901453, 68.16800143805973,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA01_2018 VALUES(14 , "BH3", "BHPol", "kHorizontal",
6, 19, 11, 3, 3, "kQuadSlot",
-8.772965055222278, -4.768747316323455, -68.10404650104135 , 79.77191854390931,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA01_2018 VALUES(15 , "BH4", "BHPol", "kHorizontal",
7, 27, 15, 4, 3, "kQuadSlot",
2.6891978366651625, -9.574807533351018, -74.8655642245048, 78.66799999811364,
0,0,0,0,
0,0,0,140,800,0);

--Surface Positions incorrect

insert into ARA01_2018 VALUES(16, "S1", "SA1", "kSurface", 
0, 4, 16, 0, 0, "kFatDipole",
57.8711568,25.9488432,1.156344144,0,
0,0,0,0,
0,0,0,25,116,0);
insert into ARA01_2018 VALUES(17, "S2", "SA2", "kSurface", 
1, 5, 17, 0, 0, "kFatDipole",
-12.316968,-59.128152,0.80206596,0,
0,0,0,0,
0,0,0,25,116,0);
insert into ARA01_2018 VALUES(18, "S3", "SA3", "kSurface", 
2, 28, 18, 0, 0, "kFatDipole",
-46.0638144,37.100256,0.649775688,0,
0,0,0,0,
0,0,0,25,116,0);
insert into ARA01_2018 VALUES(19, "S4", "SA4", "kSurface", 
3, 29, 19, 0, 0, "kFatDipole",
-7.65517392,3.30881736,0.943410864,0,
0,0,0,0,
0,0,0,25,116,0);



-- ARA2 Information -- This is not correct yet!
-- FOAM+daqChan done+position+estimated delay

-- Chan 0-15 X, Y, Z, and delay values adjusted by BAC Jan 11 2019

insert into ARA02 VALUES(0 , "BH1", "TVPol", "kVertical", 
0, 1, 1, 1, 2, "kBicone",
10.5874,2.3432,-170.247,19.44,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(1 , "BH2", "TVPol", "kVertical", 
1, 9, 5, 2, 2, "kBicone",
4.85167,-10.3981,-170.347,19.8606,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(2 , "BH3", "TVPol", "kVertical", 
2, 17, 9, 3, 2, "kBicone",
-2.58128,9.37815,-171.589,125.547,  
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(3 , "BH4", "TVPol", "kVertical", 
3, 25, 13, 4, 2, "kBicone",
-7.84111,-4.05791,-175.377,22.5759,  
0,0,0,0,  
0,0,0,140,800,0);

--updated 15-07-13

insert into ARA02 VALUES(4 , "BH1", "BVPol", "kVertical", 
4, 0, 0, 1, 4, "kBicone",
10.5873,2.3428,-189.502,93.41,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(5 , "BH2", "BVPol", "kVertical", 
5, 8, 4, 2, 4, "kBicone",
4.85157,-10.3985,-189.4,93.8306,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(6 , "BH3", "BVPol", "kVertical", 
6, 16, 8, 3, 4, "kBicone",
-2.58138,9.37775,-191.242,206.317,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(7 , "BH4", "BVPol", "kVertical", 
7, 24, 12, 4, 4, "kBicone",
-7.84131,-4.05821,-194.266,96.5459,
0,0,0,0,  
0,0,0,140,800,0);

--updated 15-07-13

insert into ARA02 VALUES(8, "BH1", "THPol", "kHorizontal", 
0, 3, 3, 1, 1, "kQuadSlot",
10.5874,2.3128,-167.492,7.24,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(9 , "BH2", "THPol", "kHorizontal", 
1, 11, 7, 2, 1, "kQuadSlot",
4.85167,-10.3981,-167.428,7.66056,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(10 , "BH3", "THPol", "kHorizontal", 
2, 19, 11, 3, 1, "kQuadSlot",
-2.58128,9.37825,-168.468,113.347,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(11 , "BH4", "THPol", "kHorizontal", 
3, 27, 15, 4, 1, "kQuadSlot",
-7.84111,-4.05781,-172.42,10.3759,
0,0,0,0,  
0,0,0,140,800,0);

--updated 15-07-13

insert into ARA02 VALUES(12, "BH1", "BHPol", "kHorizontal", 
4, 2, 2, 1, 3, "kQuadSlot",
10.5873,2.3429,-186.546,81.2,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(13, "BH2", "BHPol", "kHorizontal", 
5, 10, 6, 2, 3, "kQuadSlot",
4.85157,-10.3985,-186.115,81.6206,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(14, "BH3", "BHPol", "kHorizontal", 
6, 18, 10, 3, 3, "kQuadSlot",
-2.58138,9.37775,-187.522,187.307,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA02 VALUES(15, "BH4", "BHPol", "kHorizontal", 
7, 26, 14, 4, 3, "kQuadSlot",
-7.84111,-4.05821,-190.981,84.3359,
0,0,0,0,  
0,0,0,140,800,0);

--updated 15-07-13


--Positions incorrect
insert into ARA02 VALUES(16, "S1", "SA1", "kSurface", 
0, 4, 16, 0, 0, "kFatDipole"
,1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);
insert into ARA02 VALUES(17, "S2", "SA2", "kSurface", 
1, 5, 17, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);
insert into ARA02 VALUES(18, "S3", "SA3", "kSurface", 
2, 28, 18, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);
insert into ARA02 VALUES(19, "S4", "SA4", "kSurface", 
3, 29, 19, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);



-- ARA03 information
--Position Done
--daqChan Mapping done
--chanNum done
--estimated cable delay

-- Chan 0-15 X, Y, Z, and delay values adjusted by BAC Jan 11 2019

insert into ARA03 VALUES(0 , "BH1", "TVPol", "kVertical", 
0, 0, 1, 0, 0, "kBicone",
4.82341,-9.41804,-172.607,19.1537,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(1 , "BH2", "TVPol", "kVertical", 
1, 8, 5, 0, 0, "kBicone",
10.6429,3.40846,-176.209,126.249,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(2 , "BH3", "TVPol", "kVertical", 
2, 16, 9, 0, 0, "kBicone",
-2.0103,9.4124,-174.105,19.44,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(3 , "BH4", "TVPol", "kVertical", 
3, 24, 13, 0, 0, "kBicone",
-8.14654,-3.933,-172.214,21.5614,
0,0,0,0,  
0,0,0,140,800,0);

--updated 15-07-13

insert into ARA03 VALUES(4 , "BH1", "BVPol", "kVertical", 
4, 1, 0, 0, 0, "kBicone",
4.82321,-9.41834,-191.66,93.1237,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(5 , "BH2", "BVPol", "kVertical", 
5, 9, 4, 0, 0, "kBicone",
10.6426,3.40816,-194.933,200.219,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(6 , "BH3", "BVPol", "kVertical", 
6, 17, 8, 0, 0, "kBicone",
-2.0102,9.412,-192.666,93.41,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(7 , "BH4", "BVPol", "kVertical", 
7, 25, 12, 0, 0, "kBicone",
-8.14674,-3.9333,-191.267,95.5314,
0,0,0,0,  
0,0,0,140,800,0);

--updated 15-07-13

insert into ARA03 VALUES(8, "BH1", "THPol", "kHorizontal", 
0, 2, 3, 0, 0, "kQuadSlot",
4.82351,-9.41794,-169.486,6.95369,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(9 , "BH2", "THPol", "kHorizontal", 
1, 10, 7, 0, 0, "kQuadSlot",
10.6429,3.40856,-173.252,114.049,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(10 , "BH3", "THPol", "kHorizontal", 
2, 18, 11, 0, 0, "kQuadSlot",
-2.01,9.4124,-170.617,7.24,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(11, "BH4", "THPol", "kHorizontal", 
3, 26, 15, 0, 0, "kQuadSlot",
-8.14644,-3.9329,-169.257,9.36141,
0,0,0,0,  
0,0,0,140,800,0);

--updated 15-07-13

insert into ARA03 VALUES(12, "BH1", "BHPol", "kHorizontal", 
4, 3, 2, 0, 0, "kQuadSlot",
4.82321,-9.41824,-188.408,80.9137,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(13, "BH2", "BHPol", "kHorizontal", 
5, 11, 6, 0, 0, "kQuadSlot",
10.6427,3.40826,-191.977,188.009,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03 VALUES(14, "BH3", "BHPol", "kHorizontal", 
6, 19, 10, 0, 0, "kQuadSlot",
-2.0102,9.4121,-189.709,81.2,
0,0,0,0, 
0,0,0,140,800,0);
insert into ARA03 VALUES(15, "BH4", "BHPol", "kHorizontal", 
7, 27, 14, 0, 0, "kQuadSlot",
-8.14674,-3.9332,-188.146,83.3214,
0,0,0,0,  
0,0,0,140,800,0);


--updated 15-07-13

--Positions incorrect

insert into ARA03 VALUES(16, "S1", "SA1", "kSurface", 
0, 4, 16, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);
insert into ARA03 VALUES(17, "S2", "SA2", "kSurface", 
1, 5, 17, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);
insert into ARA03 VALUES(18, "S3", "SA3", "kSurface", 
2, 28, 18, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);
insert into ARA03 VALUES(19, "S4", "SA4", "kSurface", 
3, 29, 19, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);

--Update by UAL (01/25/2018). Here I have made the ARA03 2018 table where the channel mappings for channel 2 and channel 3 have been corrected. The correction has been by essentially switching the daqChanNum variable values between them. ARA02 did not have problems for channel mapping.
--Update by MSM (04/15/2025). Switched electric channels for RF to fix flipped channel mapping between top & bottom HPols on string 3 (RF chans 10 & 14). See DocDB https://aradocs.wipac.wisc.edu/cgi-bin/DocDB/ShowDocument?docid=3332
----------------------------------------------------------------------------------------


insert into ARA03_2018 VALUES(0 , "BH1", "TVPol", "kVertical", 
0, 0, 1, 0, 0, "kBicone",
4.82341,-9.41804,-172.607,19.1537,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03_2018 VALUES(1 , "BH2", "TVPol", "kVertical", 
1, 8, 5, 0, 0, "kBicone",
10.6429,3.40846,-176.209,126.249,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03_2018 VALUES(2 , "BH3", "TVPol", "kVertical", 
2, 17, 9, 0, 0, "kBicone",
-2.0103,9.4124,-174.105,19.44,
0,0,0,0,  
0,0,0,140,800,0);

insert into ARA03_2018 VALUES(3 , "BH4", "TVPol", "kVertical", 
3, 24, 13, 0, 0, "kBicone",
-8.14654,-3.933,-172.214,21.5614,
0,0,0,0,  
0,0,0,140,800,0);

--updated 15-07-13

insert into ARA03_2018 VALUES(4 , "BH1", "BVPol", "kVertical", 
4, 1, 0, 0, 0, "kBicone",
4.82321,-9.41834,-191.66,93.1237,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03_2018 VALUES(5 , "BH2", "BVPol", "kVertical", 
5, 9, 4, 0, 0, "kBicone",
10.6426,3.40816,-194.933,200.219,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03_2018 VALUES(6 , "BH3", "BVPol", "kVertical", 
6, 16, 8, 0, 0, "kBicone",
-2.0102,9.412,-192.666,93.41,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03_2018 VALUES(7 , "BH4", "BVPol", "kVertical", 
7, 25, 12, 0, 0, "kBicone",
-8.14674,-3.9333,-191.267,95.5314,
0,0,0,0,  
0,0,0,140,800,0);

--updated 15-07-13

insert into ARA03_2018 VALUES(8, "BH1", "THPol", "kHorizontal", 
0, 2, 3, 0, 0, "kQuadSlot",
4.82351,-9.41794,-169.486,6.95369,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03_2018 VALUES(9 , "BH2", "THPol", "kHorizontal", 
1, 10, 7, 0, 0, "kQuadSlot",
10.6429,3.40856,-173.252,114.049,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03_2018 VALUES(10 , "BH3", "THPol", "kHorizontal", 
2, 19, 11, 0, 0, "kQuadSlot",
-2.01,9.4124,-170.617,7.24,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03_2018 VALUES(11, "BH4", "THPol", "kHorizontal", 
3, 26, 15, 0, 0, "kQuadSlot",
-8.14644,-3.9329,-169.257,9.36141,
0,0,0,0,  
0,0,0,140,800,0);

--updated 15-07-13

insert into ARA03_2018 VALUES(12, "BH1", "BHPol", "kHorizontal", 
4, 3, 2, 0, 0, "kQuadSlot",
4.82321,-9.41824,-188.408,80.9137,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03_2018 VALUES(13, "BH2", "BHPol", "kHorizontal", 
5, 11, 6, 0, 0, "kQuadSlot",
10.6427,3.40826,-191.977,188.009,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA03_2018 VALUES(14, "BH3", "BHPol", "kHorizontal", 
6, 18, 10, 0, 0, "kQuadSlot",
-2.0102,9.4121,-189.709,81.2,
0,0,0,0, 
0,0,0,140,800,0);
insert into ARA03_2018 VALUES(15, "BH4", "BHPol", "kHorizontal", 
7, 27, 14, 0, 0, "kQuadSlot",
-8.14674,-3.9332,-188.146,83.3214,
0,0,0,0,  
0,0,0,140,800,0);


--updated 15-07-13

--Positions incorrect

insert into ARA03_2018 VALUES(16, "S1", "SA1", "kSurface", 
0, 4, 16, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);
insert into ARA03_2018 VALUES(17, "S2", "SA2", "kSurface", 
1, 5, 17, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);
insert into ARA03_2018 VALUES(18, "S3", "SA3", "kSurface", 
2, 28, 18, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);
insert into ARA03_2018 VALUES(19, "S4", "SA4", "kSurface", 
3, 29, 19, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);

-- ARA04 information
-- Updated by UAL (01/25/2019). Updated the X, Y and Z coordinates of the station, the foamChanNum, the channel mappings and the cable delays. I used Yue Pan's AntennaInfoV3.sqlite to get the right channel mappings (daqChanNum) and cable delays (which I am not sure about). Used this file: https://www.overleaf.com/read/bpvxgfrhqhzh  to get the surveyor coordinates which I then converted Ara Station Centric Coordinate system.
-- Updated by BAC (04/24/2019). Updated the cable delays for all deep channels.
-- Updated by BAC (04/24/2019). Updated the cable delays for all deep channels.
-- Updated by Paramita DG (05/26/2024). Updated the antenna depth, X-Y , cable delay, channel map ( Araroot channel map was incorrect, so the channel map has been updated below). 
-- PDG found the fitted positions and cable delay in Surveyed coord. system, and Pawan Giri converted those numbers into station-centric coordinate. Below are the numbers that Pawan calculated using PDG's fitted coordinate. 
-- 05/26/2024 Note that :  "The relative cable delay for Hpol are same as default araroot from deployment time... These numbers yet to be  updated "

insert into ARA04 VALUES(0 , "BH3", "TVPol", "kVertical",
0, 9, 5, 0, 2, "kBicone",
10.87,-23.027,-164.558,42.8398,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA04 VALUES(1 , "BH2", "TVPol", "kVertical",
1, 1, 9, 0, 2, "kBicone",
13.23,3.91,-159.692,42.579,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA04 VALUES(2 , "BH1", "TVPol", "kVertical",
2, 17, 1, 0, 2, "kBicone",
-11.34,20.979,-162.7681,45.1048,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA04 VALUES(3 , "BH4", "TVPol", "kVertical",
3, 29, 13, 0, 2, "kBicone",
-13.183,-4.55271,-165.27,18.62,
0,0,0,0,
0,0,0,140,800,0);

insert into ARA04 VALUES(4 , "BH3", "BVPol", "kVertical",
4, 8, 4, 0, 4, "kBicone",
10.87,-23.027,-192.95,161.8289,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA04 VALUES(5 , "BH2", "BVPol", "kVertical",
5, 0, 8, 0, 4, "kBicone",
13.23,3.91,-188.756,158.766,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA04 VALUES(6 , "BH1", "BVPol", "kVertical",
6, 16, 0, 0, 4, "kBicone",
-11.34,20.979,-189.14056,156.692,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA04 VALUES(7 , "BH4", "BVPol", "kVertical",
7, 28, 12, 0, 4, "kBicone",
-13.183,-4.55271,-194.9,132.3,
0,0,0,0,
0,0,0,140,800,0);

insert into ARA04 VALUES(8 , "BH3", "THPol", "kHorizontal",
0, 11, 7, 0, 1, "kQuadSlot",
10.87,-23.027,-161.578,6.86,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA04 VALUES(9 , "BH2", "THPol", "kHorizontal",
1, 3, 11, 0, 1, "kQuadSlot",
13.23,3.91,-156.732,6.86,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA04 VALUES(10, "BH1", "THPol", "kHorizontal",
2, 19, 3, 0, 1, "kQuadSlot",
-11.34,20.979,-159.7581,6.86,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA04 VALUES(11, "BH4", "THPol", "kHorizontal",
3, 31, 15, 0, 1, "kQuadSlot",
-13.183,-4.55271,-162.35,6.86,
0,0,0,0,
0,0,0,140,800,0);

insert into ARA04 VALUES(12, "BH3", "BHPol", "kHorizontal",
4, 10, 6, 0, 3, "kQuadSlot",
10.87,-23.027,-189.9,120.54,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA04 VALUES(13, "BH2", "BHPol", "kHorizontal",
5, 2, 10, 0, 3, "kQuadSlot",
13.23,3.91,-185.686,120.54,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA04 VALUES(14, "BH1", "BHPol", "kHorizontal",
6, 18, 2, 0, 3, "kQuadSlot",
-11.34,20.979,-186.1906,120.54,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA04 VALUES(15, "BH4", "BHPol", "kHorizontal",
7, 30, 14, 0, 3, "kQuadSlot",
-13.183,-4.55271,-191.99,120.54,
0,0,0,0,
0,0,0,140,800,0);

insert into ARA04 VALUES(16, "S1", "SA1", "kSurface",
0, 4, 16, 0, 0, "kFatDipole",
1,2,3,4,
0,0,0,0,
0,0,0,25,116,0);

insert into ARA04 VALUES(17, "S2", "SA2", "kSurface",
1, 5, 17, 0, 0, "kFatDipole",
1,2,3,4,
0,0,0,0,
0,0,0,25,116,0);
insert into ARA04 VALUES(18, "S3", "SA3", "kSurface",
2, 28, 18, 0, 0, "kFatDipole",
1,2,3,4,
0,0,0,0,
0,0,0,25,116,0);
insert into ARA04 VALUES(19, "S4", "SA4", "kSurface",
3, 29, 19, 0, 0, "kFatDipole",
1,2,3,4,
0,0,0,0,
0,0,0,25,116,0);
 

-- ARA05 information
-- Updated by UAL (01/25/2019). Updated the X, Y and Z coordinates of the station, the foamChanNum, the channel mappings and the cable delays. I used Yue Pan's AntennaInfoV3.sqlite to get the right channel mappings (daqChanNum) and cable delays (which I am not sure about). Used this file: https://www.overleaf.com/read/bpvxgfrhqhzh  to get the surveyor coordinates which I then converted Ara Station Centric Coordinate system.
-- Updated by BAC (04/24/2019). Updated the cable delays for all deep channels.
-- Updated by MSM (01/30/2023). Reconciled RFChan mapping and station coordinates with existing db. 
-- Updated by MSM (03/23/2023). Fixed swapped locations/boreholes among some channels and updated their RFChan mappings to reflect this.
-- Updated by MSM (02/05/2024). Updated x, y coordinates so that the average A5 borehole position is the origin (previously the PA borehole was the origin).
-- Updated by MSM (02/15/2024). Rotated coordinate axes from easting/northing to local statio coordinate axes.
-- Updated by MSM (01/21/2025). Reverted swapped locations/boreholes and channel mappings.

insert into ARA05 VALUES(0 , "BH2", "TVPol", "kVertical", 
1, 1, 5, 0, 2, "kBicone",
-10.7667,-20.6606,-165.0948,23.571,
0,0,0,0,
0,0,0,140,800,0);
insert into ARA05 VALUES(1 , "BH1", "TVPol", "kVertical", 
0, 9, 1, 0, 2, "kBicone",
21.8338,-8.7099,-166.5366,20.61152,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA05 VALUES(2 , "BH4", "TVPol", "kVertical", 
3, 17, 13, 0, 2, "kBicone",
11.0500,17.4638,-147.2081,21.9055,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA05 VALUES(3 , "BH3", "TVPol", "kVertical", 
2, 25, 9, 0, 2, "kBicone",
-22.7158,11.1056,-161.02376,12.4532,
0,0,0,0,  
0,0,0,140,800,0);

insert into ARA05 VALUES(4 , "BH2", "BVPol", "kVertical", 
5, 0, 4, 0, 4, "kBicone",
-10.7667,-20.6606,-194.739,138.6438,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA05 VALUES(5 , "BH1", "BVPol", "kVertical", 
4, 8, 0, 0, 4, "kBicone",
21.8338,-8.7099,-196.2045,141.5849,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA05 VALUES(6 , "BH4", "BVPol", "kVertical", 
7, 16, 12, 0, 4, "kBicone",
11.0500,17.4638,-177.7466,140.1052,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA05 VALUES(7 , "BH3", "BVPol", "kVertical", 
6, 24, 8, 0, 4, "kBicone",
-22.7158,11.1056,-190.8559,130.5,
0,0,0,0,  
0,0,0,140,800,0);

insert into ARA05 VALUES(8, "BH2", "THPol", "kHorizontal", 
1, 3, 7, 0, 1, "kQuadSlot",
-10.7667,-20.6606,-161.75,6.86,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA05 VALUES(9 , "BH1", "THPol", "kHorizontal", 
0, 11, 3, 0, 1, "kQuadSlot",
21.8338,-8.7099,-162.53,6.86,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA05 VALUES(10, "BH4", "THPol", "kHorizontal", 
3, 19, 15, 0, 1, "kQuadSlot",
11.0500,17.4638,-145.01,6.86,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA05 VALUES(11 , "BH3", "THPol", "kHorizontal", 
2, 27, 11, 0, 1, "kQuadSlot",
-22.7158,11.1056,-158.39,6.86,
0,0,0,0,  
0,0,0,140,800,0);

insert into ARA05 VALUES(12, "BH2", "BHPol", "kHorizontal", 
5, 2, 6, 0, 3, "kQuadSlot",
-10.7667,-20.6606,-191.37,120.54,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA05 VALUES(13, "BH1", "BHPol", "kHorizontal", 
4, 10, 2, 0, 3, "kQuadSlot",
21.8338,-8.7099,-192.35,120.54,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA05 VALUES(14, "BH4", "BHPol", "kHorizontal", 
7, 18, 14, 0, 3, "kQuadSlot",
11.0500,17.4638,-174.74,120.54,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA05 VALUES(15, "BH3", "BHPol", "kHorizontal", 
6, 26, 10, 0, 3, "kQuadSlot",
-22.7158,11.1056,-188.12,120.54,
0,0,0,0, 
0,0,0,140,800,0);

insert into ARA05 VALUES(16, "S1", "SA1", "kSurface", 
0, 4, 16, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);
insert into ARA05 VALUES(17, "S2", "SA2", "kSurface", 
1, 5, 17, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);
insert into ARA05 VALUES(18, "S3", "SA3", "kSurface", 
2, 28, 18, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);
insert into ARA05 VALUES(19, "S4", "SA4", "kSurface", 
3, 29, 19, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);





-- ARA06 information
-- Placeholder infomation... for now this is just a copy of the ARA03 info

insert into ARA06 VALUES(0 , "BH1", "TVPol", "kVertical", 
0, 0, 1, 0, 0, "kBicone",
4.4113,-9.3933,-173.3937,15.44,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA06 VALUES(1 , "BH2", "TVPol", "kVertical", 
1, 8, 5, 0, 0, "kBicone",
10.6919,3.5089,-173.9734,115.44,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA06 VALUES(2 , "BH3", "TVPol", "kVertical", 
2, 16, 9, 0, 0, "kBicone",
-2.0103,9.4124,-174.1054,15.44,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA06 VALUES(3 , "BH4", "TVPol", "kVertical", 
3, 24, 13, 0, 0, "kBicone",
-8.0990,-3.7098,-173.5476,15.44,
0,0,0,0,  
0,0,0,140,800,0);

insert into ARA06 VALUES(4 , "BH1", "BVPol", "kVertical", 
4, 1, 0, 0, 0, "kBicone",
4.4111,-9.3936,-192.4470,81.41,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA06 VALUES(5 , "BH2", "BVPol", "kVertical", 
5, 9, 4, 0, 0, "kBicone",
10.6916,3.5086,-192.6981,181.41,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA06 VALUES(6 , "BH3", "BVPol", "kVertical", 
6, 17, 8, 0, 0, "kBicone",
-2.0102,9.4120,-192.6658,81.41,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA06 VALUES(7 , "BH4", "BVPol", "kVertical", 
7, 25, 12, 0, 0, "kBicone",
-8.0992,-3.7101,-192.6009,81.41,
0,0,0,0,  
0,0,0,140,800,0);

insert into ARA06 VALUES(8, "BH1", "THPol", "kHorizontal", 
0, 2, 3, 0, 0, "kQuadSlot",
4.4114,-9.3932,-170.2728,7.24,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA06 VALUES(9 , "BH2", "THPol", "kHorizontal", 
1, 10, 7, 0, 0, "kQuadSlot",
10.6919,3.5090,-171.0168,107.24,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA06 VALUES(10 , "BH3", "THPol", "kHorizontal", 
2, 18, 11, 0, 0, "kQuadSlot",
-2.0100,9.4124,-172.6271,7.24,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA06 VALUES(11, "BH4", "THPol", "kHorizontal", 
3, 26, 15, 0, 0, "kQuadSlot",
-8.0989,-3.7097,-170.5910,7.24,
0,0,0,0,  
0,0,0,140,800,0);


insert into ARA06 VALUES(12, "BH1", "BHPol", "kHorizontal", 
4, 3, 2, 0, 0, "kQuadSlot",
4.4111,-9.3935,-189.1947,73.2,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA06 VALUES(13, "BH2", "BHPol", "kHorizontal", 
5, 11, 6, 0, 0, "kQuadSlot",
10.6917,3.5087,-189.7415,173.2,
0,0,0,0,  
0,0,0,140,800,0);
insert into ARA06 VALUES(14, "BH3", "BHPol", "kHorizontal", 
6, 19, 10, 0, 0, "kQuadSlot",
-2.0102,9.4121,-189.7092,73.2,
0,0,0,0, 
0,0,0,140,800,0);
insert into ARA06 VALUES(15, "BH4", "BHPol", "kHorizontal", 
7, 27, 14, 0, 0, "kQuadSlot",
-8.0992,-3.7100,-189.4800,73.2,
0,0,0,0,  
0,0,0,140,800,0);

insert into ARA06 VALUES(16, "S1", "SA1", "kSurface", 
0, 4, 16, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);
insert into ARA06 VALUES(17, "S2", "SA2", "kSurface", 
1, 5, 17, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);
insert into ARA06 VALUES(18, "S3", "SA3", "kSurface", 
2, 28, 18, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);
insert into ARA06 VALUES(19, "S4", "SA4", "kSurface", 
3, 29, 19, 0, 0, "kFatDipole",
1,2,3,4,  
0,0,0,0,  
0,0,0,25,116,0);















