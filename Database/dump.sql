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
  `userId` varchar(255) NOT NULL COMMENT '��ƽ̨�������ı�ʾĳ��user�����Ĺؼ��ֶ�(���Ȳ��ܳ���255)',
  `faceId` varchar(255) NOT NULL COMMENT 'ϵͳΨһ��ʾface(�����ڲ�ѯͼƬ�洢��Ӧ����)',
  `imgId` varchar(255) DEFAULT NULL COMMENT 'face��Ӧ��image��id(�����ڴ�ͼƬ���ݿ��ѯ����)',
  `faceFeature` mediumblob COMMENT '������ȡ��feature',
  `addedFaceSet` text COMMENT 'face�Ѿ������faceset',
  `personId` varchar(255) DEFAULT NULL COMMENT 'face��Ӧ��personId',
  `tag` varchar(255) DEFAULT NULL COMMENT '�û�������ӵı�ʾ�������û��鿴��ʾ',
  `param` text COMMENT '���ദ��Ϊ��չ������ʱ����(�ƻ�ʹ��json���沿������)',
  `createTime` timestamp NULL DEFAULT CURRENT_TIMESTAMP COMMENT 'face����ʱ��(ʱ����Ҫ�ɴ����߸���)',
  `updateTime` timestamp NULL DEFAULT NULL COMMENT '����ʱ�䣬��ϵͳ�Զ�����',
  `isDelete` int(11) DEFAULT '0' COMMENT 'Ĭ��ֵΪ0��ʾû�б�ɾ����1��ʾ�Ѿ��߼�ɾ������',
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
  `userId` varchar(255) NOT NULL COMMENT 'Ӧ�ô����ߵı�ʾ',
  `facesetId` varchar(255) NOT NULL COMMENT 'faceSet��Ψһid',
  `facesetName` varchar(255) NOT NULL DEFAULT '' COMMENT 'faceset��name��������ĳ��app��Ψһ',
  `faceidSet` mediumtext,
  `facefeatureSet` mediumblob COMMENT '����faceid��Ӧ������feature���������feature�Ķ�ȡ',
  `tag` varchar(255) DEFAULT NULL COMMENT '�û��Զ����tag(����Ϊ��)',
  `user_param` text COMMENT '�û��Զ�����Ĳ���(�����û����Ա���person��Ӧ���е�faceId����Ϣ)',
  `createTime` timestamp NULL DEFAULT CURRENT_TIMESTAMP COMMENT '������ʱ��',
  `updateTime` timestamp NULL DEFAULT NULL COMMENT '���µ�ʱ��',
  `isDelete` int(11) DEFAULT '0' COMMENT '�߼�ɾ���ı�ʾ,0��ʾ�߼��������ݿ⣬1��ʾ�߼�ɾ������',
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
  `userId` varchar(255) NOT NULL COMMENT 'ϵͳ�û�Ψһ��ʾ',
  `createTime` timestamp NULL DEFAULT CURRENT_TIMESTAMP COMMENT '�û�������ʱ��',
  `updateTime` timestamp NULL DEFAULT NULL COMMENT '���µ�ʱ��',
  `isDelete` int(11) DEFAULT '0' COMMENT '0��ʾ�߼�����,1��ʾ�߼�ɾ��'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


-- Dump completed on 2018-02-08 10:49:29
