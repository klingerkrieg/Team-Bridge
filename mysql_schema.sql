-- MySQL Workbench Forward Engineering

SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL,ALLOW_INVALID_DATES';

-- -----------------------------------------------------
-- Schema mydb
-- -----------------------------------------------------
-- -----------------------------------------------------
-- Schema vrpn2
-- -----------------------------------------------------

-- -----------------------------------------------------
-- Schema vrpn2
-- -----------------------------------------------------
CREATE SCHEMA IF NOT EXISTS `vrpn2` DEFAULT CHARACTER SET utf8 ;
USE `vrpn2` ;

-- -----------------------------------------------------
-- Table `vrpn2`.`actions`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `vrpn2`.`actions` (
  `idaction` INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `action_name` VARCHAR(400) CHARACTER SET 'utf8' NOT NULL,
  PRIMARY KEY (`idaction`))
ENGINE = InnoDB
AUTO_INCREMENT = 5
DEFAULT CHARACTER SET = utf8
COLLATE = utf8_unicode_ci;


-- -----------------------------------------------------
-- Table `vrpn2`.`devices`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `vrpn2`.`devices` (
  `iddevice` INT(11) NOT NULL AUTO_INCREMENT,
  `device_name` VARCHAR(45) CHARACTER SET 'utf8' NOT NULL,
  PRIMARY KEY (`iddevice`))
ENGINE = InnoDB
AUTO_INCREMENT = 21
DEFAULT CHARACTER SET = utf8
COLLATE = utf8_unicode_ci;


-- -----------------------------------------------------
-- Table `vrpn2`.`patients`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `vrpn2`.`patients` (
  `idpatient` INT(11) NOT NULL AUTO_INCREMENT,
  `name` VARCHAR(245) CHARACTER SET 'utf8' NOT NULL,
  PRIMARY KEY (`idpatient`))
ENGINE = InnoDB
AUTO_INCREMENT = 11
DEFAULT CHARACTER SET = utf8
COLLATE = utf8_unicode_ci;


-- -----------------------------------------------------
-- Table `vrpn2`.`sessions`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `vrpn2`.`sessions` (
  `idsession` INT(11) NOT NULL AUTO_INCREMENT,
  `date_time` DATETIME NOT NULL,
  `idpatient` INT(11) NULL DEFAULT NULL,
  PRIMARY KEY (`idsession`),
  INDEX `fk_sessoes_usuario_idx` (`idpatient` ASC),
  CONSTRAINT `fk_sessoes_usuario`
    FOREIGN KEY (`idpatient`)
    REFERENCES `vrpn2`.`patients` (`idpatient`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB
AUTO_INCREMENT = 72
DEFAULT CHARACTER SET = utf8
COLLATE = utf8_unicode_ci;


-- -----------------------------------------------------
-- Table `vrpn2`.`action_history`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `vrpn2`.`action_history` (
  `idaction_history` INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `value` DOUBLE NOT NULL,
  `idaction` INT(10) UNSIGNED NOT NULL,
  `idsession` INT(11) NOT NULL,
  `iddevice` INT(11) NOT NULL,
  PRIMARY KEY (`idaction_history`),
  INDEX `fk_actionHistory_actions1_idx` (`idaction` ASC),
  INDEX `fk_actionHistory_sessions1_idx` (`idsession` ASC),
  INDEX `fk_actionHistory_devices1_idx` (`iddevice` ASC),
  CONSTRAINT `fk_actionHistory_actions1`
    FOREIGN KEY (`idaction`)
    REFERENCES `vrpn2`.`actions` (`idaction`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_actionHistory_devices1`
    FOREIGN KEY (`iddevice`)
    REFERENCES `vrpn2`.`devices` (`iddevice`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_actionHistory_sessions1`
    FOREIGN KEY (`idsession`)
    REFERENCES `vrpn2`.`sessions` (`idsession`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB
AUTO_INCREMENT = 1377
DEFAULT CHARACTER SET = utf8
COLLATE = utf8_unicode_ci;


-- -----------------------------------------------------
-- Table `vrpn2`.`alerts`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `vrpn2`.`alerts` (
  `idalert` INT(11) NOT NULL AUTO_INCREMENT,
  `idsession` INT(11) NOT NULL,
  `init_time` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`idalert`),
  INDEX `fk_alertas_sessoes1_idx` (`idsession` ASC),
  CONSTRAINT `fk_alertas_sessoes1`
    FOREIGN KEY (`idsession`)
    REFERENCES `vrpn2`.`sessions` (`idsession`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8
COLLATE = utf8_unicode_ci;


-- -----------------------------------------------------
-- Table `vrpn2`.`analogs`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `vrpn2`.`analogs` (
  `idanalog` INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `idsession` INT(11) NOT NULL,
  `iddevice` INT(11) NOT NULL,
  PRIMARY KEY (`idanalog`),
  INDEX `fk_analogs_sessions_idx` (`idsession` ASC),
  INDEX `fk_analogs_devices1_idx` (`iddevice` ASC),
  CONSTRAINT `fk_analogs_devices1`
    FOREIGN KEY (`iddevice`)
    REFERENCES `vrpn2`.`devices` (`iddevice`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_analogs_sessions`
    FOREIGN KEY (`idsession`)
    REFERENCES `vrpn2`.`sessions` (`idsession`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB
AUTO_INCREMENT = 2453
DEFAULT CHARACTER SET = utf8
COLLATE = utf8_unicode_ci;


-- -----------------------------------------------------
-- Table `vrpn2`.`buttons`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `vrpn2`.`buttons` (
  `idbutton` INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `idsession` INT(11) NOT NULL,
  `iddevice` INT(11) NOT NULL,
  `button` INT(11) NOT NULL,
  `pressed` INT(11) NULL DEFAULT NULL,
  PRIMARY KEY (`idbutton`),
  INDEX `fk_buttons_sessions1_idx` (`idsession` ASC),
  INDEX `fk_buttons_devices1_idx` (`iddevice` ASC),
  CONSTRAINT `fk_buttons_devices1`
    FOREIGN KEY (`iddevice`)
    REFERENCES `vrpn2`.`devices` (`iddevice`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_buttons_sessions1`
    FOREIGN KEY (`idsession`)
    REFERENCES `vrpn2`.`sessions` (`idsession`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB
AUTO_INCREMENT = 39
DEFAULT CHARACTER SET = utf8
COLLATE = utf8_unicode_ci;


-- -----------------------------------------------------
-- Table `vrpn2`.`channels`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `vrpn2`.`channels` (
  `idchannel` INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `num` INT(11) NOT NULL,
  `value` DOUBLE NOT NULL,
  `idanalog` INT(10) UNSIGNED NOT NULL,
  PRIMARY KEY (`idchannel`),
  INDEX `fk_channels_analogs1_idx` (`idanalog` ASC),
  CONSTRAINT `fk_channels_analogs1`
    FOREIGN KEY (`idanalog`)
    REFERENCES `vrpn2`.`analogs` (`idanalog`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB
AUTO_INCREMENT = 5891
DEFAULT CHARACTER SET = utf8
COLLATE = utf8_unicode_ci;


-- -----------------------------------------------------
-- Table `vrpn2`.`performance_test`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `vrpn2`.`performance_test` (
  `id` INT(11) NOT NULL AUTO_INCREMENT,
  `text` LONGTEXT CHARACTER SET 'utf8' NULL DEFAULT NULL,
  PRIMARY KEY (`id`))
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8
COLLATE = utf8_unicode_ci;


-- -----------------------------------------------------
-- Table `vrpn2`.`trackers`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `vrpn2`.`trackers` (
  `idtracker` INT(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `timestamp` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  `idsession` INT(11) NOT NULL,
  `sensor` INT(11) NOT NULL,
  `pos_0` FLOAT NOT NULL,
  `pos_1` FLOAT NOT NULL,
  `pos_2` FLOAT NOT NULL,
  `quat_0` FLOAT NULL DEFAULT NULL,
  `quat_1` FLOAT NULL DEFAULT NULL,
  `quat_2` FLOAT NULL DEFAULT NULL,
  `quat_3` FLOAT NULL DEFAULT NULL,
  `iddevice` INT(11) NOT NULL,
  PRIMARY KEY (`idtracker`),
  INDEX `fk_data_sessions1_idx` (`idsession` ASC),
  INDEX `fk_trackers_devices1_idx` (`iddevice` ASC),
  CONSTRAINT `fk_data_sessions1`
    FOREIGN KEY (`idsession`)
    REFERENCES `vrpn2`.`sessions` (`idsession`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_trackers_devices1`
    FOREIGN KEY (`iddevice`)
    REFERENCES `vrpn2`.`devices` (`iddevice`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB
AUTO_INCREMENT = 100465
DEFAULT CHARACTER SET = utf8
COLLATE = utf8_unicode_ci;


SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
