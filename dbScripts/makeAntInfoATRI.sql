CREATE TABLE ARA01(antId integer primary key, holeName text, antName text, polType text, antPolNum integer, daqChanNum integer, daqTrigChan integer, foamId integer, foamChanNum integer, antType text, antLocationX real, antLocationY real, antLocationZ real, cableDelay real, calibAntLocationX real, calibAntLocationY real, calibAntLocationZ real, calibCableDelay real, antOrientX real, antOrientY real, antOrientZ real, highPassFilterMhz real, lowPassFilterMhz real, avgNoiseFigure real);


insert into ARA01 VALUES(0 , "BH1", "TVPol", "kVertical", 0, 1, 1, 0, 0, "kBicone",-4.03552152,7.5351132,-63.7909824,13.714,0,0,0,0,0,0,0,140,800,0);
insert into ARA01 VALUES(1 , "BH2", "TVPol", "kVertical", 1, 9, 5, 0, 0, "kBicone",8.55701616,1.20463056,-55.7022,13.714,0,0,0,0,0,0,0,140,800,0);
insert into ARA01 VALUES(2 , "BH3", "TVPol", "kVertical", 2, 17, 9, 0, 0, "kBicone",-9.74665056,-5.33259792,-54.7856664,13.714,0,0,0,0,0,0,0,140,800,0);
insert into ARA01 VALUES(3 , "BH4", "TVPol", "kVertical", 3, 25, 13, 0, 0, "kBicone", 2.210659536,-10.78126368,-61.7863128,13.714,0,0,0,0,0,0,0,140,800,0);


insert into ARA01 VALUES(4 , "BH1", "BVPol", "kVertical", 4, 0, 0, 0, 0, "kBicone",-4.035552,7.53505224,-82.663284,85.8187,0,0,0,0,0,0,0,140,800,0);
insert into ARA01 VALUES(5 , "BH2", "BVPol", "kVertical", 5, 8, 4, 0, 0, "kBicone",8.5569552,1.204578744,-73.5835968,81.94,0,0,0,0,0,0,0,1408,00,0);
insert into ARA01 VALUES(6 , "BH3", "BVPol", "kVertical", 6, 16, 8, 0, 0, "kBicone",-9.74671152,-5.33265888,-73.6576632,85.8187,0,0,0,0,0,0,0,140,800,0);
insert into ARA01 VALUES(7 , "BH4", "BVPol", "kVertical", 7, 24, 12, 0, 0, "kBicone",2.210616864,-10.78129416,-79.3632144,80.7578,0,0,0,0,0,0,0,140,800,0);

insert into ARA01 VALUES(8, "BH1", "BHPol", "kHorizontal", 0, 2, 2, 0, 0, "kQuadSlot",-4.035552,7.53505224,-79.2595824,72.5466,0,0,0,0,0,0,0,140,800,0);
insert into ARA01 VALUES(9, "BH2", "BHPol", "kHorizontal", 1, 10, 6, 0, 0, "kQuadSlot",8.55698568,1.204587888,-70.1292984,68.668,0,0,0,0,0,0,0,140,800,0);
insert into ARA01 VALUES(10, "BH3", "BHPol", "kHorizontal", 2, 18, 10, 0, 0, "kQuadSlot",-9.74671152,-5.3326284,-70.2033648,72.5466,0,0,0,0,0,0,0,140,800,0);
insert into ARA01 VALUES(11, "BH4", "BHPol", "kHorizontal", 3, 26, 14, 0, 0, "kQuadSlot",2.210626008,-10.78129416,-76.213716,68.668,0,0,0,0,0,0,0,140,800,0);



insert into ARA01 VALUES(12, "BH1", "THPol", "kHorizontal", 4, 3, 3, 0, 0, "kQuadSlot",-4.03552152,7.5351132,-61.7335824,2.19432,0,0,0,0,0,0,0,140,800,0);
insert into ARA01 VALUES(13 , "BH2", "THPol", "kHorizontal", 5, 11, 7, 0, 0, "kQuadSlot",8.55701616,1.204636656,-53.6448,2.19432,0,0,0,0,0,0,0,140,800,0);
insert into ARA01 VALUES(14 , "BH3", "THPol", "kHorizontal", 6, 19, 11, 0, 0, "kQuadSlot",-9.74665056,-5.33259792,-52.7282664,2.19432,0,0,0,0,0,0,0,140,800,0);
insert into ARA01 VALUES(15 , "BH4", "THPol", "kHorizontal", 7, 27, 15, 0, 0, "kQuadSlot",2.210665632,-10.7812332,-59.7289128,2.19432,0,0,0,0,0,0,0,140,800,0);

insert into ARA01 VALUES(16, "S1", "SA1", "kSurface", 0, 4, 16, 0, 0, "kFatDipole",57.8711568,25.9488432,1.156344144, 0,0,0,0,0,0,0,0,140,800,0);
insert into ARA01 VALUES(17, "S2", "SA2", "kSurface", 1, 5, 17, 0, 0, "kFatDipole",-12.316968,-59.128152,0.80206596,0,0,0,0,0,0,0,0,140,800,0);
insert into ARA01 VALUES(18, "S3", "SA3", "kSurface", 2, 28, 18, 0, 0, "kFatDipole",-46.0638144,37.100256,0.649775688,0,0,0,0,0,0,0,0,140,800,0);
insert into ARA01 VALUES(19, "S4", "SA4", "kSurface", 3, 29, 19, 0, 0, "kFatDipole",-7.65517392,3.30881736,0.943410864,0,0,0,0,0,0,0,0,140,800,0);








