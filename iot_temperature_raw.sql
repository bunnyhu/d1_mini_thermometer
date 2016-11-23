
CREATE TABLE IF NOT EXISTS `iot_temperature_raw` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `time` timestamp NULL DEFAULT CURRENT_TIMESTAMP,
  `millis` bigint(20) DEFAULT NULL,
  `station_id` varchar(32) CHARACTER SET utf8 DEFAULT NULL,
  `humidity` int(11) DEFAULT NULL,
  `temperature` int(11) DEFAULT NULL,
  `heat_index` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `time` (`time`),
  KEY `station_id` (`station_id`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci AUTO_INCREMENT=1;
