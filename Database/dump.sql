-- MySQL dump 10.13  Distrib 5.7.17, for Linux (x86_64)
--
-- Host: localhost    Database: face_2_3_0
-- ------------------------------------------------------
-- Server version	5.7.17-0ubuntu0.16.04.1



--
-- Table structure for table `face`
--

DROP TABLE IF EXISTS `face`;
CREATE TABLE `face` (
  `userId` varchar(255) NOT NULL COMMENT '由平台传过来的表示某个user创建的关键字段(长度不能超过255)',
  `faceId` varchar(255) NOT NULL COMMENT '系统唯一标示face(可用于查询图片存储对应人脸)',
  `imgId` varchar(255) DEFAULT NULL COMMENT 'face对应的image的id(可用于从图片数据库查询数据)',
  `faceFeature` mediumblob COMMENT '人脸提取的feature',
  `addedFaceSet` text COMMENT 'face已经加入的faceset',
  `personId` varchar(255) DEFAULT NULL COMMENT 'face对应的personId',
  `tag` varchar(255) DEFAULT NULL COMMENT '用户自行添加的表示，方便用户查看标示',
  `param` text COMMENT '冗余处理为拓展保存临时数据(计划使用json保存部分数据)',
  `createTime` timestamp NULL DEFAULT CURRENT_TIMESTAMP COMMENT 'face创建时间(时间需要由创建者给出)',
  `updateTime` timestamp NULL DEFAULT NULL COMMENT '更新时间，由系统自动更新',
  `isDelete` int(11) DEFAULT '0' COMMENT '默认值为0表示没有被删除，1表示已经逻辑删除数据',
  `landmark` text,
  `transform_matrix` text,
  `lastFreeTime` timestamp NULL DEFAULT CURRENT_TIMESTAMP,
  `version` varchar(128) DEFAULT '0.0.3.C.20171217',
  PRIMARY KEY (`faceId`(50)),
  KEY `index_personId` (`personId`,`userId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `faceset`
--

DROP TABLE IF EXISTS `faceset`;
CREATE TABLE `faceset` (
  `userId` varchar(255) NOT NULL COMMENT '应用创建者的标示',
  `facesetId` varchar(255) NOT NULL COMMENT 'faceSet的唯一id',
  `facesetName` varchar(255) NOT NULL DEFAULT '' COMMENT 'faceset的name，必须在某个app中唯一',
  `faceidSet` mediumtext,
  `facefeatureSet` mediumblob COMMENT '保存faceid对应的所有feature，方便进行feature的读取',
  `tag` varchar(255) DEFAULT NULL COMMENT '用户自定义的tag(可以为空)',
  `user_param` text COMMENT '用户自定定义的参数(例如用户可以保存person对应所有的faceId等信息)',
  `createTime` timestamp NULL DEFAULT CURRENT_TIMESTAMP COMMENT '创建的时间',
  `updateTime` timestamp NULL DEFAULT NULL COMMENT '更新的时间',
  `isDelete` int(11) DEFAULT '0' COMMENT '逻辑删除的标示,0表示逻辑存在数据库，1表示逻辑删除数据',
  PRIMARY KEY (`facesetId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


--
-- Table structure for table `facesettime`
--

DROP TABLE IF EXISTS `facesettime`;
CREATE TABLE `facesettime` (
  `facesetId` varchar(255) NOT NULL,
  `updateTime` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`facesetId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


--
-- Table structure for table `url`
--

DROP TABLE IF EXISTS `url`;
CREATE TABLE `url` (
  `userId` varchar(255) DEFAULT NULL,
  `id` varchar(255) NOT NULL COMMENT 'maybe faceid or imgid',
  `imgurl` text,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Table structure for table `userid`
--

DROP TABLE IF EXISTS `userid`;
CREATE TABLE `userid` (
  `userId` varchar(255) NOT NULL COMMENT '系统用户唯一标示',
  `createTime` timestamp NULL DEFAULT CURRENT_TIMESTAMP COMMENT '用户创建的时间',
  `updateTime` timestamp NULL DEFAULT NULL COMMENT '更新的时间',
  `isDelete` int(11) DEFAULT '0' COMMENT '0表示逻辑存在,1表示逻辑删除'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


-- Dump completed on 2018-02-08 10:49:29
