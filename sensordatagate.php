<?php
/**
 * Wemos D1 wifi temperature sensor json processor
 * @version 1.0
 *
 */

$dbname = '';
$dbuser = '';
$dbpassword = '';
$fp = fopen("post_log.txt","a");

if (isset($_POST['json'])) {   
    $raw = $_POST['json'];
    $json = json_decode($raw, true);
    fwrite($fp, $json. PHP_EOL);
    if (json_last_error() == JSON_ERROR_NONE) {
        try {
            $pdo = new PDO('mysql:host=localhost;dbname='.$dbname, $dbuser, $dbpassword);
            $sql = 'INSERT INTO iot_temperature_raw '
                  .'(millis, station_id, humidity, temperature, heat_index) '
                  .'VALUES (:millis, :station_id, :humidity, :temperature, :heat_index) ';
            $st = $pdo->prepare($sql);
            if (
                is_numeric($json['millis']) && 
                is_numeric($json['humidity']) && 
                is_numeric($json['temperature']) && 
                is_numeric($json['heatIndex'])
            ) {
                $st->bindParam(':millis', $json['millis']);
                $st->bindParam(':station_id', $json['station_id']);
                $st->bindParam(':humidity', $json['humidity']);
                $st->bindParam(':temperature', $json['temperature']);
                $st->bindParam(':heat_index', $json['heatIndex']);
                if ($st->execute()) {
                    echo 'OK';
                } else {
                    echo 'ERROR';            
                    fwrite($fp, 'insert got error.'. PHP_EOL);
                }                
            } else {
                $st->bindParam(':millis', NULL);
                $st->bindParam(':station_id', $json['station_id']);
                $st->bindParam(':humidity', NULL);
                $st->bindParam(':temperature', NULL);
                $st->bindParam(':heat_index', NULL);
                $st->execute();
                echo 'ERROR';                            
                fwrite($fp, 'Missing data'. PHP_EOL);
            }
        } catch (PDOException $e) {
            echo 'Connection failed: ' . $e->getMessage();
            fwrite($fp, 'Connection failed: ' . $e->getMessage(). PHP_EOL);
        }
    } else {
        echo 'ERROR';
        fwrite($fp, 'json error:'.json_last_error_msg(). PHP_EOL);
        
    }
} else {
    echo 'ERROR';
    fwrite($fp, 'No json POST variable.'. PHP_EOL);
}
fclose($fp);
